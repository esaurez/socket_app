#include "utils.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <chrono>
#include <functional>
#include <random>

void populateRandomBuffer(char *buffer, int size, std::function<char()> &gen_func) {
		for (int i = 0; i < size - 1; ++i) {
				buffer[i] = gen_func();
		}
		buffer[size - 1] = '\0';
}

void measureLatency(int client_socket, int packet_size, int frequency, std::function<char()> &gen_func) {
		if(packet_size > 1024 || packet_size < 1) {
			throw std::runtime_error("Invalid packet size"); 		
		}

    char buffer[1024] = {0};
		populateRandomBuffer(buffer, packet_size, gen_func);

		auto start = rdtsc();

    // Send the packet to the server
    send(client_socket, buffer, packet_size, 0);

    // Receive the echoed packet from the server
    recv(client_socket, buffer, packet_size, 0);

    auto end = rdtsc(); 

    // Calculate and print the latency
		double duration = static_cast<double>(end - start) / (double)frequency;
		std::cout << packet_size << ", " << duration << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT> <PACKET_SIZE> <NUM_TESTS>" << std::endl;
        return -1;
    }

		std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255); // Change the range as needed

    const char *ip_address = argv[1];
    auto port = static_cast<uint16_t>(std::stoi(argv[2]));
    int packet_size = std::stoi(argv[3]);
    int num_tests = std::stoi(argv[4]);

		auto frequency = time_calibrate_tsc();

    int client_socket;
    struct sockaddr_in server_addr;

    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }

    // Set up the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_port = htons(port);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed." << std::endl;
        return -1;
    }

    std::cout << "Packet size, latency (us)" << std::endl;

		std::function<char()> gen_func = [&dis, &gen]() { return static_cast<char>(dis(gen));};
    for (int i = 0; i < num_tests; ++i) {
        measureLatency(client_socket, packet_size, frequency,  gen_func);
    }

    // Close the socket
    close(client_socket);

    return 0;
}
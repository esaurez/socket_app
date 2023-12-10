#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

void handle_client(int client_socket) {
    while (true) {
        char buffer[1024] = {0};
        ssize_t valread;

        // Read data from the client
        valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        // Echo the received data back to the client
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // Close the client socket
    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT>" << std::endl;
        return -1;
    }

    const char *ip_address = argv[1];
    auto port = static_cast<uint16_t>(std::stoi(argv[2]));

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }

    // Set up the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_port = htons(port);

    // Bind the socket to the specified port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Binding failed." << std::endl;
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) < 0) {
        std::cerr << "Listening failed." << std::endl;
        return -1;
    }

    std::cout << "Server listening on " << ip_address << ":" << port << std::endl;

    while (true) {
        // Accept a client connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            std::cerr << "Acceptance failed." << std::endl;
            return -1;
        }

        std::cout << "Connection accepted from client." << std::endl;

        // Handle client connection in a separate thread
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach(); // Detach the thread to run independently
    }

    return 0;
}

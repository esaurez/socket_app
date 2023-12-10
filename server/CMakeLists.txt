message(STATUS "Configuring server_exe")

add_executable(server_exe main.cpp)

set_target_properties(server_exe
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
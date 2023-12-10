message(STATUS "Configuring client_exe")

add_executable(client_exe main.cpp)

set_target_properties(client_exe
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
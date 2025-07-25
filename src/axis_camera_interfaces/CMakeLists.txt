cmake_minimum_required(VERSION 3.10)
project(axis_camera_interfaces)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(sensor_msgs REQUIRED)

set(MSG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/msg")
file(GLOB MSG_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${MSG_DIR}/*.msg")

rosidl_generate_interfaces(${PROJECT_NAME}
  ${MSG_FILES}
  DEPENDENCIES sensor_msgs)

ament_package()

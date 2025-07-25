cmake_minimum_required(VERSION 3.10)
project(ptz_camera_driver)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")

cmake_policy(SET CMP0146 OLD)

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

set(THREADS_PREFER_PTHREAD_FLAG ON)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(axis_camera_interfaces REQUIRED)
find_package(camera_calibration_parsers REQUIRED)
find_package(camera_info_manager REQUIRED)
find_package(cpr REQUIRED)
find_package(image_transport REQUIRED)
find_package(OpenCV 4.11 REQUIRED)
find_package(params_manager_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(Threads REQUIRED)

add_executable(ptz_camera_driver src/ptz_camera_driver.cpp)
target_include_directories(ptz_camera_driver PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(ptz_camera_driver PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
target_link_libraries(ptz_camera_driver
  cpr::cpr
  Threads::Threads)
ament_target_dependencies(
  ptz_camera_driver
  "axis_camera_interfaces"
  "camera_calibration_parsers"
  "camera_info_manager"
  "image_transport"
  "OpenCV"
  "params_manager_cpp"
  "rclcpp"
  "rclcpp_components"
  "sensor_msgs"
  "std_msgs"
  "std_srvs"
)

install(TARGETS ptz_camera_driver
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()

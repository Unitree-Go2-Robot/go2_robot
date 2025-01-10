cmake_minimum_required(VERSION 3.12.2)

project(go2_image_transport)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(STATUS "GNU or Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

# Find ffmpeg libraries
find_package(PkgConfig)
pkg_check_modules(PC_AVDEVICE REQUIRED libavdevice>=58)
pkg_check_modules(PC_AVFORMAT REQUIRED libavformat>=58)
pkg_check_modules(PC_AVCODEC REQUIRED libavcodec>=58)
pkg_check_modules(PC_AVUTIL REQUIRED libavutil>=56)
pkg_check_modules(PC_SWSCALE REQUIRED libswscale>=5)

ament_auto_add_library(
  ${PROJECT_NAME} SHARED
  src/go2_subscriber.cpp
  src/manifest.cpp
)

target_link_libraries(
  ${PROJECT_NAME}
  ${PC_AVDEVICE_LIBRARIES}
  ${PC_AVFORMAT_LIBRARIES}
  ${PC_AVCODEC_LIBRARIES}
  ${PC_AVUTIL_LIBRARIES}
  ${PC_SWSCALE_LIBRARIES}
)

# This will load & run linters listed in package.xml
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

install(
  DIRECTORY include
  DESTINATION include
)

install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

pluginlib_export_plugin_description_file(image_transport go2_plugins.xml)

ament_auto_package()

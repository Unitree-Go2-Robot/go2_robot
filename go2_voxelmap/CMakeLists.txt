cmake_minimum_required(VERSION 3.8)
project(go2_voxelmap)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(unitree_go REQUIRED)

# Include directories
include_directories(include)

# Agressive optimizations
add_compile_options(-O3)

# Add the executable
add_executable(voxelmap_node
  src/voxelmap_node.cpp
  src/lidar_decoder.cpp
  src/voxel.c
  src/wasm-rt-impl.c
  src/wasm-rt-mem-impl.c
  src/point_cloud_creator.cpp
  include/voxel.h
  include/voxelmap_node.hpp
  include/lidar_decoder.hpp)

# Link libraries
ament_target_dependencies(voxelmap_node rclcpp sensor_msgs std_msgs unitree_go)

# Install executable
install(TARGETS voxelmap_node
  DESTINATION lib/${PROJECT_NAME}
)

# Export package
ament_package()

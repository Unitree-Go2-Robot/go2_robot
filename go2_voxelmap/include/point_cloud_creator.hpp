// BSD 3-Clause License

// Copyright (c) 2024, Intelligent Robotics Lab
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <vector>
#include <stdexcept>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

/**
 * @class PointCloudCreator
 * @brief Converts Unitree Go2 Robot Voxel Map data to ROS2 PointCloud2
 *
 * This class is specifically designed to process compressed voxel map data
 * from Unitree Go2 robots. It handles the conversion of raw voxel map buffers
 * (containing positions, UVs, and indices) into a standard ROS2 point cloud
 * representation.
 *
 * The input buffer is a continuous memory block that contains:
 * - Positions: 3D coordinates of points (scaled by resolution)
 * - UVs: Texture coordinates (currently unused)
 * - Indices: Point connectivity information (currently unused)
 *
 * The class applies spatial resolution scaling and filters out zero-value points.
 *
 * @note This is part of a WebAssembly-based decoding pipeline for Unitree Go2
 * robot's compressed voxel map data.
 */
class PointCloudCreator
{
public:
  /**
   * @brief Construct a new Point Cloud Creator for Unitree Go2 Voxel Maps
   *
   * @param heap Pointer to the continuous buffer of voxel map data
   * @param heap_size Size of the buffer to prevent out-of-bounds access
   * @param positions Offset to the positions data in the heap
   * @param point_count Total number of points in the voxel map
   * @param resolution Spatial resolution of the point cloud
   * @throws std::invalid_argument If input parameters are invalid
   */
  PointCloudCreator(
    const uint8_t * heap,
    size_t heap_size,
    uint32_t positions,
    size_t point_count,
    double resolution,
    std::array<double, 3> position_offset
  );

  /**
   * @brief Create a ROS2 PointCloud2 message from the Unitree Go2 Voxel Map
   *
   * @return sensor_msgs::msg::PointCloud2 Processed point cloud message
   * @throws std::runtime_error If point cloud creation fails
   */
  sensor_msgs::msg::PointCloud2 createPointCloud();

private:
  /**
   * @brief Validate the input voxel map data
   *
   * Checks if the input parameters are within acceptable ranges
   * @throws std::invalid_argument If validation fails
   */
  void validateInputData() const;

  /**
   * @brief Process and transform voxel map points to world coordinates
   *
   * @param cloud_msg Reference to the point cloud message to populate
   * @return size_t Number of valid points added to the point cloud
   */
  size_t processPoints(sensor_msgs::msg::PointCloud2 & cloud_msg);

  // Member variables representing Unitree Go2 voxel map data
  const uint8_t * heap_;             // Pointer to continuous buffer of voxel map data
  size_t heap_size_;                 // Size of the buffer
  uint32_t positionsOffset_;         // Offset to positions in the buffer
  size_t pointCount_;                // Total number of points
  double resolution_;                // Spatial resolution scalar
  std::array<double, 3> position_offset_; // Offset to the odom of the voxel map

  // Logging helper
  rclcpp::Logger logger_ = rclcpp::get_logger("point_cloud_creator");
};

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

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <unitree_go/msg/voxel_map_compressed.hpp>

#include "lidar_decoder.hpp"
#include "point_cloud_creator.hpp"

/**
 * @class VoxelMapNode
 * @brief ROS2 Node for processing Unitree Go2 Voxel Map data
 *
 * This node:
 * 1. Subscribes to compressed voxel map messages
 * 2. Decodes the compressed data using WebAssembly
 * 3. Converts the decoded data to a ROS2 point cloud
 * 4. Publishes the point cloud
 *
 * Designed specifically for Unitree Go2 robot's voxel map transmission
 */
class VoxelMapNode : public rclcpp::Node
{
public:
  /**
   * @brief Construct a new Voxel Map Node
   */
  VoxelMapNode();

private:
  /**
   * @brief Callback for incoming compressed voxel map messages
   *
   * @param msg Compressed voxel map message from Unitree Go2
   */
  void voxelMapCallback(const unitree_go::msg::VoxelMapCompressed::SharedPtr msg);

  // ROS2 communication interfaces
  rclcpp::Subscription<unitree_go::msg::VoxelMapCompressed>::SharedPtr voxelMapSubscription_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pointCloudPublisher_;

  // Decoder and point cloud creator
  LidarDecoder decoder_;
};

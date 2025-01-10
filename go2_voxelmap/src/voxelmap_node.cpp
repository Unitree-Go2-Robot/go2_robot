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

#include "voxelmap_node.hpp"

VoxelMapNode::VoxelMapNode()
: Node("voxelmap_node")
{
  RCLCPP_INFO(this->get_logger(), "Initializing VoxelMap Node for Unitree Go2");

  // Subscribe to compressed voxel map topic
  voxelMapSubscription_ = this->create_subscription<unitree_go::msg::VoxelMapCompressed>(
    "/utlidar/voxel_map_compressed",
    10,
    std::bind(&VoxelMapNode::voxelMapCallback, this, std::placeholders::_1)
  );

  // Publish processed point clouds
  pointCloudPublisher_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(
    "voxel_point_cloud",
    10
  );
}

void VoxelMapNode::voxelMapCallback(const unitree_go::msg::VoxelMapCompressed::SharedPtr msg)
{
  RCLCPP_DEBUG(
    this->get_logger(),
    "Received VoxelMapCompressed message: %d:%d:%d dimensions, resolution %f",
    msg->width[0], msg->width[1], msg->width[2],
    msg->resolution
  );

  try {
    // Decode the compressed voxel map
    decoder_.decode(
      msg->data,
      msg->resolution,
      msg->width[2]        // Z-axis origin
    );


    // Create point cloud from decoded data
    PointCloudCreator creator(
      decoder_.getBuffer(),
      decoder_.getMemorySize(),
      decoder_.getPositionsOffset(),
      LidarDecoder::MAX_POINTS,
      msg->resolution,
      msg->origin
    );

    // Publish the point cloud
    pointCloudPublisher_->publish(creator.createPointCloud());
  } catch (const std::exception & e) {
    RCLCPP_ERROR(
      this->get_logger(),
      "Error processing Unitree Go2 voxel map: %s",
      e.what()
    );
  }
}

// Main function to initialize ROS2 and spin the node
int main(int argc, char ** argv)
{
  // Initialize ROS2 and WebAssembly runtime
  rclcpp::init(argc, argv);

  // Create and spin the node
  rclcpp::spin(std::make_shared<VoxelMapNode>());

  // Shutdown ROS2
  rclcpp::shutdown();
  return 0;
}

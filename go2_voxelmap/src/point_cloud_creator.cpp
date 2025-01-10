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

#include "point_cloud_creator.hpp"
#include <cstring>

PointCloudCreator::PointCloudCreator(
  const uint8_t * heap,
  size_t heap_size,
  uint32_t positions,
  size_t point_count,
  double resolution,
  std::array<double, 3> position_offset
)
: heap_(heap),
  heap_size_(heap_size),
  positionsOffset_(positions),
  pointCount_(point_count),
  resolution_(resolution)
{
  position_offset_ = position_offset;
  validateInputData();
}

void PointCloudCreator::validateInputData() const
{
  if (heap_ == nullptr) {
    throw std::invalid_argument("Unitree Go2 voxel map buffer is null");
  }

  if (heap_size_ == 0) {
    throw std::invalid_argument("Unitree Go2 voxel map buffer is empty");
  }

  if (resolution_ <= 0.0) {
    throw std::invalid_argument("Invalid resolution: must be positive");
  }

  if (pointCount_ == 0) {
    RCLCPP_WARN(logger_, "Voxel map point count is zero. No points will be processed.");
  }
}

size_t PointCloudCreator::processPoints(sensor_msgs::msg::PointCloud2 & cloud_msg)
{
  size_t validPoints = 0;

  for (size_t i = 0; i < pointCount_; ++i) {
    // Safely access point coordinates from the Unitree Go2 voxel map buffer
    uint8_t ui_x = (i * 3 + 0 < heap_size_) ? heap_[positionsOffset_ + i * 3] : 0;
    uint8_t ui_y = (i * 3 + 1 < heap_size_) ? heap_[positionsOffset_ + i * 3 + 1] : 0;
    uint8_t ui_z = (i * 3 + 2 < heap_size_) ? heap_[positionsOffset_ + i * 3 + 2] : 0;

    // Skip zero points (common in sparse voxel representations)
    if (ui_x == 0 && ui_y == 0 && ui_z == 0) {
      continue;
    }

    // Transform coordinates using Unitree Go2 specific resolution scaling and offset
    float x = static_cast<float>(ui_x * resolution_) + position_offset_[0];
    float y = static_cast<float>(ui_y * resolution_) + position_offset_[1];
    float z = static_cast<float>(ui_z * resolution_) + position_offset_[2];

    // Copy to point cloud buffer
    size_t offset = validPoints * cloud_msg.point_step;
    std::memcpy(&cloud_msg.data[offset], &x, sizeof(float));         // x
    std::memcpy(&cloud_msg.data[offset + 4], &y, sizeof(float));     // y
    std::memcpy(&cloud_msg.data[offset + 8], &z, sizeof(float));     // z

    ++validPoints;
  }

  RCLCPP_DEBUG(
    logger_,
    "Processed %zu valid points from Unitree Go2 voxel map",
    validPoints
  );

  return validPoints;
}

sensor_msgs::msg::PointCloud2 PointCloudCreator::createPointCloud()
{
  sensor_msgs::msg::PointCloud2 cloudMsg;

  // Set frame and timestamp
  cloudMsg.header.frame_id = "odom";
  cloudMsg.header.stamp = rclcpp::Clock().now();

  // Configure point cloud metadata for Unitree Go2 voxel map
  cloudMsg.height = 1;            // Unorganized point cloud
  cloudMsg.is_dense = true;       // No invalid points
  cloudMsg.is_bigendian = false;

  // Define point fields (x, y, z) as per ROS2 PointCloud2 standard
  cloudMsg.fields.resize(3);

  cloudMsg.fields[0].name = "x";
  cloudMsg.fields[0].offset = 0;
  cloudMsg.fields[0].datatype = sensor_msgs::msg::PointField::FLOAT32;
  cloudMsg.fields[0].count = 1;

  cloudMsg.fields[1].name = "y";
  cloudMsg.fields[1].offset = 4;
  cloudMsg.fields[1].datatype = sensor_msgs::msg::PointField::FLOAT32;
  cloudMsg.fields[1].count = 1;

  cloudMsg.fields[2].name = "z";
  cloudMsg.fields[2].offset = 8;
  cloudMsg.fields[2].datatype = sensor_msgs::msg::PointField::FLOAT32;
  cloudMsg.fields[2].count = 1;

  // Configure point and row steps
  cloudMsg.point_step = 12;    // 3 floats * 4 bytes each
  cloudMsg.width = pointCount_;
  cloudMsg.data.resize(cloudMsg.point_step * cloudMsg.width);

  // Process points and get valid point count
  cloudMsg.width = processPoints(cloudMsg);
  cloudMsg.row_step = cloudMsg.point_step * cloudMsg.width;
  cloudMsg.data.resize(cloudMsg.row_step);

  RCLCPP_DEBUG(
    logger_,
    "Created point cloud from Unitree Go2 voxel map with %u valid points",
    cloudMsg.width
  );

  return cloudMsg;
}

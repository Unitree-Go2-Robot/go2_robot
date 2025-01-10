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

#include "lidar_decoder.hpp"
#include <rclcpp/rclcpp.hpp>

// WebAssembly function declarations (from generated Wasm-to-C bindings)
extern "C" {
void wasm2c_libvoxel_instantiate(
  w2c_libvoxel * instance,
  struct w2c_a * w2c_a_instance);
void wasm2c_libvoxel_free(w2c_libvoxel * instance);
wasm_rt_memory_t * w2c_libvoxel_c(struct w2c_libvoxel * instance);

u32 w2c_a_a(struct w2c_a * instance, uint32_t t)
{
  wasm_rt_memory_t * memory = w2c_libvoxel_c((struct w2c_libvoxel *)instance);
  return memory->size;
}

void w2c_a_b(
  struct w2c_a * instance, uint32_t target, uint32_t start,
  uint32_t end)
{
  wasm_rt_memory_t * memory = w2c_libvoxel_c((struct w2c_libvoxel *)instance);
  uint8_t * heap = memory->data;

  // Calculate the number of bytes to copy
  uint32_t length = end - start;

  // Ensure the copy operation stays within bounds
  if (start >= memory->size || end > memory->size || target >= memory->size ||
    target + length > memory->size)
  {
    return;
  }

  // Use memmove to handle overlapping ranges
  memmove(&heap[start], &heap[start], length);
}
}

LidarDecoder::LidarDecoder()
{
  // Initialize WebAssembly runtime
  wasm_rt_init();

  // Instantiate WebAssembly module
  wasm2c_libvoxel_instantiate(&instance_, w2c_a_instance_);

  // Get memory management details
  auto * memory = w2c_libvoxel_c(&instance_);
  buffer_ = memory->data;
  memory_size_ = memory->size;

  // Allocate memory offsets in WebAssembly memory
  input_ = w2c_libvoxel_f(&instance_, 61440);
  decompressBuffer_ = w2c_libvoxel_f(&instance_, DECOMPRESS_BUFFER_SIZE);
  positions_ = w2c_libvoxel_f(&instance_, MAX_POINTS * 12);
  uvs_ = w2c_libvoxel_f(&instance_, MAX_POINTS * 8);
  indices_ = w2c_libvoxel_f(&instance_, MAX_POINTS * 24);
  decompressedSize_ = w2c_libvoxel_f(&instance_, 4);
  faceCount_ = w2c_libvoxel_f(&instance_, 4);
  pointCount_ = w2c_libvoxel_f(&instance_, 4);
}

LidarDecoder::~LidarDecoder()
{
  // Clean up WebAssembly resources
  wasm2c_libvoxel_free(&instance_);
}

uint32_t LidarDecoder::getMemorySize(struct w2c_a * instance, uint32_t target)
{
  wasm_rt_memory_t * memory =
    w2c_libvoxel_c(reinterpret_cast<struct w2c_libvoxel *>(instance));
  return memory->size;
}

void LidarDecoder::copyMemory(
  struct w2c_a * instance, uint32_t target,
  uint32_t start, uint32_t end)
{
  wasm_rt_memory_t * memory =
    w2c_libvoxel_c(reinterpret_cast<struct w2c_libvoxel *>(instance));
  uint8_t * heap = memory->data;

  // Calculate the number of bytes to copy
  uint32_t length = end - start;

  // Ensure the copy operation stays within bounds
  if (start >= memory->size || end > memory->size || target >= memory->size ||
    target + length > memory->size)
  {
    RCLCPP_WARN(
      rclcpp::get_logger("LidarDecoder"),
      "Memory copy operation out of bounds");
    return;
  }

  // Use memmove to handle overlapping ranges safely
  std::memmove(&heap[target], &heap[start], length);
}

void LidarDecoder::decode(
  const std::vector<uint8_t> & compressed_data,
  float resolution, float origin_z)
{
  // Validate input data size
  if (compressed_data.size() > memory_size_) {
    throw std::runtime_error("Compressed data exceeds WebAssembly memory size");
  }

  // Copy compressed data into WebAssembly memory
  std::memcpy(&buffer_[input_], compressed_data.data(), compressed_data.size());

  // Calculate z-axis offset based on resolution
  int z_offset = static_cast<int>(origin_z / resolution);

  // Invoke WebAssembly decoding function
  w2c_libvoxel_e(
    &instance_, input_, compressed_data.size(),
    DECOMPRESS_BUFFER_SIZE, decompressBuffer_, decompressedSize_,
    positions_, uvs_, indices_, faceCount_, pointCount_, z_offset);

  // Retrieve decoded metrics
  int point_count = *reinterpret_cast<int *>(&buffer_[pointCount_]);
  int face_count = *reinterpret_cast<int *>(&buffer_[faceCount_]);

  // Log decoding results
  RCLCPP_DEBUG(
    rclcpp::get_logger("LidarDecoder"),
    "Decoded %d points and %d faces", point_count, face_count);
}

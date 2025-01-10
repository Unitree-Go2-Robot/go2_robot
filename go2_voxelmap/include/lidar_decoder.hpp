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

// WebAssembly runtime header
#include <wasm-rt.h>
#include <voxel.h>

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <memory>


/**
 * @class LidarDecoder
 * @brief Manages WebAssembly-based decoding of Unitree Go2 voxel map data
 *
 * This class handles the decoding of compressed voxel map data using a
 * WebAssembly module. It provides an interface for:
 * - Initializing WebAssembly runtime
 * - Managing WebAssembly memory
 * - Decoding compressed point cloud data
 *
 * The decoder uses a custom WebAssembly module (libvoxel) to:
 * 1. Decompress input data
 * 2. Extract point positions
 * 3. Prepare data for point cloud conversion
 *
 * @note This class is specifically designed for Unitree Go2 robot's
 * compressed voxel map decoding pipeline.
 */
class LidarDecoder
{
public:
  /**
   * @brief Construct a new Lidar Decoder
   *
   * Initializes the WebAssembly runtime and allocates necessary memory
   *
   * @throws std::runtime_error If WebAssembly initialization fails
   */
  LidarDecoder();

  /**
   * @brief Destructor to clean up WebAssembly resources
   */
  ~LidarDecoder();

  /**
   * @brief Decode compressed voxel map data
   *
   * @param compressed_data Raw compressed voxel map data
   * @param resolution Spatial resolution of the voxel map
   * @param origin_z Z-axis origin offset
   * @param positions_vec Output vector for point positions
   * @param uvs_vec Output vector for UV coordinates
   * @param indices_vec Output vector for point indices
   *
   * @throws std::runtime_error If decoding fails
   */
  void decode(
    const std::vector<uint8_t> & compressed_data,
    float resolution,
    float origin_z
  );

  /**
   * @brief Get a pointer to the decoder's memory buffer
   *
   * @return uint8_t* Pointer to the WebAssembly memory buffer
   */
  [[nodiscard]] uint8_t * getBuffer() {return buffer_;}

  /**
   * @brief Get the total size of the memory buffer
   *
   * @return size_t Size of the WebAssembly memory buffer
   */
  [[nodiscard]] size_t getMemorySize() const {return memory_size_;}

  /**
   * @brief Get the offset of positions in the memory buffer
   *
   * @return uint32_t Positions buffer offset
   */
  [[nodiscard]] uint32_t getPositionsOffset() const {return positions_;}

  /**
   * @brief Get the total number of points
   *
   * @return uint32_t Number of points in the decoded voxel map
   */
  [[nodiscard]] uint32_t getPointCount() const
  {
    return *reinterpret_cast<int *>(&buffer_[pointCount_]);
  }

  /**
   * @brief Get the total number of points
   *
   * @return uint32_t Number of points in the decoded voxel map
   */
  [[nodiscard]] uint32_t getFaceCount() const
  {
    return *reinterpret_cast<int *>(&buffer_[faceCount_]);
  }

private:
  // WebAssembly specific methods for memory management
  static uint32_t getMemorySize(struct w2c_a * instance, uint32_t target);
  static void copyMemory(
    struct w2c_a * instance, uint32_t target,
    uint32_t start, uint32_t end);

  // WebAssembly runtime objects
  w2c_libvoxel instance_;
  struct w2c_a * w2c_a_instance_;

  // Memory management
  uint8_t * buffer_;
  size_t memory_size_;

  // WebAssembly memory offsets
  uint32_t input_;
  uint32_t decompressBuffer_;
  uint32_t positions_;
  uint32_t uvs_;
  uint32_t indices_;
  uint32_t decompressedSize_;
  uint32_t faceCount_;
  uint32_t pointCount_;

  // Configuration constants
  static constexpr uint32_t DECOMPRESS_BUFFER_SIZE = 80000;

public:
  static constexpr uint32_t MAX_POINTS = 240000;
};

// Copyright 2025 Tier IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#pragma once

#include <cuda_utils/cuda_unique_ptr.hpp>
#include <cuda_utils/stream_unique_ptr.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <tensorrt_common/tensorrt_common.hpp>
#include <vector>

using cuda_utils::CudaUniquePtr;
using cuda_utils::CudaUniquePtrHost;
using cuda_utils::makeCudaStream;
using cuda_utils::StreamUniquePtr;

class SAM2ImageEncoder {
public:
  // Constructor
  SAM2ImageEncoder(const std::string &onnx_path,
                   const std::string &engine_precision,
                   const tensorrt_common::BatchConfig &batch_config,
                   const size_t max_workspace_size,
                   const tensorrt_common::BuildConfig build_config);

  ~SAM2ImageEncoder();

  // Encode images
  void EncodeImage(const std::vector<cv::Mat> &images);

  bool Infer(std::vector<void*> &_buffers);

  // High-resolution features after encoding
  CudaUniquePtrHost<float[]> feats_0_data;
  CudaUniquePtrHost<float[]> feats_1_data;
  CudaUniquePtrHost<float[]> embed_data;

  // Input dimensions
  int input_height_;
  int input_width_;

  // Model precision (fp16 or fp32)
  std::string encoder_precision_;

  // Batch size
  int batch_size_;
  int feats_0_size_;
  int feats_1_size_;
  int embed_size_;

private:
  // Allocate GPU memory
  void AllocateGPUMemory(bool _CPU = true);

  // Get input details from ONNX model
  void GetInputDetails();

  // Prepare input tensor
  cv::Mat Preprocess(const std::vector<cv::Mat> &images);

  // Execute inference
  bool Infer(const cv::Mat &input_tensor);



  std::unique_ptr<tensorrt_common::TrtCommon> trt_encoder_;

  CudaUniquePtr<float[]> input_d_;
  CudaUniquePtr<float[]> feats_0_data_d_;
  CudaUniquePtr<float[]> feats_1_data_d_;
  CudaUniquePtr<float[]> embed_data_d_;

  StreamUniquePtr stream_{makeCudaStream()};
};

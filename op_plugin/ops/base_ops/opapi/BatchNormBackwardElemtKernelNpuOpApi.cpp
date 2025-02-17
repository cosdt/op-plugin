// Copyright (c) 2023 Huawei Technologies Co., Ltd
// All rights reserved.
//
// Licensed under the BSD 3-Clause License  (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "op_plugin/AclOpsInterface.h"
#include "op_plugin/OpApiInterface.h"
#include "op_plugin/utils/op_api_common.h"

namespace op_api {
using npu_preparation = at_npu::native::OpPreparation;

at::Tensor batch_norm_backward_elemt(const at::Tensor& grad_out, const at::Tensor& input, const at::Tensor& mean,
                                     const at::Tensor& invstd, const c10::optional<at::Tensor>& weight_opt,
                                     const at::Tensor& sum_dy, const at::Tensor& sum_dy_xmu, const at::Tensor& count) {
  DO_COMPATIBILITY(
      aclnnBatchNormElemtBackward,
      acl_op::batch_norm_backward_elemt(grad_out, input, mean, invstd, weight_opt, sum_dy, sum_dy_xmu, count));

  at::Tensor grad_input = npu_preparation::apply_tensor_without_format(input);
  EXEC_NPU_CMD(aclnnBatchNormElemtBackward, grad_out, input, mean, invstd, weight_opt, sum_dy, sum_dy_xmu, count,
               grad_input);
  return grad_input;
}
}  // namespace op_api

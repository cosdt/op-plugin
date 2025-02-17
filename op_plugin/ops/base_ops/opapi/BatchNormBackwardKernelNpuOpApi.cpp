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

std::tuple<at::Tensor, at::Tensor, at::Tensor> native_batch_norm_backward(
    const at::Tensor& grad_out, const at::Tensor& self, const c10::optional<at::Tensor>& weight_opt,
    const c10::optional<at::Tensor>& running_mean_opt, const c10::optional<at::Tensor>& running_var_opt,
    const c10::optional<at::Tensor>& save_mean_opt, const c10::optional<at::Tensor>& save_invstd_opt, bool train,
    double eps, std::array<bool, 3> grad_input_mask) {
  DO_COMPATIBILITY(aclnnBatchNormBackward,
                   acl_op::native_batch_norm_backward(grad_out, self, weight_opt, running_mean_opt, running_var_opt,
                                                      save_mean_opt, save_invstd_opt, train, eps, grad_input_mask));
  // grad_input_mask
  at::Tensor grad_input;
  at::Tensor grad_weight;
  at::Tensor grad_bias;
  if (grad_input_mask[0]) {
    grad_input = npu_preparation::apply_tensor_without_format(self.sizes(), self.options());
  }
  if (grad_input_mask[1]) {
    grad_weight = npu_preparation::apply_tensor_without_format({self.size(1)}, self.options().dtype(at::kFloat));
  }
  if (grad_input_mask[2]) {
    grad_bias = npu_preparation::apply_tensor_without_format({self.size(1)}, self.options().dtype(at::kFloat));
  }

  // calculate the output result of the NPU
  EXEC_NPU_CMD(aclnnBatchNormBackward, grad_out, self, weight_opt, running_mean_opt, running_var_opt, save_mean_opt,
               save_invstd_opt, train, eps, grad_input_mask, grad_input, grad_weight, grad_bias);
  return std::make_tuple(grad_input, grad_weight, grad_bias);
}
}  // namespace op_api

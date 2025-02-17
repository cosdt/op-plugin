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

at::Tensor& max_unpool3d_out(
    const at::Tensor& self,
    const at::Tensor& indices,
    at::IntArrayRef output_size,
    at::IntArrayRef stride,
    at::IntArrayRef padding,
    at::Tensor& result) {
  DO_COMPATIBILITY(aclnnMaxUnpool3d, acl_op::max_unpool3d_out(self, indices, output_size, stride, padding, result));
  auto out_shape = op_infer::max_pool3d_output_size(self, output_size);
  npu_preparation::check_tensor({self, indices}, result, self.scalar_type(), out_shape);
  EXEC_NPU_CMD(aclnnMaxUnpool3d, self, indices, output_size, stride, padding, result);
  return result;
}

at::Tensor max_unpool3d(
    const at::Tensor& self,
    const at::Tensor& indices,
    at::IntArrayRef output_size,
    at::IntArrayRef stride,
    at::IntArrayRef padding) {
  DO_COMPATIBILITY(aclnnMaxUnpool3d, acl_op::max_unpool3d(self, indices, output_size, stride, padding));
  auto out_shape = op_infer::max_pool3d_output_size(self, output_size);
  at::Tensor result = npu_preparation::apply_tensor_without_format(self, out_shape);
  EXEC_NPU_CMD(aclnnMaxUnpool3d, self, indices, output_size, stride, padding, result);
  return result;
}
} // namespace op_api

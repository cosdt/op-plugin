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

at::Tensor& gcd_out(const at::Tensor& self, const at::Tensor& other, at::Tensor& result) {
  DO_COMPATIBILITY(aclnnGcd, acl_op::gcd_out(self, other, result));
  auto output_size_vec = op_infer::broadcast_ops_npu_output_size(self, other);
  at::IntArrayRef output_size = output_size_vec;
  // Shape of result must be the same as broadcastshape of self and other, dtype has no limitation.
  if (result.sizes() != output_size) {
    result.resize_(output_size);
  }
  // dispatch hostAPI
  EXEC_NPU_CMD(aclnnGcd, self, other, result);
  return result;
}

} // namespace op_api

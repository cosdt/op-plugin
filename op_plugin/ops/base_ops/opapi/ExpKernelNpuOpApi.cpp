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

at::Tensor& exp_out(const at::Tensor& self, at::Tensor& result) {
  DO_COMPATIBILITY(aclnnExp, acl_op::exp_out(self, result));
  npu_preparation::check_tensor({self}, result, self.sizes());
  EXEC_NPU_CMD(aclnnExp, self, result);
  return result;
}

at::Tensor& exp_(at::Tensor& self) {
  DO_COMPATIBILITY(aclnnInplaceExp, acl_op::exp_(self));
  EXEC_NPU_CMD(aclnnInplaceExp, self);
  return self;
}

at::Tensor exp(const at::Tensor& self) {
  DO_COMPATIBILITY(aclnnExp, acl_op::exp(self));

  at::Tensor result;
  if (self.scalar_type() == at::ScalarType::Bool || self.scalar_type() == at::ScalarType::Long) {
    result = npu_preparation::apply_tensor_without_format(self.sizes(), self.options().dtype(at::kFloat));
  } else {
    result = npu_preparation::apply_tensor_without_format(self);
  }

  EXEC_NPU_CMD(aclnnExp, self, result);
  return result;
}
} // namespace op_api

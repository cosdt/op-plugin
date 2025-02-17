// Copyright (c) 2023 Huawei Technologies Co., Ltd
// All rights reserved.
//
// Licensed under the BSD 3-Clause License  (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
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

at::Tensor celu(const at::Tensor& self, const at::Scalar& alpha) {
  DO_COMPATIBILITY(aclnnCelu, acl_op::celu(self, alpha));
  at::Tensor result = npu_preparation::apply_tensor_without_format(self);
  EXEC_NPU_CMD(aclnnCelu, self, alpha, result);
  return result;
}

at::Tensor& celu_(at::Tensor& self, const at::Scalar& alpha) {
  DO_COMPATIBILITY(aclnnInplaceCelu, acl_op::celu_(self, alpha));
  EXEC_NPU_CMD(aclnnInplaceCelu, self, alpha);
  return self;
}
} // namespace op_api

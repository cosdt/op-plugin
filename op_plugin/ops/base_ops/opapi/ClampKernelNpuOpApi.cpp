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

at::Tensor& clamp_out(const at::Tensor& self, const c10::optional<at::Scalar>& min,
                      const c10::optional<at::Scalar>& max, at::Tensor& result) {
  DO_COMPATIBILITY(aclnnClamp, acl_op::clamp_out(self, min, max, result));
  npu_preparation::check_tensor({self}, result, self.scalar_type(), self.sizes());
  EXEC_NPU_CMD(aclnnClamp, self, min, max, result);
  return result;
}

at::Tensor clamp(const at::Tensor& self, const c10::optional<at::Scalar>& min, const c10::optional<at::Scalar>& max) {
  DO_COMPATIBILITY(aclnnClamp, acl_op::clamp(self, min, max));
  at::Tensor result = npu_preparation::apply_tensor_without_format(self);
  return op_api::clamp_out(self, min, max, result);
}

at::Tensor& clamp_(at::Tensor& self, const c10::optional<at::Scalar>& min, const c10::optional<at::Scalar>& max) {
  DO_COMPATIBILITY(aclnnClamp, acl_op::clamp_(self, min, max));
  return op_api::clamp_out(self, min, max, self);
}

at::Tensor& clamp_out(const at::Tensor& self, const c10::optional<at::Tensor>& min,
                      const c10::optional<at::Tensor>& max, at::Tensor& result) {
  DO_COMPATIBILITY(aclnnClampTensor, acl_op::clamp_out(self, min, max, result));
  auto out_size = op_infer::clamp_npu_output_size(self, min, max);
  npu_preparation::check_tensor({self}, result, result.scalar_type(), out_size);
  EXEC_NPU_CMD(aclnnClampTensor, self, min, max, result);
  return result;
}

at::Tensor& clamp_(at::Tensor& self, const c10::optional<at::Tensor>& min, const c10::optional<at::Tensor>& max) {
  DO_COMPATIBILITY(aclnnClampTensor, acl_op::clamp_(self, min, max));
  return op_api::clamp_out(self, min, max, self);
}

at::Tensor clamp(const at::Tensor& self, const c10::optional<at::Tensor>& min, const c10::optional<at::Tensor>& max) {
  DO_COMPATIBILITY(aclnnClampTensor, acl_op::clamp(self, min, max));
  at::Tensor result =
      npu_preparation::apply_tensor_without_format(self, op_infer::clamp_npu_output_size(self, min, max));
  EXEC_NPU_CMD(aclnnClampTensor, self, min, max, result);
  return result;
}

at::Tensor& clamp_min_out(const at::Tensor& self, const at::Scalar& min, at::Tensor& result) {
  DO_COMPATIBILITY(aclnnClampMin, acl_op::clamp_min_out(self, min, result));
  npu_preparation::check_tensor({self}, result, self.scalar_type(), self.sizes());
  EXEC_NPU_CMD(aclnnClampMin, self, min, result);
  return result;
}

at::Tensor clamp_min(const at::Tensor& self, const at::Scalar& min) {
  DO_COMPATIBILITY(aclnnClampMin, acl_op::clamp_min(self, min));
  at::Tensor result = npu_preparation::apply_tensor_without_format(self);
  return op_api::clamp_min_out(self, min, result);
}

at::Tensor& clamp_min_(at::Tensor& self, const at::Scalar& min) {
  DO_COMPATIBILITY(aclnnClampMin, acl_op::clamp_min_(self, min));
  return op_api::clamp_min_out(self, min, self);
}

at::Tensor& clamp_max_out(const at::Tensor& self, const at::Tensor& max, at::Tensor& result) {
  DO_COMPATIBILITY(aclnnClampMaxTensor, acl_op::clamp_max_out(self, max, result));
  auto output_size = op_infer::broadcast_ops_npu_output_size(self, max);
  npu_preparation::check_tensor({self, max}, result, result.scalar_type(), output_size);
  EXEC_NPU_CMD(aclnnClampMaxTensor, self, max, result);
  return result;
}

at::Tensor clamp_max(const at::Tensor& self, const at::Tensor& max) {
  DO_COMPATIBILITY(aclnnClampMaxTensor, acl_op::clamp_max(self, max));
  auto output_size = op_infer::broadcast_ops_npu_output_size(self, max);
  at::Tensor result = npu_preparation::apply_tensor_without_format(self, output_size);
  EXEC_NPU_CMD(aclnnClampMaxTensor, self, max, result);
  return result;
}

at::Tensor& clamp_max_(at::Tensor& self, const at::Tensor& max) {
  DO_COMPATIBILITY(aclnnInplaceClampMaxTensor, acl_op::clamp_max_(self, max));
  EXEC_NPU_CMD(aclnnInplaceClampMaxTensor, self, max);
  return self;
}

at::Tensor& clamp_min_out(const at::Tensor& self, const at::Tensor& min, at::Tensor& result) {
  DO_COMPATIBILITY(aclnnClampMinTensor, acl_op::clamp_min_out(self, min, result));
  npu_preparation::check_tensor({self}, result, self.scalar_type(), self.sizes());
  EXEC_NPU_CMD(aclnnClampMinTensor, self, min, result);
  return result;
}

at::Tensor clamp_min(const at::Tensor& self, const at::Tensor& min) {
  DO_COMPATIBILITY(aclnnClampMinTensor, acl_op::clamp_min(self, min));
  at::Tensor result = npu_preparation::apply_tensor_without_format(self);
  EXEC_NPU_CMD(aclnnClampMinTensor, self, min, result);
  return result;
}

at::Tensor& clamp_min_(at::Tensor& self, const at::Tensor& min) {
  DO_COMPATIBILITY(aclnnInplaceClampMinTensor, acl_op::clamp_min_(self, min));
  EXEC_NPU_CMD(aclnnInplaceClampMinTensor, self, min);
  return self;
}

at::Tensor clamp_max(const at::Tensor& self, const at::Scalar& max) {
  DO_COMPATIBILITY(aclnnClampMax, acl_op::clamp_max(self, max));
  at::Tensor result = npu_preparation::apply_tensor_without_format(self);
  EXEC_NPU_CMD(aclnnClampMax, self, max, result);
  return result;
}

at::Tensor& clamp_max_(at::Tensor& self, const at::Scalar& max) {
  DO_COMPATIBILITY(aclnnInplaceClampMax, acl_op::clamp_max_(self, max));
  EXEC_NPU_CMD(aclnnInplaceClampMax, self, max);
  return self;
}
}  // namespace op_api
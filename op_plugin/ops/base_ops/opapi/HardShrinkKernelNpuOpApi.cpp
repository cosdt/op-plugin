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

at::Tensor hardshrink(const at::Tensor& self, const at::Scalar& lambd)
{
  DO_COMPATIBILITY(aclnnHardshrink, acl_op::hardshrink(self, lambd));

  auto output_size = op_infer::input_same_output_size(self);
  at::Tensor result = npu_preparation::apply_tensor_without_format(output_size, self.options());

  // calculate the output result of the NPU
  EXEC_NPU_CMD(aclnnHardshrink, self, lambd, result);
  return result;
}

at::Tensor& hardshrink_out(const at::Tensor& self, const at::Scalar& lambd, at::Tensor& result)
{
  DO_COMPATIBILITY(aclnnHardshrink, acl_op::hardshrink_out(self, lambd, result));

  auto output_size = op_infer::input_same_output_size(self);
  npu_preparation::check_tensor({self}, result, result.scalar_type(), output_size);

  // calculate the output result of the NPU
  EXEC_NPU_CMD(aclnnHardshrink, self, lambd, result);
  return result;
}
}  // namespace op_api


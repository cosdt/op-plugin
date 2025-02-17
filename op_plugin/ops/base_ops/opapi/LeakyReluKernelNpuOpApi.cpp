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
at::Tensor &leaky_relu_out(const at::Tensor &self, const at::Scalar &negval, at::Tensor &result)
{
    DO_COMPATIBILITY(aclnnLeakyRelu, acl_op::leaky_relu_out(self, negval, result));

    at_npu::native::OpPreparation::check_tensor({self}, result, self.scalar_type(), self.sizes());
    // calculate the output result of the NPU
    EXEC_NPU_CMD(aclnnLeakyRelu, self, negval, result);

    return result;
}

at::Tensor leaky_relu(const at::Tensor &self, const at::Scalar &negval)
{
    DO_COMPATIBILITY(aclnnLeakyRelu, acl_op::leaky_relu(self, negval));

    at::Tensor result = at_npu::native::OpPreparation::apply_tensor_without_format(self);
    // calculate the output result of the NPU
    EXEC_NPU_CMD(aclnnLeakyRelu, self, negval, result);

    return result;
}

at::Tensor &leaky_relu_(at::Tensor &self, const at::Scalar &negval)
{
    DO_COMPATIBILITY(aclnnInplaceLeakyRelu, acl_op::leaky_relu_(self, negval));
    EXEC_NPU_CMD(aclnnInplaceLeakyRelu, self, negval);
    return self;
}

} // namespace op_api

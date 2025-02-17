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

std::tuple<at::Tensor, at::Tensor> batch_norm_stats(const at::Tensor& self, double eps)
{
    DO_COMPATIBILITY(aclnnBatchNormStats, acl_op::batch_norm_stats(self, eps));
    TORCH_CHECK(self.ndimension() >= 2, "Expected 2D+ Tensor, but got tensor with ", self.ndimension(), " Dimension",
        OPS_ERROR(ErrCode::PARAM));
    at::Tensor mean = npu_preparation::apply_tensor_without_format({self.size(1)}, self.options().dtype(at::kFloat));
    at::Tensor invstd = npu_preparation::apply_tensor_without_format({self.size(1)}, self.options().dtype(at::kFloat));

    EXEC_NPU_CMD(aclnnBatchNormStats, self, eps, mean, invstd);
    return std::tie(mean, invstd);
}
}  // namespace op_api

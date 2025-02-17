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
at::Tensor npu_masked_softmax_with_rel_pos_bias(
    const at::Tensor& x,
    const c10::optional<at::Tensor> &atten_mask,
    const at::Tensor& relative_pos_bias,
    double scale_value,
    int64_t inner_precision_mode)
{
    at::Tensor result = npu_preparation::apply_tensor_without_format(x);
    EXEC_NPU_CMD(aclnnMaskedSoftmaxWithRelPosBias, x, atten_mask, relative_pos_bias, scale_value, inner_precision_mode, result);
    return result;
}
}  // namespace op_api
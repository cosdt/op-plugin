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

at::Tensor& _index_copy_(at::Tensor& self, const int64_t dim, const at::Tensor& index,
                         const at::Tensor& source) {
  DO_COMPATIBILITY(aclnnInplaceIndexCopy, acl_op::_index_copy_(self, dim, index, source));
  EXEC_NPU_CMD(aclnnInplaceIndexCopy, self, dim, index, source);
  return self;
}

at::Tensor index_copy(const at::Tensor& self, const int64_t dim, const at::Tensor& index,
                      const at::Tensor& source)
{
    DO_COMPATIBILITY(aclnnIndexCopy, acl_op::index_copy(self, dim, index, source));
    at::Tensor out = at_npu::native::OpPreparation::apply_tensor_without_format(self.sizes(), self.options());
    EXEC_NPU_CMD(aclnnIndexCopy, self, dim, index, source, out);
    return out;
}

}  // namespace op_api

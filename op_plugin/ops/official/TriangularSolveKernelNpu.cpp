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

#include "op_plugin/ops/OpInterface.h"
#include "op_plugin/utils/OpAdapter.h"

#include <ATen/native/LinearAlgebraUtils.h>

namespace op_plugin {
using npu_preparation = at_npu::native::OpPreparation;

namespace {
std::tuple<at::Tensor, at::Tensor> triangular_solve_out_nocheck(
    const at::Tensor& self,
    const at::Tensor& A,
    bool upper,
    bool transpose,
    bool unitriangular) {

  at::Tensor self_broadcasted, a_broadcasted;
  std::tie(self_broadcasted, a_broadcasted) = at::native::_linalg_broadcast_batch_dims(self, A, "triangular_solve");
  TORCH_CHECK(self_broadcasted.dtype() == at::kFloat && a_broadcasted.dtype() == at::kFloat,
      "_triangular_solve_helper_npu only supported Float, but get ", self_broadcasted.dtype(), ' ', a_broadcasted.dtype());
  auto self_working_copy = npu_preparation::ApplyTensor(self_broadcasted);
  auto a_working_copy = a_broadcasted.clone();
  at::Tensor a_tensor = a_broadcasted;
  if (unitriangular) {
    auto diagonal_tensor = at::eye(a_tensor.size(-2), a_tensor.size(-1), a_tensor.options());
    a_tensor = a_tensor * (1 - diagonal_tensor) + diagonal_tensor;
  }
  at_npu::native::OpCommand cmd;
  cmd.Name("MatrixTriangularSolve")
      .Input(a_tensor)
      .Input(self_broadcasted)
      .Output(self_working_copy)
      .Attr("lower", !upper)
      .Attr("adjoint", transpose)
      .Run();

  return std::tie(self_working_copy, a_working_copy);
}
} // namespace

std::tuple<at::Tensor&, at::Tensor&> triangular_solve_out(
    const at::Tensor& self,
    const at::Tensor& A,
    bool upper,
    bool transpose,
    bool unitriangular,
    at::Tensor& result,
    at::Tensor& clone_a) {
  at::Tensor result_tmp, clone_a_tmp;
  std::tie(result_tmp, clone_a_tmp) = triangular_solve_out_nocheck(self, A, upper, transpose, unitriangular);
  result.resize_as_(result_tmp).copy_(result_tmp);
  clone_a.resize_as_(clone_a_tmp).copy_(clone_a_tmp);
  return std::tie(result, clone_a);
}
} // namespace op_plugin

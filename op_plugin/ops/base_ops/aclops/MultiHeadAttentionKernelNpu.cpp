// Copyright (c) 2023 Huawei Technologies Co., Ltd
// Copyright (c) 2019, Facebook CORPORATION.
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
#include "op_plugin/utils/OpAdapter.h"

namespace acl_op {
using npu_preparation = at_npu::native::OpPreparation;
using tuple_tensor =
    std::tuple<at::Tensor, at::Tensor, at::Tensor, at::Tensor, at::Tensor, at::Tensor, at::Tensor, at::Tensor>;
using tuple_tensors = std::tuple<at::Tensor, at::Tensor, at::Tensor, at::Tensor, at::Tensor, at::Tensor, at::Tensor,
                                 at::Tensor, at::Tensor, at::Tensor, at::Tensor>;

namespace {
static const int64_t FZ_ALIGN_NUM1 = 16;
static const size_t BIAS_BUM1 = 4;
static const int64_t FZ_ALIGN_NUM = 16;

tuple_tensor multi_head_attention_nocheck(const at::Tensor &query, const at::Tensor &key, const at::Tensor &value,
                                          const at::Tensor &query_weight, const at::Tensor &key_weight,
                                          const at::Tensor &value_weight, const at::Tensor &attn_mask,
                                          const at::Tensor &out_proj_weight, const at::Tensor &query_bias,
                                          const at::Tensor &key_bias, const at::Tensor &value_bias,
                                          const at::Tensor &out_proj_bias, const at::Tensor &mask,
                                          int64_t attn_head_num, int64_t attn_dim_per_head, int64_t src_len,
                                          int64_t tgt_len, double dropout_prob, bool softmax_use_float)
{
    TORCH_CHECK(tgt_len > 0 && src_len > 0 && attn_head_num > 0 && attn_dim_per_head > 0,
        "tgt_len, src_len, attn_head_num, attn_dim_per_head should not equal zero."
        + OPS_ERROR(ErrCode::PARAM));
    TORCH_CHECK(tgt_len % FZ_ALIGN_NUM == 0 && src_len % FZ_ALIGN_NUM == 0 && attn_head_num % FZ_ALIGN_NUM == 0 &&
        attn_dim_per_head % FZ_ALIGN_NUM == 0,
        "tgt_len, src_len, attn_head_num, attn_dim_per_head should align to 16."
        + OPS_ERROR(ErrCode::VALUE));
    TORCH_CHECK(query.dim() >= 1 && key.dim() >= 1 && value.dim() >= 1, "query, key, value should be at least 1d."
        + OPS_ERROR(ErrCode::PARAM));

    auto query_shape = query.sizes();
    int64_t batch = query_shape[0] / tgt_len;
    auto weight_col = attn_head_num * attn_dim_per_head;

    auto query_options = query.options();
    auto query_format = npu_preparation::get_tensor_npu_format(query);

    at::Tensor y = npu_preparation::apply_tensor_with_format({query_shape[0], weight_col}, query_options, query_format);
    at::Tensor dropout_mask = npu_preparation::apply_tensor_with_format(
        {batch * attn_head_num * tgt_len * src_len / 8}, query.options().dtype(at::kByte), ACL_FORMAT_ND);
    at::Tensor query_res = npu_preparation::apply_tensor_with_format({batch, attn_head_num, tgt_len, attn_dim_per_head},
                                                                     query_options, query_format);
    at::Tensor key_res = npu_preparation::apply_tensor_with_format({batch, attn_head_num, src_len, attn_dim_per_head},
                                                                   query_options, query_format);
    at::Tensor value_res = npu_preparation::apply_tensor_with_format({batch, attn_head_num, src_len, attn_dim_per_head},
                                                                     query_options, query_format);
    at::Tensor attn_scores;
    if (softmax_use_float) {
        attn_scores = npu_preparation::apply_tensor_with_format({batch, attn_head_num, tgt_len, src_len},
                                                                query.options().dtype(at::kFloat), query_format);
    } else {
        attn_scores = npu_preparation::apply_tensor_with_format({batch, attn_head_num, tgt_len, src_len}, query_options,
                                                                query_format);
    }
    at::Tensor attn_res = npu_preparation::apply_tensor_with_format({batch, attn_head_num, tgt_len, src_len},
                                                                    query_options, query_format);
    at::Tensor context =
        npu_preparation::apply_tensor_with_format({query_shape[0], weight_col}, query_options, query_format);

    at_npu::native::OpCommand cmd;
    cmd.Name("MultiHeadAttention")
        .Input(query)
        .Input(key)
        .Input(value)
        .Input(query_weight)
        .Input(key_weight)
        .Input(value_weight)
        .Input(attn_mask)
        .Input(out_proj_weight);
    if (query_bias.defined()) {
        cmd.Input(query_bias);
    }
    if (key_bias.defined()) {
        cmd.Input(key_bias);
    }
    if (value_bias.defined()) {
        cmd.Input(value_bias);
    }
    if (out_proj_bias.defined()) {
        cmd.Input(out_proj_bias);
    }
    if (mask.defined()) {
        cmd.Input(mask);
    }

    cmd.Output(y)
        .Output(dropout_mask)
        .Output(query_res)
        .Output(key_res)
        .Output(value_res)
        .Output(attn_scores)
        .Output(attn_res)
        .Output(context)
        .Attr("attn_head_num", attn_head_num)
        .Attr("attn_dim_per_head", attn_dim_per_head)
        .Attr("src_len", src_len)
        .Attr("tgt_len", tgt_len)
        .Attr("keep_prob", static_cast<float>(1 - dropout_prob))
        .Attr("softmax_use_float", softmax_use_float)
        .Run();

    return std::make_tuple(y, dropout_mask, query_res, key_res, value_res, attn_scores, attn_res, context);
}
} // namespace

tuple_tensors npu_multi_head_attention_backward(
    const at::Tensor &query, const at::Tensor &key, const at::Tensor &value, const at::Tensor &query_weight,
    const at::Tensor &key_weight, const at::Tensor &value_weight, const at::Tensor &out_proj_weight,
    const c10::optional<at::Tensor> &query_bias_opt, const c10::optional<at::Tensor> &key_bias_opt,
    const c10::optional<at::Tensor> &value_bias_opt, const c10::optional<at::Tensor> &out_proj_bias_opt,
    const at::Tensor &query_res, const at::Tensor &key_res, const at::Tensor &value_res, const at::Tensor &attn_scores,
    const at::Tensor &attn_res, const at::Tensor &context, const at::Tensor &y_grad, const at::Tensor &dropout_mask,
    int64_t attn_head_num, int64_t attn_dim_per_head, int64_t src_len, int64_t tgt_len, double dropout_prob,
    bool softmax_use_float)
{
    const at::Tensor &query_bias = c10::value_or_else(query_bias_opt, [] { return at::Tensor(); });
    const at::Tensor &key_bias = c10::value_or_else(key_bias_opt, [] { return at::Tensor(); });
    const at::Tensor &value_bias = c10::value_or_else(value_bias_opt, [] { return at::Tensor(); });
    const at::Tensor &out_proj_bias = c10::value_or_else(out_proj_bias_opt, [] { return at::Tensor(); });

    TORCH_CHECK(tgt_len > 0 && src_len > 0 && attn_head_num > 0 && attn_dim_per_head > 0,
        "tgt_len, src_len, attn_head_num, attn_dim_per_head should not equal zero."
        + OPS_ERROR(ErrCode::VALUE));
    TORCH_CHECK(tgt_len % FZ_ALIGN_NUM1 == 0 && src_len % FZ_ALIGN_NUM1 == 0 && attn_head_num % FZ_ALIGN_NUM1 == 0 &&
        attn_dim_per_head % FZ_ALIGN_NUM1 == 0,
        "tgt_len, src_len, attn_head_num, attn_dim_per_head should align to 16."
        + OPS_ERROR(ErrCode::VALUE));
    TORCH_CHECK(query.dim() >= 1 && key.dim() >= 1 && value.dim() >= 1, "query, key, value should be at least 1d."
        + OPS_ERROR(ErrCode::PARAM));
    auto query_shape = query.sizes();
    int64_t batch = query_shape[0] / tgt_len;
    auto weight_col = attn_head_num * attn_dim_per_head;
    at::Tensor query_weight_grad = npu_preparation::apply_tensor(query_weight, {weight_col, weight_col});
    at::Tensor key_weight_grad = npu_preparation::apply_tensor(key_weight, {weight_col, weight_col});
    at::Tensor value_weight_grad = npu_preparation::apply_tensor(value_weight, {weight_col, weight_col});
    at::Tensor out_proj_weight_grad = npu_preparation::apply_tensor(out_proj_weight, {weight_col, weight_col});
    at::Tensor query_grad = npu_preparation::apply_tensor(query, {query_shape[0], weight_col});
    at::Tensor key_grad = npu_preparation::apply_tensor(key, {batch * src_len, weight_col});
    at::Tensor value_grad = npu_preparation::apply_tensor(value, {batch * src_len, weight_col});
    at::Tensor query_bias_grad = npu_preparation::apply_tensor(query_bias, {1, weight_col});
    at::Tensor key_bias_grad = npu_preparation::apply_tensor(key_bias, {1, weight_col});
    at::Tensor value_bias_grad = npu_preparation::apply_tensor(value_bias, {1, weight_col});
    at::Tensor out_proj_bias_grad = npu_preparation::apply_tensor(out_proj_bias, {1, weight_col});
    vector<uint8_t> grad_mask(BIAS_BUM1);
    grad_mask.clear();
    grad_mask.push_back(query_bias.defined());
    grad_mask.push_back(key_bias.defined());
    grad_mask.push_back(value_bias.defined());
    grad_mask.push_back(out_proj_bias.defined());
    at::ArrayRef<uint8_t> bias_grad_mask(grad_mask);

    at_npu::native::OpCommand cmd;
    cmd.Name("MultiHeadAttentionGrad")
        .Input(query)
        .Input(key)
        .Input(value)
        .Input(query_weight)
        .Input(key_weight)
        .Input(value_weight)
        .Input(out_proj_weight)
        .Input(query_res)
        .Input(key_res)
        .Input(value_res)
        .Input(attn_scores)
        .Input(attn_res)
        .Input(context)
        .Input(y_grad);

    if (dropout_prob > 0) {
        cmd.Input(dropout_mask);
    }

    cmd.Output(query_weight_grad)
        .Output(key_weight_grad)
        .Output(value_weight_grad)
        .Output(out_proj_weight_grad)
        .Output(query_grad)
        .Output(key_grad)
        .Output(value_grad)
        .Output(query_bias_grad)
        .Output(key_bias_grad)
        .Output(value_bias_grad)
        .Output(out_proj_bias_grad)
        .Attr("attn_head_num", attn_head_num)
        .Attr("attn_dim_per_head", attn_dim_per_head)
        .Attr("src_len", src_len)
        .Attr("tgt_len", tgt_len)
        .Attr("keep_prob", static_cast<float>(1 - dropout_prob))
        .Attr("softmax_use_float", softmax_use_float)
        .Attr("bias_grad_mask", bias_grad_mask)
        .Run();

    return std::make_tuple(query_weight_grad, key_weight_grad, value_weight_grad, out_proj_weight_grad, query_grad,
                           key_grad, value_grad, query_bias_grad, key_bias_grad, value_bias_grad, out_proj_bias_grad);
}

tuple_tensor npu_multi_head_attention(
    const at::Tensor &query, const at::Tensor &key, const at::Tensor &value, const at::Tensor &query_weight,
    const at::Tensor &key_weight, const at::Tensor &value_weight, const at::Tensor &attn_mask,
    const at::Tensor &out_proj_weight, const c10::optional<at::Tensor> &query_bias_opt,
    const c10::optional<at::Tensor> &key_bias_opt, const c10::optional<at::Tensor> &value_bias_opt,
    const c10::optional<at::Tensor> &out_proj_bias_opt, const c10::optional<at::Tensor> &dropout_mask_opt,
    int64_t attn_head_num, int64_t attn_dim_per_head, int64_t src_len, int64_t tgt_len, double dropout_prob,
    bool softmax_use_float)
{
    const at::Tensor &query_bias = c10::value_or_else(query_bias_opt, [] { return at::Tensor(); });
    const at::Tensor &key_bias = c10::value_or_else(key_bias_opt, [] { return at::Tensor(); });
    const at::Tensor &value_bias = c10::value_or_else(value_bias_opt, [] { return at::Tensor(); });
    const at::Tensor &out_proj_bias = c10::value_or_else(out_proj_bias_opt, [] { return at::Tensor(); });
    const at::Tensor &mask = c10::value_or_else(dropout_mask_opt, [] { return at::Tensor(); });

    return multi_head_attention_nocheck(query, key, value, query_weight, key_weight, value_weight, attn_mask,
                                        out_proj_weight, query_bias, key_bias, value_bias, out_proj_bias, mask,
                                        attn_head_num, attn_dim_per_head, src_len, tgt_len, dropout_prob,
                                        softmax_use_float);
}
} // namespace acl_op

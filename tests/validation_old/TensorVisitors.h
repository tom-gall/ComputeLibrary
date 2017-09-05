/*
 * Copyright (c) 2017 ARM Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __ARM_COMPUTE_TEST_TENSOR_VISITORS_H__
#define __ARM_COMPUTE_TEST_TENSOR_VISITORS_H__

#include "Tensor.h"
#include "TensorOperations.h"
#include "arm_compute/core/Error.h"
#include "arm_compute/core/Helpers.h"
#include "arm_compute/runtime/Lut.h"

#include "tests/validation_old/boost_wrapper.h"

#include <algorithm>
#include <map>
#include <memory>
#include <ostream>
#include <vector>

namespace arm_compute
{
namespace test
{
namespace validation
{
namespace tensor_visitors
{
// Absolute Difference visitor
struct absolute_difference_visitor : public boost::static_visitor<>
{
public:
    template <typename T1, typename T2, typename T3>
    void operator()(const Tensor<T1> &in1, const Tensor<T2> &in2, Tensor<T3> &out) const
    {
        tensor_operations::absolute_difference(in1, in2, out);
    }
};
// Pixel-wise Multiplication visitor
struct pixel_wise_multiplication_visitor : public boost::static_visitor<>
{
public:
    explicit pixel_wise_multiplication_visitor(float scale, ConvertPolicy convert_policy, RoundingPolicy rounding_policy)
        : _scale(scale), _convert_policy(convert_policy), _rounding_policy(rounding_policy)
    {
    }

    template <typename T1, typename T2, typename T3>
    void operator()(const Tensor<T1> &in1, const Tensor<T2> &in2, Tensor<T3> &out) const
    {
        tensor_operations::pixel_wise_multiplication(in1, in2, out, _scale, _convert_policy, _rounding_policy);
    }

private:
    float          _scale;
    ConvertPolicy  _convert_policy;
    RoundingPolicy _rounding_policy;
};
// Fixed Point Pixel-wise Multiplication visitor
struct fixed_point_pixel_wise_multiplication_visitor : public boost::static_visitor<>
{
public:
    explicit fixed_point_pixel_wise_multiplication_visitor(const TensorVariant &in1, const TensorVariant &in2, float scale, ConvertPolicy convert_policy, RoundingPolicy rounding_policy)
        : _in1(in1), _in2(in2), _scale(scale), _convert_policy(convert_policy), _rounding_policy(rounding_policy)
    {
    }

    template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
    void operator()(Tensor<T> &out) const
    {
        const Tensor<T> &in1 = boost::get<Tensor<T>>(_in1);
        const Tensor<T> &in2 = boost::get<Tensor<T>>(_in2);
        tensor_operations::fixed_point_pixel_wise_multiplication(in1, in2, out, _scale, _convert_policy, _rounding_policy);
    }
    template < typename T, typename std::enable_if < !std::is_integral<T>::value, int >::type = 0 >
    void operator()(Tensor<T> &out) const
    {
        ARM_COMPUTE_ERROR("NOT SUPPORTED!");
    }

private:
    const TensorVariant &_in1;
    const TensorVariant &_in2;
    float                _scale;
    ConvertPolicy        _convert_policy;
    RoundingPolicy       _rounding_policy;
};

// ROI Pooling layer
struct roi_pooling_layer_visitor : public boost::static_visitor<>
{
public:
    explicit roi_pooling_layer_visitor(const TensorVariant &in, const std::vector<ROI> &rois, ROIPoolingLayerInfo pool_info)
        : _in(in), _rois(rois), _pool_info(pool_info)
    {
    }

    template <typename T>
    void operator()(Tensor<T> &out) const
    {
        const Tensor<T> &in = boost::get<Tensor<T>>(_in);
        tensor_operations::roi_pooling_layer(in, out, _rois, _pool_info);
    }

private:
    const TensorVariant    &_in;
    const std::vector<ROI> &_rois;
    ROIPoolingLayerInfo     _pool_info;
};

// Fixed Point operations visitor
struct fixed_point_operation_visitor : public boost::static_visitor<>
{
public:
    explicit fixed_point_operation_visitor(const TensorVariant &in, FixedPointOp op)
        : _in(in), _op(op)
    {
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    void operator()(Tensor<T> &out) const
    {
        const Tensor<T> &in = boost::get<Tensor<T>>(_in);
        tensor_operations::fixed_point_operation(in, out, _op);
    }
    template < typename T, typename std::enable_if < !std::is_integral<T>::value, int >::type = 0 >
    void operator()(Tensor<T> &out) const
    {
        ARM_COMPUTE_ERROR("NOT SUPPORTED!");
    }

private:
    const TensorVariant &_in;
    FixedPointOp         _op;
};
// Print Tensor visitor
struct print_visitor : public boost::static_visitor<>
{
public:
    explicit print_visitor(std::ostream &out)
        : _out(out)
    {
    }

    template <typename T>
    void operator()(const Tensor<T> &in) const
    {
        tensor_operations::print(in, _out);
    }

private:
    std::ostream &_out;
};
} // namespace tensor_visitors
} // namespace validation
} // namespace test
} // namespace arm_compute

#endif /* __ARM_COMPUTE_TEST_TENSOR_VISITORS_H__ */

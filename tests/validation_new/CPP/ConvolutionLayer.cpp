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
#include "ConvolutionLayer.h"

#include "tests/validation_new/FixedPoint.h"
#include "tests/validation_new/Helpers.h"
#include "tests/validation_new/half.h"

namespace arm_compute
{
namespace test
{
namespace validation
{
namespace reference
{
namespace
{
inline bool is_valid_pixel(int i, int min, int max)
{
    return (i >= min && i < max);
}

// 3D convolution for floating point type
template <typename T, typename std::enable_if<is_floating_point<T>::value, int>::type = 0>
void convolution3d(const T *in, const T *weights, const T *bias, T *out, int xi, int yi, int width_in, int height_in, int depth_in, int width_weights, int height_weights, int fixed_point_position)
{
    ARM_COMPUTE_UNUSED(fixed_point_position);

    const int half_width_weights  = width_weights / 2;
    const int half_height_weights = height_weights / 2;

    // Reset accumulator
    T acc(0);

    // Compute a 2D convolution for each IFM and accumulate the result
    for(int ifm = 0; ifm < depth_in; ++ifm)
    {
        // Compute the offset for the input slice
        const int offset_slice_in = xi + yi * width_in + ifm * width_in * height_in;

        // Compute 2D convolution
        for(int yk = -half_height_weights; yk <= half_height_weights; ++yk)
        {
            for(int xk = -half_width_weights; xk <= half_width_weights; ++xk)
            {
                // Check if the pixel is out-of-bound
                if(is_valid_pixel(xi + xk, 0, width_in) && is_valid_pixel(yi + yk, 0, height_in))
                {
                    const int idx = xk + half_width_weights;
                    const int idy = yk + half_height_weights;

                    const T i_value = in[offset_slice_in + xk + yk * width_in];
                    const T w_value = weights[idx + idy * width_weights + ifm * width_weights * height_weights];

                    acc += i_value * w_value;
                }
            }
        }
    }

    // Accumulate the bias and store the result
    *out = acc + (*bias);
}

// 3D convolution for fixed point type
template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
void convolution3d(const T *in, const T *weights, const T *bias, T *out, int xi, int yi, int width_in, int height_in, int depth_in, int width_weights, int height_weights,
                   int fixed_point_position)
{
    const int half_width_weights  = width_weights / 2;
    const int half_height_weights = height_weights / 2;

    using namespace fixed_point_arithmetic;
    using promoted_type = fixed_point_arithmetic::traits::promote_t<T>;

    // Reset accumulator
    fixed_point<promoted_type> acc(0, fixed_point_position);

    // Compute a 2D convolution for each IFM and accumulate the result
    for(int ifm = 0; ifm < depth_in; ++ifm)
    {
        // Compute the offset for the input slice
        const int offset_slice_in = xi + yi * width_in + ifm * width_in * height_in;

        // Compute 2D convolution
        for(int yk = -half_height_weights; yk <= half_height_weights; ++yk)
        {
            for(int xk = -half_width_weights; xk <= half_width_weights; ++xk)
            {
                // Check if the pixel is out-of-bound
                if(is_valid_pixel(xi + xk, 0, width_in) && is_valid_pixel(yi + yk, 0, height_in))
                {
                    const int idx = xk + half_width_weights;
                    const int idy = yk + half_height_weights;

                    const fixed_point<promoted_type> i_value(in[offset_slice_in + xk + yk * width_in], fixed_point_position, true);
                    const fixed_point<promoted_type> w_value(weights[idx + idy * width_weights + ifm * width_weights * height_weights], fixed_point_position, true);
                    const fixed_point<promoted_type> iw = i_value * w_value;
                    acc                                 = iw + acc;
                }
            }
        }
    }

    // Get the bias
    const fixed_point<promoted_type> b(*bias, fixed_point_position, true);

    // Accumulate the bias and covert back
    acc = acc + b;
    fixed_point<T> res(acc);
    *out = res.raw();
}
} // namespace

template <typename T>
SimpleTensor<T> convolution_layer(const SimpleTensor<T> &src, const SimpleTensor<T> &weights, const SimpleTensor<T> &bias, const TensorShape &output_shape, const PadStrideInfo &info)
{
    // Create reference
    SimpleTensor<T> dst{ output_shape, src.data_type(), 1, src.fixed_point_position() };

    // Compute reference
    const int width_in       = src.shape().x();
    const int height_in      = src.shape().y();
    const int depth_in       = src.shape().z();
    const int width_out      = dst.shape().x();
    const int height_out     = dst.shape().y();
    const int depth_out      = dst.shape().z();
    const int width_weights  = weights.shape().x();
    const int height_weights = weights.shape().y();
    const int depth_weights  = weights.shape().z();
    const int pad_xi         = std::min(static_cast<int>(info.pad().first), width_weights / 2);
    const int pad_yi         = std::min(static_cast<int>(info.pad().second), height_weights / 2);
    const int start_xi       = width_weights / 2 - pad_xi;
    const int start_yi       = height_weights / 2 - pad_yi;
    const int end_xi         = width_in - start_xi;
    const int end_yi         = height_in - start_yi;
    const int stride_xi      = info.stride().first;
    const int stride_yi      = info.stride().second;
    const int num_batches    = src.shape().total_size() / (width_in * height_in * depth_in);

    for(int r = 0; r < num_batches; ++r)
    {
        for(int yi = start_yi; yi < end_yi; yi += stride_yi)
        {
            for(int xi = start_xi; xi < end_xi; xi += stride_xi)
            {
                for(int ofm = 0; ofm < depth_out; ++ofm)
                {
                    // Compute input and output offsets
                    const int offset_in  = r * width_in * height_in * depth_in;
                    const int xo         = (xi - start_xi) / stride_xi;
                    const int yo         = (yi - start_yi) / stride_yi;
                    const int offset_out = xo + yo * width_out + ofm * width_out * height_out + r * width_out * height_out * depth_out;

                    // Compute 3D convolution
                    convolution3d(src.data() + offset_in,
                                  weights.data() + ofm * width_weights * height_weights * depth_weights,
                                  bias.data() + ofm,
                                  dst.data() + offset_out,
                                  xi, yi,
                                  width_in, height_in, depth_in,
                                  width_weights, height_weights,
                                  src.fixed_point_position());
                }
            }
        }
    }

    return dst;
}

template SimpleTensor<float> convolution_layer(const SimpleTensor<float> &src, const SimpleTensor<float> &weights, const SimpleTensor<float> &bias, const TensorShape &output_shape,
                                               const PadStrideInfo &info);
template SimpleTensor<half_float::half> convolution_layer(const SimpleTensor<half_float::half> &src, const SimpleTensor<half_float::half> &weights, const SimpleTensor<half_float::half> &bias,
                                                          const TensorShape &output_shape, const PadStrideInfo &info);
template SimpleTensor<qint8_t> convolution_layer(const SimpleTensor<qint8_t> &src, const SimpleTensor<qint8_t> &weights, const SimpleTensor<qint8_t> &bias, const TensorShape &output_shape,
                                                 const PadStrideInfo &info);
template SimpleTensor<qint16_t> convolution_layer(const SimpleTensor<qint16_t> &src, const SimpleTensor<qint16_t> &weights, const SimpleTensor<qint16_t> &bias, const TensorShape &output_shape,
                                                  const PadStrideInfo &info);
} // namespace reference
} // namespace validation
} // namespace test
} // namespace arm_compute

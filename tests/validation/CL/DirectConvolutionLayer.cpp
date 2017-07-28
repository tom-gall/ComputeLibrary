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
#include "AssetsLibrary.h"
#include "CL/CLAccessor.h"
#include "Globals.h"
#include "TypePrinter.h"
#include "Utils.h"
#include "validation/Datasets.h"
#include "validation/Reference.h"
#include "validation/Validation.h"

#include "arm_compute/core/Helpers.h"
#include "arm_compute/core/Types.h"
#include "arm_compute/runtime/CL/functions/CLDirectConvolutionLayer.h"
#include "arm_compute/runtime/Tensor.h"
#include "arm_compute/runtime/TensorAllocator.h"

#include "boost_wrapper.h"

#include <random>
#include <string>
#include <tuple>

using namespace arm_compute;
using namespace arm_compute::test;
using namespace arm_compute::test::validation;

namespace
{
/** Define tolerance of the direct convolution layer
 *
 * @param[in] dt DataType of the tensor.
 *
 * @return Tolerance depending on the data type.
 */
float direct_convolution_layer_tolerance(DataType dt)
{
    switch(dt)
    {
        case DataType::F16:
            return 0.1f;
        case DataType::F32:
            return 1e-3f;
        default:
            return 0.f;
    }
}

/** Compute CL direct convolution layer function.
 *
 * @param[in] src_shape            Shape of the input tensor.
 * @param[in] weights_shape        Shape of the weights.
 * @param[in] bias_shape           Shape of the bias tensor.
 * @param[in] dst_shape            Shape of the output tensor.
 * @param[in] dt                   Data type of input, convolution matrix and output tensors.
 * @param[in] conv_info            Padding and stride information.
 * @param[in] fixed_point_position (Optional) Number of bits for the fractional part of the fixed point numbers
 *
 * @return Computed output tensor.
*/
CLTensor compute_convolution_layer(const TensorShape &src_shape, const TensorShape &weights_shape, const TensorShape &bias_shape, const TensorShape &dst_shape,
                                   DataType dt, PadStrideInfo conv_info, int fixed_point_position = 0)
{
    // Create tensors
    CLTensor src     = create_tensor<CLTensor>(src_shape, dt, 1, fixed_point_position);
    CLTensor weights = create_tensor<CLTensor>(weights_shape, dt, 1, fixed_point_position);

    CLTensor bias = create_tensor<CLTensor>(bias_shape, dt, 1, fixed_point_position);
    CLTensor dst  = create_tensor<CLTensor>(dst_shape, dt, 1, fixed_point_position);

    // Create and configure function
    CLDirectConvolutionLayer conv_layer;
    conv_layer.configure(&src, &weights, &bias, &dst, conv_info);

    // Allocate tensors
    src.allocator()->allocate();
    weights.allocator()->allocate();
    dst.allocator()->allocate();
    bias.allocator()->allocate();

    BOOST_TEST(!src.info()->is_resizable());
    BOOST_TEST(!weights.info()->is_resizable());
    BOOST_TEST(!dst.info()->is_resizable());
    BOOST_TEST(!bias.info()->is_resizable());

    // Fill tensors
    switch(dt)
    {
        case DataType::F16:
        case DataType::F32:
        {
            std::uniform_real_distribution<> distribution(-1.f, 1.f);
            library->fill(CLAccessor(src), distribution, 0);
            library->fill(CLAccessor(weights), distribution, 1);
            library->fill(CLAccessor(bias), distribution, 2);
            break;
        }
        default:
        {
            ARM_COMPUTE_ERROR("Not supported");
        }
    }

    // Compute function
    conv_layer.run();

    return dst;
}

TensorShape get_output_shape(TensorShape in_shape, TensorShape kernel_shape, const PadStrideInfo &conv_info)
{
    TensorShape out_shape(in_shape);
    const std::pair<unsigned int, unsigned int> scaled_dims = arm_compute::scaled_dimensions(in_shape.x(),
                                                                                             in_shape.y(),
                                                                                             kernel_shape.x(),
                                                                                             kernel_shape.y(),
                                                                                             conv_info);
    out_shape.set(0, scaled_dims.first);
    out_shape.set(1, scaled_dims.second);
    out_shape.set(2, kernel_shape[3]);
    return out_shape;
}

} // namespace

#ifndef DOXYGEN_SKIP_THIS
BOOST_AUTO_TEST_SUITE(CL)
BOOST_AUTO_TEST_SUITE(DirectConvolutionLayer)

BOOST_AUTO_TEST_SUITE(Float)

BOOST_TEST_DECORATOR(*boost::unit_test::label("precommit"))
BOOST_DATA_TEST_CASE(W1x1, DirectConvolutionShapes() * boost::unit_test::data::make({ DataType::F16, DataType::F32 }) * boost::unit_test::data::xrange(1, 4, 1) * boost::unit_test::data::xrange(1, 4,
                     1)
                     * boost::unit_test::data::make({ 1, 4, 8, 16 }),
                     input_shape, dt, sx, sy, num_kernels)
{
    const unsigned int  kernel_size = 1;
    const PadStrideInfo conv_info(sx, sy, 0, 0, DimensionRoundingType::FLOOR);
    const TensorShape   w_shape(kernel_size, kernel_size, input_shape.z(), static_cast<unsigned int>(num_kernels));
    const TensorShape   b_shape(static_cast<unsigned int>(num_kernels));
    const TensorShape   d_shape(get_output_shape(input_shape, w_shape, conv_info));

    CLTensor dst = compute_convolution_layer(input_shape, w_shape, b_shape, d_shape, dt, conv_info);

    RawTensor ref = Reference::compute_reference_convolution_layer(input_shape, w_shape, b_shape, d_shape, dt, conv_info, 0);

    // Validate output
    validate(CLAccessor(dst), ref, direct_convolution_layer_tolerance(dt));
}

BOOST_TEST_DECORATOR(*boost::unit_test::label("precommit"))
BOOST_DATA_TEST_CASE(W3x3, DirectConvolutionShapes() * boost::unit_test::data::make({ DataType::F16, DataType::F32 }) * boost::unit_test::data::xrange(1, 3, 1) * boost::unit_test::data::xrange(1, 3,
                     1)
                     * boost::unit_test::data::xrange(0, 2, 1)
                     * boost::unit_test::data::xrange(0, 2, 1) * boost::unit_test::data::make({ 1, 4, 8, 16 }),
                     input_shape, dt, sx, sy, px, py, num_kernels)
{
    const unsigned int  kernel_size = 3;
    const PadStrideInfo conv_info(sx, sy, px, py, DimensionRoundingType::FLOOR);
    const TensorShape   w_shape(kernel_size, kernel_size, input_shape.z(), static_cast<unsigned int>(num_kernels));
    const TensorShape   b_shape(static_cast<unsigned int>(num_kernels));
    const TensorShape   d_shape(get_output_shape(input_shape, w_shape, conv_info));

    CLTensor dst = compute_convolution_layer(input_shape, w_shape, b_shape, d_shape, dt, conv_info);

    RawTensor ref = Reference::compute_reference_convolution_layer(input_shape, w_shape, b_shape, d_shape, dt, conv_info, 0);

    // Validate output
    validate(CLAccessor(dst), ref, direct_convolution_layer_tolerance(dt));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
#endif /* DOXYGEN_SKIP_THIS */

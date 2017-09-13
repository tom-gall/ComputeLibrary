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
#include "CL/CLAccessor.h"
#include "TypePrinter.h"
#include "tests/Globals.h"
#include "tests/Utils.h"
#include "tests/validation_old/Datasets.h"
#include "tests/validation_old/Reference.h"
#include "tests/validation_old/Validation.h"

#include "arm_compute/runtime/CL/functions/CLPixelWiseMultiplication.h"

using namespace arm_compute;
using namespace arm_compute::test;
using namespace arm_compute::test::validation;

namespace
{
const float tolerance_f32 = 1.f; /**< Tolerance value for comparing reference's output against implementation's output for float input */
const float tolerance_f16 = 1.f; /**< Tolerance value for comparing reference's output against implementation's output for float input */

/** Compute CL pixel-wise multiplication function.
 *
 * @param[in] shape                Shape of the input and output tensors.
 * @param[in] dt_in0               Data type of first input tensor.
 * @param[in] dt_in1               Data type of second input tensor.
 * @param[in] dt_out               Data type of the output tensor.
 * @param[in] scale                Non-negative scale.
 * @param[in] convert_policy       Overflow policy of the operation.
 * @param[in] rounding_policy      Rounding policy of the operation.
 * @param[in] fixed_point_position (Optional) Fixed point position that expresses the number of bits for the fractional part of the number.
 *
 * @return Computed output tensor.
 */
CLTensor compute_pixel_wise_multiplication(const TensorShape &shape, DataType dt_in0, DataType dt_in1, DataType dt_out, float scale, ConvertPolicy convert_policy, RoundingPolicy rounding_policy,
                                           int fixed_point_position = 0)
{
    // Create tensors
    CLTensor src1 = create_tensor<CLTensor>(shape, dt_in0, 1, fixed_point_position);
    CLTensor src2 = create_tensor<CLTensor>(shape, dt_in1, 1, fixed_point_position);
    CLTensor dst  = create_tensor<CLTensor>(shape, dt_out, 1, fixed_point_position);

    // Create and configure function
    CLPixelWiseMultiplication multiply;
    multiply.configure(&src1, &src2, &dst, scale, convert_policy, rounding_policy);

    // Allocate tensors
    src1.allocator()->allocate();
    src2.allocator()->allocate();
    dst.allocator()->allocate();

    BOOST_TEST(!src1.info()->is_resizable());
    BOOST_TEST(!src2.info()->is_resizable());
    BOOST_TEST(!dst.info()->is_resizable());

    // Fill tensors
    library->fill_tensor_uniform(CLAccessor(src1), 0);
    library->fill_tensor_uniform(CLAccessor(src2), 1);

    // Compute function
    multiply.run();

    return dst;
}
} // namespace

#ifndef DOXYGEN_SKIP_THIS
BOOST_AUTO_TEST_SUITE(CL)
BOOST_AUTO_TEST_SUITE(PixelWiseMultiplication)

BOOST_AUTO_TEST_SUITE(Float16)
BOOST_TEST_DECORATOR(*boost::unit_test::label("precommit"))
BOOST_DATA_TEST_CASE(RunSmall, SmallShapes() * DataType::F16 *ConvertPolicies() * RoundingPolicy::TO_NEAREST_UP,
                     shape, dt, convert_policy, rounding_policy)
{
    constexpr float scale = 1.f / 255.f;

    // Compute function
    CLTensor dst = compute_pixel_wise_multiplication(shape, dt, dt, dt, scale, convert_policy, rounding_policy);

    // Compute reference
    RawTensor ref_dst = Reference::compute_reference_pixel_wise_multiplication(shape, dt, dt, dt, scale, convert_policy, rounding_policy);

    // Validate output
    validate(CLAccessor(dst), ref_dst, tolerance_f16);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Float)
BOOST_TEST_DECORATOR(*boost::unit_test::label("precommit"))
BOOST_DATA_TEST_CASE(RunSmall, SmallShapes() * DataType::F32 *ConvertPolicies() * RoundingPolicy::TO_NEAREST_UP,
                     shape, dt, convert_policy, rounding_policy)
{
    constexpr float scale = 1.f / 255.f;

    // Compute function
    CLTensor dst = compute_pixel_wise_multiplication(shape, dt, dt, dt, scale, convert_policy, rounding_policy);

    // Compute reference
    RawTensor ref_dst = Reference::compute_reference_pixel_wise_multiplication(shape, dt, dt, dt, scale, convert_policy, rounding_policy);

    // Validate output
    validate(CLAccessor(dst), ref_dst, tolerance_f32);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Quantized)
BOOST_AUTO_TEST_SUITE(QS8)
BOOST_TEST_DECORATOR(*boost::unit_test::label("precommit"))
BOOST_DATA_TEST_CASE(RunSmall, SmallShapes() * DataType::QS8 *ConvertPolicies() * RoundingPolicy::TO_ZERO * boost::unit_test::data::xrange<int>(1, 7),
                     shape, dt, convert_policy, rounding_policy, fixed_point_position)
{
    // Compute function
    CLTensor dst = compute_pixel_wise_multiplication(shape, dt, dt, dt, 1.f, convert_policy, rounding_policy, fixed_point_position);

    // Compute reference
    RawTensor ref_dst = Reference::compute_reference_fixed_point_pixel_wise_multiplication(shape, dt, dt, dt, 1.f, fixed_point_position, convert_policy, rounding_policy);

    // Validate output
    validate(CLAccessor(dst), ref_dst);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(QS16)
BOOST_TEST_DECORATOR(*boost::unit_test::label("precommit"))
BOOST_DATA_TEST_CASE(RunSmall, SmallShapes() * DataType::QS16 *ConvertPolicies() * RoundingPolicy::TO_ZERO * boost::unit_test::data::xrange<int>(1, 15),
                     shape, dt, convert_policy, rounding_policy, fixed_point_position)
{
    // Compute function
    CLTensor dst = compute_pixel_wise_multiplication(shape, dt, dt, dt, 1.f, convert_policy, rounding_policy, fixed_point_position);

    // Compute reference
    RawTensor ref_dst = Reference::compute_reference_fixed_point_pixel_wise_multiplication(shape, dt, dt, dt, 1.f, fixed_point_position, convert_policy, rounding_policy);

    // Validate output
    validate(CLAccessor(dst), ref_dst);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
#endif // DOXYGEN_SKIP_THIS
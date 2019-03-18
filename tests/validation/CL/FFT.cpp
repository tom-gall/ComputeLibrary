/*
 * Copyright (c) 2019 ARM Limited.
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
#include "arm_compute/core/Types.h"
#include "arm_compute/runtime/CL/CLTensor.h"
#include "arm_compute/runtime/CL/functions/CLFFT1D.h"
#include "tests/CL/CLAccessor.h"
#include "tests/framework/Asserts.h"
#include "tests/framework/Macros.h"
#include "tests/framework/datasets/Datasets.h"
#include "tests/validation/Validation.h"
#include "tests/validation/fixtures/FFTFixture.h"

namespace arm_compute
{
namespace test
{
namespace validation
{
namespace
{
const auto data_types = framework::dataset::make("DataType", { DataType::F32 });
const auto shapes     = framework::dataset::make("TensorShape", { TensorShape(2U, 2U, 3U), TensorShape(3U, 2U, 3U),
                                                                  TensorShape(4U, 2U, 3U), TensorShape(5U, 2U, 3U),
                                                                  TensorShape(7U, 2U, 3U), TensorShape(8U, 2U, 3U),
                                                                  TensorShape(9U, 2U, 3U), TensorShape(25U, 2U, 3U),
                                                                  TensorShape(49U, 2U, 3U), TensorShape(64U, 2U, 3U),
                                                                  TensorShape(16U, 2U, 3U), TensorShape(32U, 2U, 3U),
                                                                  TensorShape(96U, 2U, 2U)
                                                                });
} // namespace
TEST_SUITE(CL)
TEST_SUITE(FFT1D)

DATA_TEST_CASE(Configuration, framework::DatasetMode::ALL, combine(shapes, data_types),
               shape, data_type)
{
    // Create tensors
    CLTensor src = create_tensor<CLTensor>(shape, data_type, 2);
    CLTensor dst = create_tensor<CLTensor>(shape, data_type, 2);

    ARM_COMPUTE_EXPECT(src.info()->is_resizable(), framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(dst.info()->is_resizable(), framework::LogLevel::ERRORS);

    // Create and configure function
    CLFFT1D fft1d;
    fft1d.configure(&src, &dst, FFT1DInfo());

    // Validate valid region
    const ValidRegion valid_region = shape_to_valid_region(shape);
    validate(src.info()->valid_region(), valid_region);
    validate(dst.info()->valid_region(), valid_region);

    // Validate padding
    validate(src.info()->padding(), PaddingSize());
    validate(dst.info()->padding(), PaddingSize());
}

// *INDENT-OFF*
// clang-format off
DATA_TEST_CASE(Validate, framework::DatasetMode::ALL, zip(zip(zip(
        framework::dataset::make("InputInfo", { TensorInfo(TensorShape(32U, 13U, 2U), 2, DataType::F32), // Mismatching data types
                                                TensorInfo(TensorShape(32U, 13U, 2U), 2, DataType::F32), // Mismatching shapes
                                                TensorInfo(TensorShape(32U, 13U, 2U), 1, DataType::F32), // Invalid channels
                                                TensorInfo(TensorShape(32U, 13U, 2U), 2, DataType::F32), // Unsupported axis
                                                TensorInfo(TensorShape(11U, 13U, 2U), 2, DataType::F32), // Undecomposable FFT
                                                TensorInfo(TensorShape(25U, 13U, 2U), 2, DataType::F32),
        }),
        framework::dataset::make("OutputInfo",{ TensorInfo(TensorShape(32U, 13U, 2U), 2, DataType::F16),
                                                TensorInfo(TensorShape(16U, 13U, 2U), 2, DataType::F32),
                                                TensorInfo(TensorShape(32U, 13U, 2U), 1, DataType::F32),
                                                TensorInfo(TensorShape(32U, 13U, 2U), 2, DataType::F32),
                                                TensorInfo(TensorShape(11U, 13U, 2U), 2, DataType::F32),
                                                TensorInfo(TensorShape(25U, 13U, 2U), 2, DataType::F32),
        })),
        framework::dataset::make("Axis", { 0, 0, 0, 1, 0, 0 })),
        framework::dataset::make("Expected", { false, false, false, false, false, true })),
        input_info, output_info, axis, expected)
{
    FFT1DInfo desc;
    desc.axis = axis;
    const Status s = CLFFT1D::validate(&input_info.clone()->set_is_resizable(false), &output_info.clone()->set_is_resizable(false), desc);
    ARM_COMPUTE_EXPECT(bool(s) == expected, framework::LogLevel::ERRORS);
}
// clang-format on
// *INDENT-ON*

template <typename T>
using CLFFT1DFixture = FFTValidationFixture<CLTensor, CLAccessor, CLFFT1D, T>;

TEST_SUITE(Float)
TEST_SUITE(FP32)
FIXTURE_DATA_TEST_CASE(RunSmall, CLFFT1DFixture<float>, framework::DatasetMode::ALL, combine(shapes, framework::dataset::make("DataType", DataType::F32)))
{
    // Validate output
    validate(CLAccessor(_target), _reference, RelativeTolerance<float>(0.1f), 0.05f);
}
TEST_SUITE_END() // FP32
TEST_SUITE_END() // Float

TEST_SUITE_END() // FFT1D
TEST_SUITE_END() // CL
} // namespace validation
} // namespace test
} // namespace arm_compute

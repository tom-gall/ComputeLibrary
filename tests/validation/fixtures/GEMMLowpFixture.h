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
#ifndef ARM_COMPUTE_TEST_GEMMLOWP_FIXTURE
#define ARM_COMPUTE_TEST_GEMMLOWP_FIXTURE

#include "arm_compute/core/TensorShape.h"
#include "arm_compute/core/Types.h"
#include "tests/AssetsLibrary.h"
#include "tests/Globals.h"
#include "tests/IAccessor.h"
#include "tests/framework/Asserts.h"
#include "tests/framework/Fixture.h"
#include "tests/validation/CPP/GEMMLowp.h"
#include "tests/validation/Helpers.h"

#include <random>

namespace arm_compute
{
namespace test
{
namespace validation
{
template <typename TensorType, typename AccessorType, typename FunctionType>
class GEMMLowpMatrixMultiplyCoreValidationFixture : public framework::Fixture
{
public:
    template <typename...>
    void setup(TensorShape shape_a, TensorShape shape_b, TensorShape shape_c, int32_t a_offset, int32_t b_offset)
    {
        _target    = compute_target(shape_a, shape_b, shape_c, a_offset, b_offset);
        _reference = compute_reference(shape_a, shape_b, shape_c, a_offset, b_offset);
    }

protected:
    template <typename U>
    void fill(U &&tensor, int i)
    {
        // Between 1 and 254 in order to avoid having -128 and 128 for the DOT product path
        std::uniform_int_distribution<> distribution(1, 254);
        library->fill(tensor, distribution, i);
    }

    TensorType compute_target(const TensorShape &shape_a, const TensorShape &shape_b, const TensorShape &shape_c,
                              int32_t a_offset, int32_t b_offset)
    {
        // Create tensors
        TensorType a = create_tensor<TensorType>(shape_a, DataType::QASYMM8, 1);
        TensorType b = create_tensor<TensorType>(shape_b, DataType::QASYMM8, 1);
        TensorType c = create_tensor<TensorType>(shape_c, DataType::S32, 1);

        a.info()->set_quantization_info(QuantizationInfo(1.0f / 255, a_offset));
        b.info()->set_quantization_info(QuantizationInfo(1.0f / 255, b_offset));

        // Create and configure function
        FunctionType gemmlowp;
        gemmlowp.configure(&a, &b, &c);

        ARM_COMPUTE_EXPECT(a.info()->is_resizable(), framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(b.info()->is_resizable(), framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(c.info()->is_resizable(), framework::LogLevel::ERRORS);

        // Allocate tensors
        a.allocator()->allocate();
        b.allocator()->allocate();
        c.allocator()->allocate();

        ARM_COMPUTE_EXPECT(!a.info()->is_resizable(), framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(!b.info()->is_resizable(), framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(!c.info()->is_resizable(), framework::LogLevel::ERRORS);

        // Fill tensors
        fill(AccessorType(a), 0);
        fill(AccessorType(b), 1);

        // Compute GEMM function
        gemmlowp.run();
        return c;
    }

    SimpleTensor<int32_t> compute_reference(const TensorShape &shape_a, const TensorShape &shape_b, const TensorShape &shape_c,
                                            int32_t a_offset, int32_t b_offset)
    {
        // Create reference
        SimpleTensor<uint8_t> a{ shape_a, DataType::QASYMM8, 1 };
        SimpleTensor<uint8_t> b{ shape_b, DataType::QASYMM8, 1 };

        // Fill reference
        fill(a, 0);
        fill(b, 1);

        return reference::gemmlowp_matrix_multiply_core<uint8_t>(a, b, a_offset, b_offset);
    }

    TensorType            _target{};
    SimpleTensor<int32_t> _reference{};
};

template <typename TensorType, typename AccessorType, typename FunctionType>
class GEMMLowpQuantizeDownInt32ToUint8ScaleValidationFixture : public framework::Fixture
{
public:
    template <typename...>
    void setup(TensorShape shape, int32_t result_offset, int32_t result_mult_int, int32_t result_shift)
    {
        _target    = compute_target(shape, result_offset, result_mult_int, result_shift);
        _reference = compute_reference(shape, result_offset, result_mult_int, result_shift);
    }

protected:
    template <typename U>
    void fill(U &&tensor, int i)
    {
        std::uniform_int_distribution<> distribution(-6000, 6000);
        library->fill(tensor, distribution, i);
    }

    TensorType compute_target(const TensorShape &shape, int32_t result_offset, int32_t result_mult_int, int32_t result_shift)
    {
        // Create tensors
        TensorType a = create_tensor<TensorType>(shape, DataType::S32, 1);
        TensorType b = create_tensor<TensorType>(shape, DataType::QASYMM8, 1);

        // Create and configure function
        FunctionType output_stage;
        output_stage.configure(&a, &b, result_offset, result_mult_int, result_shift);

        ARM_COMPUTE_EXPECT(a.info()->is_resizable(), framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(b.info()->is_resizable(), framework::LogLevel::ERRORS);

        // Allocate tensors
        a.allocator()->allocate();
        b.allocator()->allocate();

        ARM_COMPUTE_EXPECT(!a.info()->is_resizable(), framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(!b.info()->is_resizable(), framework::LogLevel::ERRORS);

        // Fill tensors
        fill(AccessorType(a), 0);

        // Compute GEMM function
        output_stage.run();
        return b;
    }

    SimpleTensor<uint8_t> compute_reference(const TensorShape &shape, int32_t result_offset, int32_t result_mult_int, int32_t result_shift)
    {
        // Create reference
        SimpleTensor<int32_t> a{ shape, DataType::S32, 1 };

        // Fill reference
        fill(a, 0);

        return reference::gemmlowp_quantize_down_int32_to_uint8_scale<int32_t>(a, result_offset, result_mult_int, result_shift);
    }

    TensorType            _target{};
    SimpleTensor<uint8_t> _reference{};
};
} // namespace validation
} // namespace test
} // namespace arm_compute
#endif /* ARM_COMPUTE_TEST_GEMMLOWP_FIXTURE */

/*
 * Copyright (c) 2018 ARM Limited.
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
#ifndef ARM_COMPUTE_TEST_STRIDED_SLICE_FIXTURE
#define ARM_COMPUTE_TEST_STRIDED_SLICE_FIXTURE

#include "arm_compute/core/TensorShape.h"
#include "arm_compute/core/Types.h"

#include "tests/AssetsLibrary.h"
#include "tests/Globals.h"
#include "tests/IAccessor.h"
#include "tests/RawLutAccessor.h"
#include "tests/framework/Asserts.h"
#include "tests/framework/Fixture.h"
#include "tests/validation/Helpers.h"
#include "tests/validation/reference/StridedSlice.h"

namespace arm_compute
{
namespace test
{
namespace validation
{
template <typename TensorType, typename AccessorType, typename FunctionType, typename T>
class StridedSliceFixture : public framework::Fixture
{
public:
    template <typename...>
    void setup(TensorShape shape,
               Coordinates starts, Coordinates ends, BiStrides strides,
               int32_t begin_mask, int32_t end_mask, int32_t shrink_mask,
               DataType data_type)
    {
        _target    = compute_target(shape, starts, ends, strides, begin_mask, end_mask, shrink_mask, data_type);
        _reference = compute_reference(shape, starts, ends, strides, begin_mask, end_mask, shrink_mask, data_type);
    }

protected:
    template <typename U>
    void fill(U &&tensor, int i)
    {
        library->fill_tensor_uniform(tensor, i);
    }

    TensorType compute_target(const TensorShape &shape,
                              const Coordinates &starts, const Coordinates &ends, const BiStrides &strides,
                              int32_t begin_mask, int32_t end_mask, int32_t shrink_mask,
                              DataType data_type)
    {
        // Create tensors
        TensorType src = create_tensor<TensorType>(shape, data_type);
        TensorType dst;

        // Create and configure function
        FunctionType strided_slice;
        strided_slice.configure(&src, &dst, starts, ends, strides, begin_mask, end_mask, shrink_mask);

        ARM_COMPUTE_EXPECT(src.info()->is_resizable(), framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(dst.info()->is_resizable(), framework::LogLevel::ERRORS);

        // Allocate tensors
        src.allocator()->allocate();
        dst.allocator()->allocate();

        ARM_COMPUTE_EXPECT(!src.info()->is_resizable(), framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(!dst.info()->is_resizable(), framework::LogLevel::ERRORS);

        // Fill tensors
        fill(AccessorType(src), 0);
        fill(AccessorType(dst), 1);

        // Compute function
        strided_slice.run();

        return dst;
    }

    SimpleTensor<T> compute_reference(const TensorShape &shape,
                                      const Coordinates &starts, const Coordinates &ends, const BiStrides &strides,
                                      int32_t begin_mask, int32_t end_mask, int32_t shrink_mask,
                                      DataType data_type)
    {
        // Create reference
        SimpleTensor<T> src{ shape, data_type };

        // Fill reference
        fill(src, 0);

        return reference::strided_slice(src, starts, ends, strides, begin_mask, end_mask, shrink_mask);
    }

    TensorType      _target{};
    SimpleTensor<T> _reference{};
};
} // namespace validation
} // namespace test
} // namespace arm_compute
#endif /* ARM_COMPUTE_TEST_STRIDED_SLICE_FIXTURE */
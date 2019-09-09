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
#include "arm_compute/runtime/Allocator.h"
#include "arm_compute/runtime/MemoryGroup.h"
#include "arm_compute/runtime/MemoryManagerOnDemand.h"
#include "arm_compute/runtime/NEON/functions/NENormalizationLayer.h"
#include "arm_compute/runtime/OffsetLifetimeManager.h"
#include "arm_compute/runtime/PoolManager.h"
#include "support/ToolchainSupport.h"
#include "tests/AssetsLibrary.h"
#include "tests/Globals.h"
#include "tests/NEON/Accessor.h"
#include "tests/Utils.h"
#include "tests/framework/Asserts.h"
#include "tests/framework/Macros.h"
#include "tests/framework/datasets/Datasets.h"
#include "tests/validation/fixtures/UNIT/DynamicTensorFixture.h"
#include <arm_compute/runtime/OffsetMemoryPool.h>

namespace arm_compute
{
namespace test
{
namespace validation
{
TEST_SUITE(NEON)
TEST_SUITE(UNIT)
TEST_SUITE(DynamicTensor)

using NEDynamicTensorType3SingleFunction = DynamicTensorType3SingleFunction<Tensor, Accessor, Allocator, OffsetLifetimeManager, PoolManager, MemoryManagerOnDemand, NENormalizationLayer>;

/** Tests the memory manager with dynamic input and output tensors.
 *
 *  Create and manage the tensors needed to run a simple function. After the function is executed,
 *  change the input and output size requesting more memory and go through the manage/allocate process.
 *  The memory manager should be able to update the inner structures and allocate the requested memory
 * */
FIXTURE_DATA_TEST_CASE(DynamicTensorType3Single, NEDynamicTensorType3SingleFunction, framework::DatasetMode::ALL,
                       framework::dataset::zip(framework::dataset::make("Level0Shape", { TensorShape(12U, 11U, 3U), TensorShape(256U, 8U, 12U) }),
                                               framework::dataset::make("Level1Shape", { TensorShape(67U, 31U, 15U), TensorShape(11U, 2U, 3U) })))
{
    if(input_l0.total_size() < input_l1.total_size())
    {
        ARM_COMPUTE_EXPECT(internal_l0.size < internal_l1.size, framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(cross_l0.size < cross_l1.size, framework::LogLevel::ERRORS);
    }
    else
    {
        ARM_COMPUTE_EXPECT(internal_l0.size == internal_l1.size, framework::LogLevel::ERRORS);
        ARM_COMPUTE_EXPECT(cross_l0.size == cross_l1.size, framework::LogLevel::ERRORS);
    }
}

TEST_SUITE_END() // DynamicTensor
TEST_SUITE_END() // UNIT
TEST_SUITE_END() // NEON
} // namespace validation
} // namespace test
} // namespace arm_compute

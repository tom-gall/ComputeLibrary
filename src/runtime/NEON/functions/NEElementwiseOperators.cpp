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
 * IMPLIED, INNEUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY NEAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "arm_compute/runtime/NEON/functions/NEElementwiseOperations.h"
#include <arm_compute/core/NEON/kernels/NEElementwiseOperationKernel.h>

#include "arm_compute/core/ITensor.h"
#include "support/ToolchainSupport.h"

#include <utility>

namespace arm_compute
{
void NEElementwiseMax::configure(ITensor *input1, ITensor *input2, ITensor *output)
{
    auto k = arm_compute::support::cpp14::make_unique<NEArithmeticOperationKernel>();
    k->configure(ArithmeticOperation::MAX, input1, input2, output);
    _kernel = std::move(k);
}

Status NEElementwiseMax::validate(const ITensorInfo *input1, const ITensorInfo *input2, const ITensorInfo *output)
{
    return NEArithmeticOperationKernel::validate(ArithmeticOperation::MAX, input1, input2, output);
}

void NEElementwiseMin::configure(ITensor *input1, ITensor *input2, ITensor *output)
{
    auto k = arm_compute::support::cpp14::make_unique<NEArithmeticOperationKernel>();
    k->configure(ArithmeticOperation::MIN, input1, input2, output);
    _kernel = std::move(k);
}

Status NEElementwiseMin::validate(const ITensorInfo *input1, const ITensorInfo *input2, const ITensorInfo *output)
{
    return NEArithmeticOperationKernel::validate(ArithmeticOperation::MIN, input1, input2, output);
}

void NEElementwiseSquaredDiff::configure(ITensor *input1, ITensor *input2, ITensor *output)
{
    auto k = arm_compute::support::cpp14::make_unique<NEArithmeticOperationKernel>();
    k->configure(ArithmeticOperation::SQUARED_DIFF, input1, input2, output);
    _kernel = std::move(k);
}

Status NEElementwiseSquaredDiff::validate(const ITensorInfo *input1, const ITensorInfo *input2, const ITensorInfo *output)
{
    return NEArithmeticOperationKernel::validate(ArithmeticOperation::SQUARED_DIFF, input1, input2, output);
}
} // namespace arm_compute
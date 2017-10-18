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
#include "arm_compute/runtime/NEON/functions/NEDepthwiseConvolution.h"

#include "arm_compute/core/Helpers.h"
#include "arm_compute/core/ITensor.h"
#include "arm_compute/core/PixelValue.h"
#include "arm_compute/runtime/NEON/NEScheduler.h"
#include "support/ToolchainSupport.h"

using namespace arm_compute;

NEDepthwiseConvolution3x3::NEDepthwiseConvolution3x3()
    : _kernel(), _bias_kernel(), _border_handler(), _has_bias(false)
{
}

void NEDepthwiseConvolution3x3::configure(ITensor *input, ITensor *output, const ITensor *weights, const ITensor *biases, const PadStrideInfo &conv_info)
{
    ARM_COMPUTE_ERROR_ON_DATA_TYPE_CHANNEL_NOT_IN(input, 1, DataType::F32);
    ARM_COMPUTE_ERROR_ON_MISMATCHING_DATA_TYPES(input, output, weights);

    // Call convolution kernel
    _kernel.configure(input, output, weights, conv_info);
    _border_handler.configure(input, _kernel.border_size(), BorderMode::CONSTANT, PixelValue(static_cast<float>(0.f)));
    if(biases != nullptr)
    {
        _bias_kernel.configure(output, biases);
        _has_bias = true;
    }
}

void NEDepthwiseConvolution3x3::run()
{
    NEScheduler::get().schedule(&_border_handler, Window::DimX);
    NEScheduler::get().schedule(&_kernel, Window::DimX);
    if(_has_bias)
    {
        NEScheduler::get().schedule(&_bias_kernel, Window::DimX);
    }
}
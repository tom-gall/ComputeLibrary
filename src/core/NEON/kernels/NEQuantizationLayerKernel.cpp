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
#include "arm_compute/core/NEON/kernels/NEQuantizationLayerKernel.h"

#include "arm_compute/core/Error.h"
#include "arm_compute/core/Helpers.h"
#include "arm_compute/core/TensorInfo.h"
#include "arm_compute/core/Utils.h"
#include "arm_compute/core/Validate.h"
#include "arm_compute/core/Window.h"

#include <arm_neon.h>

using namespace arm_compute;

NEQuantizationLayerKernel::NEQuantizationLayerKernel()
    : _input(nullptr), _output(nullptr), _min(nullptr), _max(nullptr)
{
}

void NEQuantizationLayerKernel::configure(const ITensor *input, ITensor *output, const float *min, const float *max)
{
    ARM_COMPUTE_ERROR_ON_DATA_TYPE_CHANNEL_NOT_IN(input, 1, DataType::F32);
    ARM_COMPUTE_ERROR_ON_NULLPTR(output);

    // Output tensor auto initialization if not yet initialized
    auto_init_if_empty(*output->info(), input->info()->tensor_shape(), 1, DataType::U8, 0);

    ARM_COMPUTE_ERROR_ON_DATA_TYPE_CHANNEL_NOT_IN(output, 1, DataType::U8);
    ARM_COMPUTE_ERROR_ON_MISMATCHING_SHAPES(input, output);

    _input  = input;
    _output = output;
    _min    = min;
    _max    = max;

    constexpr unsigned int num_elems_processed_per_iteration = 8;

    // Configure window
    Window                 win = calculate_max_window(*input->info(), Steps(num_elems_processed_per_iteration));
    AccessWindowHorizontal output_access(output->info(), 0, num_elems_processed_per_iteration);
    update_window_and_padding(win, AccessWindowHorizontal(input->info(), 0, num_elems_processed_per_iteration), output_access);
    output_access.set_valid_region(win, input->info()->valid_region());

    INEKernel::configure(win);
}

void NEQuantizationLayerKernel::run(const Window &window)
{
    ARM_COMPUTE_ERROR_ON_UNCONFIGURED_KERNEL(this);
    ARM_COMPUTE_ERROR_ON_INVALID_SUBWINDOW(INEKernel::window(), window);

    Iterator input(_input, window);
    Iterator output(_output, window);

    const float32x4_t vmin             = vdupq_n_f32(*_min);
    const float32x4_t inv_range        = vdupq_n_f32(1.0f / (*_max - *_min));
    const float32x4_t quantization_max = vdupq_n_f32(255.0f);
    const float32x4_t quantization_mul = vdupq_n_f32(256.0f);

    // Uniformly map values to range 8bit integers, i.e. [min, max] -> [0, 255]
    execute_window_loop(window, [&](const Coordinates & id)
    {
        float32x4x2_t val = vld2q_f32(reinterpret_cast<const float *>(input.ptr()));
        // Map float values to range [0.0, 1.0]
        val.val[0] = vsubq_f32(val.val[0], vmin);
        val.val[1] = vsubq_f32(val.val[1], vmin);
        val.val[0] = vmulq_f32(val.val[0], inv_range);
        val.val[1] = vmulq_f32(val.val[1], inv_range);

        // Quantize
        val.val[0] = vmulq_f32(val.val[0], quantization_mul);
        val.val[1] = vmulq_f32(val.val[1], quantization_mul);
        val.val[0] = vminq_f32(val.val[0], quantization_max);
        val.val[1] = vminq_f32(val.val[1], quantization_max);

        const uint32x4_t   val_u32_low  = vcvtq_u32_f32(val.val[0]);
        const uint32x4_t   val_u32_high = vcvtq_u32_f32(val.val[1]);
        const uint16x4x2_t val_u16      = vzip_u16(vmovn_u32(val_u32_low), vmovn_u32(val_u32_high));

        const uint8x8_t quantized = vmovn_u16(vcombine_u16(val_u16.val[0], val_u16.val[1]));
        vst1_u8(reinterpret_cast<uint8_t *>(output.ptr()), quantized);
    },
    input, output);
}

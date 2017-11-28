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
#include "arm_compute/core/NEON/kernels/NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel.h"

#include "arm_compute/core/AccessWindowStatic.h"
#include "arm_compute/core/Error.h"
#include "arm_compute/core/Helpers.h"
#include "arm_compute/core/ITensor.h"
#include "arm_compute/core/NEON/NEAsymm.h"
#include "arm_compute/core/Types.h"
#include "arm_compute/core/Utils.h"
#include "arm_compute/core/Validate.h"
#include "arm_compute/core/Window.h"

#include <arm_neon.h>
#include <cstddef>
#include <cstdint>

using namespace arm_compute;

namespace
{
Error validate_arguments(const ITensorInfo *input, const ITensorInfo *bias, const ITensorInfo *output, int min, int max)
{
    ARM_COMPUTE_RETURN_ERROR_ON_DATA_TYPE_CHANNEL_NOT_IN(input, 1, DataType::S32);
    ARM_COMPUTE_RETURN_ERROR_ON_DATA_TYPE_CHANNEL_NOT_IN(output, 1, DataType::QASYMM8);
    ARM_COMPUTE_RETURN_ERROR_ON_MISMATCHING_SHAPES(input, output);
    ARM_COMPUTE_RETURN_ERROR_ON(max > 255);
    ARM_COMPUTE_RETURN_ERROR_ON(min < 0 || min > max);

    // Check biases if exist
    if(bias != nullptr)
    {
        ARM_COMPUTE_RETURN_ERROR_ON_MISMATCHING_DATA_TYPES(input, bias);
        ARM_COMPUTE_RETURN_ERROR_ON(bias->num_dimensions() > 1);
        ARM_COMPUTE_RETURN_ERROR_ON(input->dimension(0) != bias->dimension(0));
    }
    return Error{};
}

std::pair<Error, Window> validate_and_configure_window(ITensorInfo *input, ITensorInfo *bias, ITensorInfo *output)
{
    constexpr unsigned int num_elems_processed_per_iteration = 16;

    // Configure kernel window
    Window win = calculate_max_window(*output, Steps(num_elems_processed_per_iteration));

    AccessWindowHorizontal input_access(input, 0, num_elems_processed_per_iteration);
    AccessWindowHorizontal output_result_access(output, 0, num_elems_processed_per_iteration);

    bool window_changed = update_window_and_padding(win,
                                                    input_access,
                                                    output_result_access);

    if(bias != nullptr)
    {
        AccessWindowStatic bias_access(bias, 0, 0, ceil_to_multiple(bias->dimension(0), num_elems_processed_per_iteration), bias->tensor_shape()[1]);
        window_changed = window_changed || update_window_and_padding(win, bias_access);
    }

    output_result_access.set_valid_region(win, ValidRegion(Coordinates(), output->tensor_shape()));

    Error err = (window_changed) ? ARM_COMPUTE_CREATE_ERROR(ErrorCode::RUNTIME_ERROR, "Insufficient Padding!") : Error{};
    return std::make_pair(err, win);
}

template <bool    is_bounded_relu>
inline uint8x16_t finalize_quantization(int32x4x4_t &in_s32, int result_fixedpoint_multiplier, int32_t result_shift, int32x4_t result_offset_after_shift_s32, uint8x16_t min_u8,
                                        uint8x16_t max_u8)
{
    const static int32x4_t zero_s32 = vdupq_n_s32(0);

    // Fixed point multiplication with vector saturating rounding doubling multiply high with scalar
    in_s32.val[0] = vqrdmulhq_n_s32(in_s32.val[0], result_fixedpoint_multiplier);
    in_s32.val[1] = vqrdmulhq_n_s32(in_s32.val[1], result_fixedpoint_multiplier);
    in_s32.val[2] = vqrdmulhq_n_s32(in_s32.val[2], result_fixedpoint_multiplier);
    in_s32.val[3] = vqrdmulhq_n_s32(in_s32.val[3], result_fixedpoint_multiplier);

    // Round to the nearest division by a power-of-two using result_shift_s32
    in_s32.val[0] = rounding_divide_by_pow2(in_s32.val[0], result_shift);
    in_s32.val[1] = rounding_divide_by_pow2(in_s32.val[1], result_shift);
    in_s32.val[2] = rounding_divide_by_pow2(in_s32.val[2], result_shift);
    in_s32.val[3] = rounding_divide_by_pow2(in_s32.val[3], result_shift);

    // Add the offset terms
    in_s32.val[0] = vaddq_s32(in_s32.val[0], result_offset_after_shift_s32);
    in_s32.val[1] = vaddq_s32(in_s32.val[1], result_offset_after_shift_s32);
    in_s32.val[2] = vaddq_s32(in_s32.val[2], result_offset_after_shift_s32);
    in_s32.val[3] = vaddq_s32(in_s32.val[3], result_offset_after_shift_s32);

    // Saturate negative values
    in_s32.val[0] = vmaxq_s32(in_s32.val[0], zero_s32);
    in_s32.val[1] = vmaxq_s32(in_s32.val[1], zero_s32);
    in_s32.val[2] = vmaxq_s32(in_s32.val[2], zero_s32);
    in_s32.val[3] = vmaxq_s32(in_s32.val[3], zero_s32);

    // Convert S32 to S16
    const int16x8x2_t in_s16 =
    {
        {
            vcombine_s16(vqmovn_s32(in_s32.val[0]), vqmovn_s32(in_s32.val[1])),
            vcombine_s16(vqmovn_s32(in_s32.val[2]), vqmovn_s32(in_s32.val[3]))
        }
    };

    // Convert S16 to U8
    uint8x16_t out_u8 = vcombine_u8(vqmovun_s16(in_s16.val[0]), vqmovun_s16(in_s16.val[1]));

    if(is_bounded_relu)
    {
        out_u8 = vmaxq_u8(out_u8, min_u8);
        out_u8 = vminq_u8(out_u8, max_u8);
    }

    return out_u8;
}
} // namespace

namespace arm_compute
{
class Coordinates;
} // namespace arm_compute

template <bool is_bounded_relu>
void NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel::run(const Window &window)
{
    const int32x4_t  result_offset_after_shift_s32 = vdupq_n_s32(_result_offset_after_shift);
    const uint8x16_t min_u8                        = vdupq_n_u8(static_cast<uint8_t>(_min));
    const uint8x16_t max_u8                        = vdupq_n_u8(static_cast<uint8_t>(_max));

    ARM_COMPUTE_UNUSED(min_u8);
    ARM_COMPUTE_UNUSED(max_u8);

    Iterator in(_input, window);
    Iterator out(_output, window);

    if(_bias != nullptr)
    {
        Window win_biases;
        win_biases.set(Window::DimX, Window::Dimension(window.x().start(), window.x().end(), window.x().step()));
        win_biases.set(Window::DimY, Window::Dimension(0, 1, 1));

        Iterator bias(_bias, win_biases);
        execute_window_loop(window, [&](const Coordinates & id)
        {
            int32x4x4_t in_s32 =
            {
                {
                    vld1q_s32(reinterpret_cast<const int32_t *>(in.ptr()) + 0),
                    vld1q_s32(reinterpret_cast<const int32_t *>(in.ptr()) + 4),
                    vld1q_s32(reinterpret_cast<const int32_t *>(in.ptr()) + 8),
                    vld1q_s32(reinterpret_cast<const int32_t *>(in.ptr()) + 12)
                }
            };

            const int32x4x4_t bias_s32 =
            {
                {
                    vld1q_s32(reinterpret_cast<const int32_t *>(bias.ptr()) + 0),
                    vld1q_s32(reinterpret_cast<const int32_t *>(bias.ptr()) + 4),
                    vld1q_s32(reinterpret_cast<const int32_t *>(bias.ptr()) + 8),
                    vld1q_s32(reinterpret_cast<const int32_t *>(bias.ptr()) + 12)
                }
            };

            // Add the bias to GEMM's result
            in_s32.val[0] = vaddq_s32(in_s32.val[0], bias_s32.val[0]);
            in_s32.val[1] = vaddq_s32(in_s32.val[1], bias_s32.val[1]);
            in_s32.val[2] = vaddq_s32(in_s32.val[2], bias_s32.val[2]);
            in_s32.val[3] = vaddq_s32(in_s32.val[3], bias_s32.val[3]);

            vst1q_u8(out.ptr(), finalize_quantization<is_bounded_relu>(in_s32, _result_fixedpoint_multiplier, _result_shift, result_offset_after_shift_s32, min_u8, max_u8));
        },
        in, bias, out);
    }
    else
    {
        execute_window_loop(window, [&](const Coordinates & id)
        {
            int32x4x4_t in_s32 =
            {
                {
                    vld1q_s32(reinterpret_cast<const int32_t *>(in.ptr()) + 0),
                    vld1q_s32(reinterpret_cast<const int32_t *>(in.ptr()) + 4),
                    vld1q_s32(reinterpret_cast<const int32_t *>(in.ptr()) + 8),
                    vld1q_s32(reinterpret_cast<const int32_t *>(in.ptr()) + 12)
                }
            };

            vst1q_u8(out.ptr(), finalize_quantization<is_bounded_relu>(in_s32, _result_fixedpoint_multiplier, _result_shift, result_offset_after_shift_s32, min_u8, max_u8));
        },
        in, out);
    }
}

NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel::NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel()
    : _func(nullptr), _input(nullptr), _bias(nullptr), _output(nullptr), _result_fixedpoint_multiplier(0), _result_shift(0), _result_offset_after_shift(0), _min(0), _max(0)
{
}

void NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel::configure(const ITensor *input, const ITensor *bias, ITensor *output, int result_fixedpoint_multiplier, int result_shift,
                                                                          int result_offset_after_shift, int min, int max)
{
    // Perform validate step
    ARM_COMPUTE_ERROR_ON_NULLPTR(input, output);

    // Output auto inizialitation if not yet initialized
    auto_init_if_empty(*output->info(), input->info()->clone()->set_data_type(DataType::QASYMM8));

    ARM_COMPUTE_ERROR_THROW_ON(validate_arguments(input->info(),
                                                  (bias != nullptr) ? bias->info() : nullptr,
                                                  output->info(),
                                                  min,
                                                  max));

    _input                        = input;
    _bias                         = bias;
    _output                       = output;
    _result_fixedpoint_multiplier = result_fixedpoint_multiplier;
    _result_shift                 = result_shift;
    _result_offset_after_shift    = result_offset_after_shift;
    _min                          = min;
    _max                          = max;

    // Configure kernel window
    auto win_config = validate_and_configure_window(input->info(), (bias != nullptr) ? bias->info() : nullptr, output->info());
    ARM_COMPUTE_ERROR_THROW_ON(win_config.first);
    INEKernel::configure(win_config.second);

    // Check if we need to clamp the result using min and max
    const bool is_bounded_relu = ((min != max) && !(min == 0 && max == 255));
    _func                      = is_bounded_relu ? &NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel::run<true> : &NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel::run<false>;
}

Error NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel::validate(const ITensorInfo *input, const ITensorInfo *bias, const ITensorInfo *output, int min, int max)
{
    ARM_COMPUTE_RETURN_ON_ERROR(validate_arguments(input, bias, output, min, max));
    ARM_COMPUTE_RETURN_ON_ERROR(validate_and_configure_window(input->clone().get(),
                                                              (bias != nullptr) ? bias->clone().get() : nullptr,
                                                              output->clone().get())
                                .first);

    return Error{};
}

void NEGEMMLowpQuantizeDownInt32ToUint8ScaleByFixedPointKernel::run(const Window &window, const ThreadInfo &info)
{
    ARM_COMPUTE_UNUSED(info);
    ARM_COMPUTE_ERROR_ON_UNCONFIGURED_KERNEL(this);
    ARM_COMPUTE_ERROR_ON_INVALID_SUBWINDOW(INEKernel::window(), window);

    (this->*_func)(window);
}
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
#ifndef __ARM_COMPUTE_NEREORGLAYERKERNEL_H__
#define __ARM_COMPUTE_NEREORGLAYERKERNEL_H__

#include "arm_compute/core/NEON/INEKernel.h"

namespace arm_compute
{
// Forward declarations
class ITensor;

/** Interface for the kernel to perform tensor re-organization */
class NEReorgLayerKernel : public INEKernel
{
public:
    const char *name() const override
    {
        return "NEReorgLayerKernel";
    }
    /** Default constructor */
    NEReorgLayerKernel();
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    NEReorgLayerKernel(const NEReorgLayerKernel &) = delete;
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    NEReorgLayerKernel &operator=(const NEReorgLayerKernel &) = delete;
    /** Default Move Constructor. */
    NEReorgLayerKernel(NEReorgLayerKernel &&) = default;
    /** Default move assignment operator */
    NEReorgLayerKernel &operator=(NEReorgLayerKernel &&) = default;
    /** Default destructor */
    ~NEReorgLayerKernel() = default;
    /** Set the input and output of the kernel
     *
     * @param[in]  input  Source tensor. Data type supported: U8/S8/U16/S16/QASYMM8/U32/S32/F16/F32
     * @param[out] output Destination tensor. Data type supported: Same as @p input
     * @param[in]  stride Stride to be used during data re-organization
     */
    void configure(const ITensor *input, ITensor *output, int32_t stride);

    /** Static function to check if given info will lead to a valid configuration of @ref NEReshapeLayerKernel
     *
     * @param[in] input  Source tensor info. Data type supported: U8/S8/U16/S16/QASYMM8/U32/S32/F16/F32
     * @param[in] output Destination tensor info. Data type supported: Same as @p input
     * @param[in] stride Stride to be used during data re-organization
     *
     * @return a status
     */
    static Status validate(const ITensorInfo *input, const ITensorInfo *output, int32_t stride);

    // Inherited methods overridden:
    void run(const Window &window, const ThreadInfo &info) override;

private:
    /** Template function to run the reorg
     *
     * @param[in] window Region on which to execute the kernel. (Must be a valid region of the window returned by window()).
     */
    template <typename T>
    void run_reorg(const Window &window);

    /** Common signature for all the specialised reorg functions
     *
     * @param[in] window Region on which to execute the kernel.
     */
    using ReorgFunctionPtr = void (NEReorgLayerKernel::*)(const Window &window);

private:
    ReorgFunctionPtr _func;
    const ITensor   *_input;
    ITensor         *_output;
    int32_t          _stride;
};
} // namespace arm_compute
#endif /*__ARM_COMPUTE_NEREORGLAYERKERNEL_H__ */

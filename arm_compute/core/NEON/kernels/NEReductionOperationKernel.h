/*
 * Copyright (c) 2017-2019 ARM Limited.
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
#ifndef __ARM_COMPUTE_NEREDUCTIONOPERATIONKERNEL_H__
#define __ARM_COMPUTE_NEREDUCTIONOPERATIONKERNEL_H__

#include "arm_compute/core/NEON/INEKernel.h"

namespace arm_compute
{
class ITensor;

/** NEON kernel to perform a reduction operation
 *
 * @note For ARG_MIN/ARG_MAX reduction, the indices are computed in unsigned
 *       32-bit (U32). It is the user's responsibility to check that the
 *       results do not overflow in case the output data type is set to signed
 *       32-bit integer (S32).
 */
class NEReductionOperationKernel : public INEKernel
{
public:
    const char *name() const override
    {
        return "NEReductionOperationKernel";
    }
    /** Default constructor */
    NEReductionOperationKernel();
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    NEReductionOperationKernel(const NEReductionOperationKernel &) = delete;
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    NEReductionOperationKernel &operator=(const NEReductionOperationKernel &) = delete;
    /** Allow instances of this class to be moved */
    NEReductionOperationKernel(NEReductionOperationKernel &&) = default;
    /** Allow instances of this class to be moved */
    NEReductionOperationKernel &operator=(NEReductionOperationKernel &&) = default;
    /** Default destructor */
    ~NEReductionOperationKernel() = default;

    /** Set the source, destination of the kernel
     *
     * @param[in]  input  Source tensor. Data type supported: QASYMM8/F16/F32. Data layouts supported: NCHW.
     * @param[out] output Destination tensor.Data types and data layouts supported: same as @p input, S32 for ARG_MIX/ARG_MAX.
     *                    Output will have the same number of dimensions as input.
     * @param[in]  axis   Axis along which to reduce. Supported reduction axis : 0
     * @param[in]  op     Reduction operation to perform.
     */
    void configure(const ITensor *input, ITensor *output, unsigned int axis, ReductionOperation op);

    /** Static function to check if given info will lead to a valid configuration of @ref NEReductionOperationKernel.
     *
     * @param[in] input  Source tensor info. Data type supported: QASYMM8/F16/F32. Data layouts supported: NCHW.
     * @param[in] output Destination tensor info.Data types and data layouts supported: same as @p input, S32 for ARG_MIX/ARG_MAX.
     *                   Output will have the same number of dimensions as input.
     * @param[in] axis   Axis along which to reduce. Supported reduction axis : 0
     * @param[in] op     Reduction operation to perform.
     *
     * @return a status
     */
    static Status validate(const ITensorInfo *input, const ITensorInfo *output, unsigned int axis, ReductionOperation op);

    // Inherited methods overridden:
    void run(const Window &window, const ThreadInfo &info) override;
    BorderSize border_size() const override;

private:
    const ITensor     *_input;
    ITensor           *_output;
    unsigned int       _reduction_axis;
    ReductionOperation _op;
    BorderSize         _border_size;
};
} // namespace arm_compute
#endif /*__ARM_COMPUTE_NEREDUCTIONOPERATIONKERNEL_H__ */

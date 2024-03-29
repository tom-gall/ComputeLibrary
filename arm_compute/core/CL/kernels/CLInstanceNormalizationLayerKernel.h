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
#ifndef __ARM_COMPUTE_CLINSTANCENORMALIZATIONLAYERKERNEL_H__
#define __ARM_COMPUTE_CLINSTANCENORMALIZATIONLAYERKERNEL_H__

#include "arm_compute/core/CL/ICLKernel.h"

namespace arm_compute
{
class ICLTensor;

/** Interface for performing an instance normalization */
class CLInstanceNormalizationLayerKernel : public ICLKernel
{
public:
    /** Constructor */
    CLInstanceNormalizationLayerKernel();
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    CLInstanceNormalizationLayerKernel(const CLInstanceNormalizationLayerKernel &) = delete;
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    CLInstanceNormalizationLayerKernel &operator=(const CLInstanceNormalizationLayerKernel &) = delete;
    /** Default Move Constructor. */
    CLInstanceNormalizationLayerKernel(CLInstanceNormalizationLayerKernel &&) = default;
    /** Default move assignment operator */
    CLInstanceNormalizationLayerKernel &operator=(CLInstanceNormalizationLayerKernel &&) = default;
    /** Default destructor */
    ~CLInstanceNormalizationLayerKernel() = default;

    /** Set the input and output tensors.
     *
     * @param[in, out] input   Source tensor. Data types supported: F16/F32. Data layout supported: NCHW
     * @param[out]     output  Destination tensor. Data types and data layouts supported: same as @p input.
     * @param[in]      gamma   (Optional) The scale scalar value applied to the normalized tensor. Defaults to 1.0
     * @param[in]      beta    (Optional) The offset scalar value applied to the normalized tensor. Defaults to 0.0
     * @param[in]      epsilon (Optional) Lower bound value for the normalization. Defaults to 1e-12
     */
    void configure(ICLTensor *input, ICLTensor *output, float gamma = 1.0f, float beta = 0.0f, float epsilon = 1e-12f);

    /** Static function to check if given info will lead to a valid configuration of @ref CLInstanceNormalizationLayer.
     *
     * @param[in] input   Source tensor info. In case of @p output tensor = nullptr this tensor will store the result of the normalization.
     *                    Data types supported: F16/F32. Data layout supported: NHWC, NCHW
     * @param[in] output  Destination tensor info. Data types and data layouts supported: same as @p input.
     * @param[in] gamma   (Optional) The scale scalar value applied to the normalized tensor. Defaults to 1.0
     * @param[in] beta    (Optional) The offset scalar value applied to the normalized tensor. Defaults to 0.0
     * @param[in] epsilon (Optional) Lower bound value for the normalization. Defaults to 1e-12
     *
     * @return a status
     */
    static Status validate(const ITensorInfo *input, const ITensorInfo *output, float gamma = 1.0f, float beta = 0.0f, float epsilon = 1e-12f);

    // Inherited methods overridden:
    void run(const Window &window, cl::CommandQueue &queue) override;

private:
    ICLTensor *_input;
    ICLTensor *_output;
    float      _gamma;
    float      _beta;
    float      _epsilon;
    bool       _run_in_place;
};
} // namespace arm_compute
#endif /*__ARM_COMPUTE_CLINSTANCENORMALIZATIONLAYERKERNEL_H__ */

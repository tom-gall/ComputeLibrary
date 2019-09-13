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
#ifndef __ARM_COMPUTE_NEGENERATEPROPOSALSLAYER_H__
#define __ARM_COMPUTE_NEGENERATEPROPOSALSLAYER_H__
#include "arm_compute/core/CPP/kernels/CPPBoxWithNonMaximaSuppressionLimitKernel.h"
#include "arm_compute/core/NEON/kernels/NEBoundingBoxTransformKernel.h"
#include "arm_compute/core/NEON/kernels/NECopyKernel.h"
#include "arm_compute/core/NEON/kernels/NEGenerateProposalsLayerKernel.h"
#include "arm_compute/core/NEON/kernels/NEMemsetKernel.h"
#include "arm_compute/core/NEON/kernels/NEPermuteKernel.h"
#include "arm_compute/core/NEON/kernels/NEReshapeLayerKernel.h"
#include "arm_compute/core/NEON/kernels/NEStridedSliceKernel.h"
#include "arm_compute/core/Types.h"
#include "arm_compute/runtime/CPP/CPPScheduler.h"
#include "arm_compute/runtime/IFunction.h"
#include "arm_compute/runtime/MemoryGroup.h"
#include "arm_compute/runtime/Tensor.h"

namespace arm_compute
{
class ITensor;

/** Basic function to generate proposals for a RPN (Region Proposal Network)
 *
 * This function calls the following Neon kernels:
 * -# @ref NEComputeAllAnchors
 * -# @ref NEPermute x 2
 * -# @ref NEReshapeLayer x 2
 * -# @ref NEStridedSlice x 3
 * -# @ref NEBoundingBoxTransform
 * -# @ref NECopyKernel
 * -# @ref NEMemsetKernel
 * And the following CPP kernels:
 * -# @ref CPPBoxWithNonMaximaSuppressionLimit
 */
class NEGenerateProposalsLayer : public IFunction
{
public:
    /** Default constructor
     *
     * @param[in] memory_manager (Optional) Memory manager.
     */
    NEGenerateProposalsLayer(std::shared_ptr<IMemoryManager> memory_manager = nullptr);
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    NEGenerateProposalsLayer(const NEGenerateProposalsLayer &) = delete;
    /** Default move constructor */
    NEGenerateProposalsLayer(NEGenerateProposalsLayer &&) = default;
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    NEGenerateProposalsLayer &operator=(const NEGenerateProposalsLayer &) = delete;
    /** Default move assignment operator */
    NEGenerateProposalsLayer &operator=(NEGenerateProposalsLayer &&) = default;

    /** Set the input and output tensors.
     *
     * @param[in]  scores              Scores from convolution layer of size (W, H, A), where H and W are the height and width of the feature map, and A is the number of anchors. Data types supported: F16/F32
     * @param[in]  deltas              Bounding box deltas from convolution layer of size (W, H, 4*A). Data types supported: Same as @p scores
     * @param[in]  anchors             Anchors tensor of size (4, A). Data types supported: Same as @p input
     * @param[out] proposals           Box proposals output tensor of size (5, W*H*A). Data types supported: Same as @p input
     * @param[out] scores_out          Box scores output tensor of size (W*H*A). Data types supported: Same as @p input
     * @param[out] num_valid_proposals Scalar output tensor which says which of the first proposals are valid. Data types supported: U32
     * @param[in]  info                Contains GenerateProposals operation information described in @ref GenerateProposalsInfo
     *
     * @note Only single image prediction is supported. Height and Width (and scale) of the image will be contained in the @ref GenerateProposalsInfo struct.
     * @note Proposals contains all the proposals. Of those, only the first num_valid_proposals are valid.
     */
    void configure(const ITensor *scores, const ITensor *deltas, const ITensor *anchors, ITensor *proposals, ITensor *scores_out, ITensor *num_valid_proposals,
                   const GenerateProposalsInfo &info);

    /** Static function to check if given info will lead to a valid configuration of @ref NEGenerateProposalsLayer
     *
     * @param[in] scores              Scores info from convolution layer of size (W, H, A), where H and W are the height and width of the feature map, and A is the number of anchors. Data types supported: F16/F32
     * @param[in] deltas              Bounding box deltas info from convolution layer of size (W, H, 4*A). Data types supported: Same as @p scores
     * @param[in] anchors             Anchors tensor info of size (4, A). Data types supported: Same as @p input
     * @param[in] proposals           Box proposals info  output tensor of size (5, W*H*A). Data types supported: Data types supported: U32
     * @param[in] scores_out          Box scores output tensor info of size (W*H*A). Data types supported: Same as @p input
     * @param[in] num_valid_proposals Scalar output tensor info which says which of the first proposals are valid. Data types supported: Same as @p input
     * @param[in] info                Contains GenerateProposals operation information described in @ref GenerateProposalsInfo
     *
     * @return a Status
     */
    static Status validate(const ITensorInfo *scores, const ITensorInfo *deltas, const ITensorInfo *anchors, const ITensorInfo *proposals, const ITensorInfo *scores_out,
                           const ITensorInfo           *num_valid_proposals,
                           const GenerateProposalsInfo &info);

    // Inherited methods overridden:
    void run() override;

private:
    // Memory group manager
    MemoryGroup _memory_group;

    // Neon kernels
    NEPermuteKernel              _permute_deltas_kernel;
    NEReshapeLayerKernel         _flatten_deltas_kernel;
    NEPermuteKernel              _permute_scores_kernel;
    NEReshapeLayerKernel         _flatten_scores_kernel;
    NEComputeAllAnchorsKernel    _compute_anchors_kernel;
    NEBoundingBoxTransformKernel _bounding_box_kernel;
    NEMemsetKernel               _memset_kernel;
    NECopyKernel                 _padded_copy_kernel;

    // CPP kernels
    CPPBoxWithNonMaximaSuppressionLimitKernel _cpp_nms_kernel;

    bool _is_nhwc;

    // Temporary tensors
    Tensor _deltas_permuted;
    Tensor _deltas_flattened;
    Tensor _scores_permuted;
    Tensor _scores_flattened;
    Tensor _all_anchors;
    Tensor _all_proposals;
    Tensor _keeps_nms_unused;
    Tensor _classes_nms_unused;
    Tensor _proposals_4_roi_values;

    // Output tensor pointers
    ITensor *_num_valid_proposals;
    ITensor *_scores_out;

    /** Internal function to run the CPP BoxWithNMS kernel */
    void run_cpp_nms_kernel();
};
} // namespace arm_compute
#endif /* __ARM_COMPUTE_NEGENERATEPROPOSALSLAYER_H__ */

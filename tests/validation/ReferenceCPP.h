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
#ifndef __ARM_COMPUTE_TEST_REFERENCE_REFERENCE_CPP_H__
#define __ARM_COMPUTE_TEST_REFERENCE_REFERENCE_CPP_H__

#include "RawTensor.h"
#include "Reference.h"

#include <map>
#include <memory>
#include <ostream>
#include <vector>

namespace arm_compute
{
class Tensor;

namespace test
{
namespace validation
{
/** C++ reference implementation. */
class ReferenceCPP final : public Reference
{
public:
    /** Function to compute reference sobel 3x3.
     *
     * @param[in] src                   Input tensor.
     * @param[in] dst_x                 Result tensor along x axis
     * @param[in] dst_y                 Result tensor along y axis
     * @param[in] border_mode           Border mode to use for input tensor
     * @param[in] constant_border_value Constant value to use if @p border_mode is constant
     *
     */
    static void sobel_3x3(RawTensor &src, RawTensor &dst_x, RawTensor &dst_y, BorderMode border_mode, uint8_t constant_border_value);
    /** Function to compute reference sobel 5x5.
     *
     * @param[in] src                   Input tensor.
     * @param[in] dst_x                 Result tensor along x axis
     * @param[in] dst_y                 Result tensor along y axis
     * @param[in] border_mode           Border mode to use for input tensor
     * @param[in] constant_border_value Constant value to use if @p border_mode is constant
     *
     */
    static void sobel_5x5(RawTensor &src, RawTensor &dst_x, RawTensor &dst_y, BorderMode border_mode, uint8_t constant_border_value);
    /** Function to compute reference Harris corners.
     *
     * @param[in]  src                   Input tensor
     * @param[in]  Gx                    Tensor used to compute Sobel along the x axis
     * @param[in]  Gy                    Tensor used to compute Sobel along the y axis
     * @param[in]  candidates            Tensor used to store candidate corners
     * @param[in]  non_maxima            Tensor used to store non_maxima suppressed candidate corners
     * @param[in]  threshold             Minimum threshold with which to eliminate Harris Corner scores (computed using the normalized Sobel kernel).
     * @param[in]  min_dist              Radial Euclidean distance for the euclidean distance stage
     * @param[in]  sensitivity           Sensitivity threshold k from the Harris-Stephens equation
     * @param[in]  gradient_size         The gradient window size to use on the input. The implementation supports 3, 5, and 7
     * @param[in]  block_size            The block window size used to compute the Harris Corner score. The implementation supports 3, 5, and 7.
     * @param[out] corners               Array of keypoints to store the results.
     * @param[in]  border_mode           Border mode to use
     * @param[in]  constant_border_value Constant value to use for borders if border_mode is set to CONSTANT.
     *
     */
    static void harris_corners(RawTensor &src, RawTensor &Gx, RawTensor &Gy, const RawTensor &candidates, const RawTensor &non_maxima, float threshold, float min_dist, float sensitivity,
                               int32_t gradient_size, int32_t block_size, KeyPointArray &corners, BorderMode border_mode, uint8_t constant_border_value);
    /** Function to compute the min max values and their location in a tensor.
     *
     * @param[in]  src       Input tensor.
     * @param[out] min       Minimum value of the tensor.
     * @param[out] max       Maximum value of the tensor
     * @param[out] min_loc   Array with locations of minimum values
     * @param[out] max_loc   Array with locations of maximum values
     * @param[out] min_count Number of minimum values found
     * @param[out] max_count Number of maximum values found
     */
    static void min_max_location(const RawTensor &src, void *min, void *max, IArray<Coordinates2D> &min_loc, IArray<Coordinates2D> &max_loc, uint32_t &min_count, uint32_t &max_count);
    /** Function to compute the integral image of a tensor.
     *
     * @param[in]  src Input tensor.
     * @param[out] dst Result tensor.
     */
    static void integral_image(const RawTensor &src, RawTensor &dst);
    /** Function to compute the absolute difference between two tensors.
     *
     * @param[in]  src1 First tensor.
     * @param[in]  src2 Second tensor.
     * @param[out] dst  Result tensor.
     */
    static void absolute_difference(const RawTensor &src1, const RawTensor &src2, RawTensor &dst);
    /** Function to accumulate an input tensor into an output tensor.
     *
     * @param[in]      src Input tensor.
     * @param[in, out] dst Result tensor.
     */
    static void accumulate(const RawTensor &src, RawTensor &dst);
    /** Function to accumulate a squared value from an input tensor to an output tensor.
     *
     * @param[in]      src   Input tensor.
     * @param[in, out] dst   Result tensor.
     * @param[in]      shift A uint32_t value within the range of [0, 15]
     */
    static void accumulate_squared(const RawTensor &src, RawTensor &dst, uint32_t shift);
    /** Function to accumulate a weighted value from an input tensor to an output tensor.
     *
     * @param[in]      src   Input tensor.
     * @param[in, out] dst   Result tensor.
     * @param[in]      alpha A float value within the range of [0, 1]
     */
    static void accumulate_weighted(const RawTensor &src, RawTensor &dst, float alpha);
    /** Arithmetic addition of @p src1 and @p src2
     *
     * @param[in]  src1           First tensor.
     * @param[in]  src2           Second tensor.
     * @param[out] dst            Result tensor.
     * @param[in]  convert_policy Overflow policy.
     */
    static void arithmetic_addition(const RawTensor &src1, const RawTensor &src2, RawTensor &dst, ConvertPolicy convert_policy);
    /** Arithmetic subtraction of @p src2 from @p src1
     *
     * @param[in]  src1           First tensor.
     * @param[in]  src2           Second tensor.
     * @param[out] dst            Result tensor.
     * @param[in]  convert_policy Overflow policy.
     */
    static void arithmetic_subtraction(const RawTensor &src1, const RawTensor &src2, RawTensor &dst, ConvertPolicy convert_policy);
    /** Function to compute box3x3 filtered result tensor.
     *
     * @param[in]  src                   Input tensor.
     * @param[out] dst                   Result tensor.
     * @param[in]  border_mode           Border mode.
     * @param[in]  constant_border_value Constant border value if @p border_mode is BorderMode::CONSTANT.
     */
    static void box3x3(const RawTensor &src, RawTensor &dst, BorderMode border_mode, uint8_t constant_border_value);
    /** Depth conversion from @p src to @p dst
     *
     * @param[in]  src    First tensor.
     * @param[out] dst    Result tensor.
     * @param[in]  policy Overflow policy.
     * @param[in]  shift  Value for down/up conversions.
     */
    static void depth_convert(const RawTensor &src, RawTensor &dst, ConvertPolicy policy, uint32_t shift);
    /** Function to compute gaussian3x3 filtered result tensor.
     *
     * @param[in]  src                   Input tensor.
     * @param[out] dst                   Result tensor.
     * @param[in]  border_mode           Border mode
     * @param[in]  constant_border_value Constant border value if @p border_mode is BorderMode::CONSTANT
     */
    static void gaussian3x3(const RawTensor &src, RawTensor &dst, BorderMode border_mode, uint8_t constant_border_value);
    /** Function to compute gaussian5x5 filtered result tensor.
     *
     * @param[in]  src                   Input tensor.
     * @param[out] dst                   Result tensor.
     * @param[in]  border_mode           Border mode
     * @param[in]  constant_border_value Constant border value if @p border_mode is BorderMode::CONSTANT
     */
    static void gaussian5x5(const RawTensor &src, RawTensor &dst, BorderMode border_mode, uint8_t constant_border_value);
    /** Compute non linear filter function.
     *
     * @param[in]  src                   First input tensor
     * @param[out] dst                   Output tensor
     * @param[in]  function              Non linear function to perform
     * @param[in]  mask_size             Mask size. Supported sizes: 3, 5
     * @param[in]  pattern               Matrix pattern
     * @param[in]  mask                  The given mask.
     * @param[in]  border_mode           Strategy to use for borders.
     * @param[in]  constant_border_value (Optional) Constant value to use for borders if border_mode is set to CONSTANT.
    */
    static void non_linear_filter(const RawTensor &src, RawTensor &dst, NonLinearFilterFunction function, unsigned int mask_size,
                                  MatrixPattern pattern, const uint8_t *mask, BorderMode border_mode, uint8_t constant_border_value = 0);
    /** Element-wise multiplication of @p src1, @p src2 and @p scale
     *
     * @param[in]  src1            First tensor.
     * @param[in]  src2            Second tensor.
     * @param[out] dst             Result tensor.
     * @param[in]  scale           A non-negative float multiplied to each product.
     * @param[in]  convert_policy  Overflow policy.
     * @param[in]  rounding_policy Rounding policy.
     */
    static void pixel_wise_multiplication(const RawTensor &src1, const RawTensor &src2, RawTensor &dst, float scale, ConvertPolicy convert_policy, RoundingPolicy rounding_policy);
    /** Fixed-point Pixel-wise multiplication of @p src1 by @p src2
     *
     * @param[in]  src1            First tensor.
     * @param[in]  src2            Second tensor.
     * @param[out] dst             Result tensor.
     * @param[in]  scale           A non-negative float multiplied to each product.
     * @param[in]  convert_policy  Overflow policy.
     * @param[in]  rounding_policy Rounding policy.
     */
    static void fixed_point_pixel_wise_multiplication(const RawTensor &src1, const RawTensor &src2, RawTensor &dst, float scale, ConvertPolicy convert_policy, RoundingPolicy rounding_policy);
    /** Table Lookup f@p src to @p dst
     *
     * @param[in]  src Input tensor.
     * @param[out] dst Result tensor.
     * @param[in]  lut Input lookup table.
     */
    template <typename T>
    static void table_lookup(const RawTensor &src, RawTensor &dst, std::map<T, T> &lut);
    /** Threshold of@p src to @p dst
     *
     * @param[in]  src         Input tensor.
     * @param[out] dst         Result tensor.
     * @param[in]  threshold   Threshold. When the threhold type is RANGE, this is used as the lower threshold.
     * @param[in]  false_value value to set when the condition is not respected.
     * @param[in]  true_value  value to set when the condition is respected.
     * @param[in]  type        Thresholding type. Either RANGE or BINARY.
     * @param[in]  upper       Upper threshold. Only used when the thresholding type is RANGE.
     */
    static void threshold(const RawTensor &src, RawTensor &dst, uint8_t threshold, uint8_t false_value, uint8_t true_value, ThresholdType type, uint8_t upper);
    /** Warp perspective of@p src to @p dst
     *
     * @param[in]  src                   First tensor.
     * @param[out] dst                   Result tensor.
     * @param[out] valid_mask            Valid mask tensor.
     * @param[in]  matrix                The perspective matrix. Must be 3x3 of type float.
     * @param[in]  policy                The interpolation type.
     * @param[in]  border_mode           Strategy to use for borders.
     * @param[in]  constant_border_value Constant value to use for borders if border_mode is set to CONSTANT.
     */
    static void warp_perspective(const RawTensor &src, RawTensor &dst, RawTensor &valid_mask, const float *matrix, InterpolationPolicy policy, BorderMode border_mode, uint8_t constant_border_value);

    /** Batch Normalization of @p src based on the information from @p norm_info.
     *
     * @param[in]  src                  Input tensor.
     * @param[out] dst                  Result tensor.
     * @param[out] mean                 Mean vector tensor.
     * @param[out] var                  Var vector tensor.
     * @param[out] beta                 Beta vector tensor.
     * @param[out] gamma                Gamma vector tensor.
     * @param[in]  epsilon              Small value to avoid division with zero.
     * @param[in]  fixed_point_position Fixed point position.
     */
    static void batch_normalization_layer(const RawTensor &src, RawTensor &dst, const RawTensor &mean, const RawTensor &var, const RawTensor &beta, const RawTensor &gamma, float epsilon,
                                          int fixed_point_position = 0);
    /** ROI Pooling layer of @p src based on the information from @p pool_info and @p rois.
     *
     * @param[in]  src       Input tensor.
     * @param[out] dst       Result tensor.
     * @param[in]  rois      Region of Interest points.
     * @param[in]  pool_info ROI Pooling Layer information.
     */
    static void roi_pooling_layer(const RawTensor &src, RawTensor &dst, const std::vector<ROI> &rois, const ROIPoolingLayerInfo &pool_info);
    /** Fixed point operations of @p src
     *
     * @param[in]  src Input tensor.
     * @param[out] dst Result tensor.
     * @param[in]  op  Fixed point operation to perform.
     */
    static void fixed_point_operation(const RawTensor &src, RawTensor &dst, FixedPointOp op);

private:
    ReferenceCPP()  = delete;
    ~ReferenceCPP() = delete;
};
} // namespace validation
} // namespace test
} // namespace arm_compute
#endif /* __ARM_COMPUTE_TEST_REFERENCE_REFERENCE_CPP_H__ */

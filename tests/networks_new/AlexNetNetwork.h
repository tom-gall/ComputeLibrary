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
#ifndef __ARM_COMPUTE_TEST_MODEL_OBJECTS_ALEXNET_H__
#define __ARM_COMPUTE_TEST_MODEL_OBJECTS_ALEXNET_H__

#include "AssetsLibrary.h"
#include "Globals.h"
#include "Utils.h"

#include <memory>

namespace arm_compute
{
namespace test
{
namespace networks
{
/** AlexNet model object */
template <typename ITensorType,
          typename TensorType,
          typename SubTensorType,
          typename Accessor,
          typename ActivationLayerFunction,
          typename ConvolutionLayerFunction,
          typename DirectConvolutionLayerFunction,
          typename FullyConnectedLayerFunction,
          typename NormalizationLayerFunction,
          typename PoolingLayerFunction,
          typename SoftmaxLayerFunction>
class AlexNetNetwork
{
public:
    void init(DataType data_type, int fixed_point_position, int batches, bool reshaped_weights = false)
    {
        _data_type            = data_type;
        _fixed_point_position = fixed_point_position;
        _batches              = batches;
        _reshaped_weights     = reshaped_weights;

        // Initialize weights and biases
        if(!_reshaped_weights)
        {
            w[0].allocator()->init(TensorInfo(TensorShape(11U, 11U, 3U, 96U), 1, _data_type, _fixed_point_position));
            b[0].allocator()->init(TensorInfo(TensorShape(96U), 1, _data_type, _fixed_point_position));
            w[1].allocator()->init(TensorInfo(TensorShape(5U, 5U, 48U, 256U), 1, _data_type, _fixed_point_position));
            b[1].allocator()->init(TensorInfo(TensorShape(256U), 1, _data_type, _fixed_point_position));
            w[2].allocator()->init(TensorInfo(TensorShape(3U, 3U, 256U, 384U), 1, _data_type, _fixed_point_position));
            b[2].allocator()->init(TensorInfo(TensorShape(384U), 1, _data_type, _fixed_point_position));
            w[3].allocator()->init(TensorInfo(TensorShape(3U, 3U, 192U, 384U), 1, _data_type, _fixed_point_position));
            b[3].allocator()->init(TensorInfo(TensorShape(384U), 1, _data_type, _fixed_point_position));
            w[4].allocator()->init(TensorInfo(TensorShape(3U, 3U, 192U, 256U), 1, _data_type, _fixed_point_position));
            b[4].allocator()->init(TensorInfo(TensorShape(256U), 1, _data_type, _fixed_point_position));
            w[5].allocator()->init(TensorInfo(TensorShape(9216U, 4096U), 1, _data_type, _fixed_point_position));
            b[5].allocator()->init(TensorInfo(TensorShape(4096U), 1, _data_type, _fixed_point_position));
            w[6].allocator()->init(TensorInfo(TensorShape(4096U, 4096U), 1, _data_type, _fixed_point_position));
            b[6].allocator()->init(TensorInfo(TensorShape(4096U), 1, _data_type, _fixed_point_position));
            w[7].allocator()->init(TensorInfo(TensorShape(4096U, 1000U), 1, _data_type, _fixed_point_position));
            b[7].allocator()->init(TensorInfo(TensorShape(1000U), 1, _data_type, _fixed_point_position));

            w21 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[1], TensorShape(5U, 5U, 48U, 128U), Coordinates()));
            w22 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[1], TensorShape(5U, 5U, 48U, 128U), Coordinates(0, 0, 0, 128)));
            b21 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[1], TensorShape(128U), Coordinates()));
            b22 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[1], TensorShape(128U), Coordinates(128)));

            w41 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[3], TensorShape(3U, 3U, 192U, 192U), Coordinates()));
            w42 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[3], TensorShape(3U, 3U, 192U, 192U), Coordinates(0, 0, 0, 192)));
            b41 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[3], TensorShape(192U), Coordinates()));
            b42 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[3], TensorShape(192U), Coordinates(192)));

            w51 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[4], TensorShape(3U, 3U, 192U, 128U), Coordinates()));
            w52 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[4], TensorShape(3U, 3U, 192U, 128U), Coordinates(0, 0, 0, 128)));
            b51 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[4], TensorShape(128U), Coordinates()));
            b52 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[4], TensorShape(128U), Coordinates(128)));
        }
        else
        {
            const unsigned int data_type_size = 16 / arm_compute::data_size_from_type(_data_type);

            // Create tensor for the reshaped weights
            auto w21_tensor = std::unique_ptr<TensorType>(new TensorType());
            auto w22_tensor = std::unique_ptr<TensorType>(new TensorType());

            w[0].allocator()->init(TensorInfo(TensorShape(366U * data_type_size, 96U / data_type_size), 1, _data_type, _fixed_point_position));
            w21_tensor->allocator()->init(TensorInfo(TensorShape(1248U * data_type_size, 128U / data_type_size), 1, _data_type, _fixed_point_position));
            w22_tensor->allocator()->init(TensorInfo(TensorShape(1248U * data_type_size, 128U / data_type_size), 1, _data_type, _fixed_point_position));
            w21 = std::move(w21_tensor);
            w22 = std::move(w22_tensor);

            // Configure the direct convolution's weights. Direct convolution doesn't need reshape weights
            if(!_is_direct_conv)
            {
                auto w41_tensor = std::unique_ptr<TensorType>(new TensorType());
                auto w42_tensor = std::unique_ptr<TensorType>(new TensorType());
                auto w51_tensor = std::unique_ptr<TensorType>(new TensorType());
                auto w52_tensor = std::unique_ptr<TensorType>(new TensorType());
                w41_tensor->allocator()->init(TensorInfo(TensorShape(1920U * data_type_size, 192U / data_type_size), 1, _data_type, _fixed_point_position));
                w42_tensor->allocator()->init(TensorInfo(TensorShape(1920U * data_type_size, 192U / data_type_size), 1, _data_type, _fixed_point_position));
                w51_tensor->allocator()->init(TensorInfo(TensorShape(1920U * data_type_size, 128U / data_type_size), 1, _data_type, _fixed_point_position));
                w52_tensor->allocator()->init(TensorInfo(TensorShape(1920U * data_type_size, 128U / data_type_size), 1, _data_type, _fixed_point_position));
                w[2].allocator()->init(TensorInfo(TensorShape(2560U * data_type_size, 384U / data_type_size), 1, _data_type, _fixed_point_position));
                w41 = std::move(w41_tensor);
                w42 = std::move(w42_tensor);
                w51 = std::move(w51_tensor);
                w52 = std::move(w52_tensor);
            }
            else
            {
                w[2].allocator()->init(TensorInfo(TensorShape(3U, 3U, 256U, 384U), 1, _data_type, _fixed_point_position));
                b[2].allocator()->init(TensorInfo(TensorShape(384U), 1, _data_type, _fixed_point_position));
                w[3].allocator()->init(TensorInfo(TensorShape(3U, 3U, 192U, 384U), 1, _data_type, _fixed_point_position));
                b[3].allocator()->init(TensorInfo(TensorShape(384U), 1, _data_type, _fixed_point_position));
                w[4].allocator()->init(TensorInfo(TensorShape(3U, 3U, 192U, 256U), 1, _data_type, _fixed_point_position));
                b[4].allocator()->init(TensorInfo(TensorShape(256U), 1, _data_type, _fixed_point_position));
                w41 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[3], TensorShape(3U, 3U, 192U, 192U), Coordinates()));
                w42 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[3], TensorShape(3U, 3U, 192U, 192U), Coordinates(0, 0, 0, 192)));
                b41 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[3], TensorShape(192U), Coordinates()));
                b42 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[3], TensorShape(192U), Coordinates(192)));

                w51 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[4], TensorShape(3U, 3U, 192U, 128U), Coordinates()));
                w52 = std::unique_ptr<SubTensorType>(new SubTensorType(&w[4], TensorShape(3U, 3U, 192U, 128U), Coordinates(0, 0, 0, 128)));
                b51 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[4], TensorShape(128U), Coordinates()));
                b52 = std::unique_ptr<SubTensorType>(new SubTensorType(&b[4], TensorShape(128U), Coordinates(128)));
            }

            b[5].allocator()->init(TensorInfo(TensorShape(4096U), 1, _data_type, _fixed_point_position));
            b[6].allocator()->init(TensorInfo(TensorShape(4096U), 1, _data_type, _fixed_point_position));
            b[7].allocator()->init(TensorInfo(TensorShape(1000U), 1, _data_type, _fixed_point_position));

            if(_batches > 1)
            {
                w[5].allocator()->init(TensorInfo(TensorShape(9216U * data_type_size, 4096U / data_type_size), 1, _data_type, _fixed_point_position));
                w[6].allocator()->init(TensorInfo(TensorShape(4096U * data_type_size, 4096U / data_type_size), 1, _data_type, _fixed_point_position));
                w[7].allocator()->init(TensorInfo(TensorShape(4096U * data_type_size, 1000U / data_type_size), 1, _data_type, _fixed_point_position));
            }
            else
            {
                w[5].allocator()->init(TensorInfo(TensorShape(4096U, 9216U), 1, _data_type, _fixed_point_position));
                w[6].allocator()->init(TensorInfo(TensorShape(4096U, 4096U), 1, _data_type, _fixed_point_position));
                w[7].allocator()->init(TensorInfo(TensorShape(1000U, 4096U), 1, _data_type, _fixed_point_position));
            }
        }
    }

    void build()
    {
        input.allocator()->init(TensorInfo(TensorShape(227U, 227U, 3U, _batches), 1, _data_type, _fixed_point_position));
        output.allocator()->init(TensorInfo(TensorShape(1000U, _batches), 1, _data_type, _fixed_point_position));

        // Initialize intermediate tensors
        // Layer 1
        conv1_out.allocator()->init(TensorInfo(TensorShape(55U, 55U, 96U, _batches), 1, _data_type, _fixed_point_position));
        act1_out.allocator()->init(TensorInfo(TensorShape(55U, 55U, 96U, _batches), 1, _data_type, _fixed_point_position));
        norm1_out.allocator()->init(TensorInfo(TensorShape(55U, 55U, 96U, _batches), 1, _data_type, _fixed_point_position));
        pool1_out.allocator()->init(TensorInfo(TensorShape(27U, 27U, 96U, _batches), 1, _data_type, _fixed_point_position));
        pool11_out = std::unique_ptr<SubTensorType>(new SubTensorType(&pool1_out, TensorShape(27U, 27U, 48U, _batches), Coordinates()));
        pool12_out = std::unique_ptr<SubTensorType>(new SubTensorType(&pool1_out, TensorShape(27U, 27U, 48U, _batches), Coordinates(0, 0, 48)));
        // Layer 2
        conv2_out.allocator()->init(TensorInfo(TensorShape(27U, 27U, 256U, _batches), 1, _data_type, _fixed_point_position));
        conv21_out = std::unique_ptr<SubTensorType>(new SubTensorType(&conv2_out, TensorShape(27U, 27U, 128U, _batches), Coordinates()));
        conv22_out = std::unique_ptr<SubTensorType>(new SubTensorType(&conv2_out, TensorShape(27U, 27U, 128U, _batches), Coordinates(0, 0, 128)));
        act2_out.allocator()->init(TensorInfo(TensorShape(27U, 27U, 256U, _batches), 1, _data_type, _fixed_point_position));
        norm2_out.allocator()->init(TensorInfo(TensorShape(27U, 27U, 256U, _batches), 1, _data_type, _fixed_point_position));
        pool2_out.allocator()->init(TensorInfo(TensorShape(13U, 13U, 256U, _batches), 1, _data_type, _fixed_point_position));
        // Layer 3
        conv3_out.allocator()->init(TensorInfo(TensorShape(13U, 13U, 384U, _batches), 1, _data_type, _fixed_point_position));
        act3_out.allocator()->init(TensorInfo(TensorShape(13U, 13U, 384U, _batches), 1, _data_type, _fixed_point_position));
        act31_out = std::unique_ptr<SubTensorType>(new SubTensorType(&act3_out, TensorShape(13U, 13U, 192U, _batches), Coordinates()));
        act32_out = std::unique_ptr<SubTensorType>(new SubTensorType(&act3_out, TensorShape(13U, 13U, 192U, _batches), Coordinates(0, 0, 192)));
        // Layer 4
        conv4_out.allocator()->init(TensorInfo(TensorShape(13U, 13U, 384U, _batches), 1, _data_type, _fixed_point_position));
        conv41_out = std::unique_ptr<SubTensorType>(new SubTensorType(&conv4_out, TensorShape(13U, 13U, 192U, _batches), Coordinates()));
        conv42_out = std::unique_ptr<SubTensorType>(new SubTensorType(&conv4_out, TensorShape(13U, 13U, 192U, _batches), Coordinates(0, 0, 192)));
        act4_out.allocator()->init(TensorInfo(TensorShape(13U, 13U, 384U, _batches), 1, _data_type, _fixed_point_position));
        act41_out = std::unique_ptr<SubTensorType>(new SubTensorType(&act4_out, TensorShape(13U, 13U, 192U, _batches), Coordinates()));
        act42_out = std::unique_ptr<SubTensorType>(new SubTensorType(&act4_out, TensorShape(13U, 13U, 192U, _batches), Coordinates(0, 0, 192)));
        // Layer 5
        conv5_out.allocator()->init(TensorInfo(TensorShape(13U, 13U, 256U, _batches), 1, _data_type, _fixed_point_position));
        conv51_out = std::unique_ptr<SubTensorType>(new SubTensorType(&conv5_out, TensorShape(13U, 13U, 128U, _batches), Coordinates()));
        conv52_out = std::unique_ptr<SubTensorType>(new SubTensorType(&conv5_out, TensorShape(13U, 13U, 128U, _batches), Coordinates(0, 0, 128)));
        act5_out.allocator()->init(TensorInfo(TensorShape(13U, 13U, 256U, _batches), 1, _data_type, _fixed_point_position));
        pool5_out.allocator()->init(TensorInfo(TensorShape(6U, 6U, 256U, _batches), 1, _data_type, _fixed_point_position));
        // Layer 6
        fc6_out.allocator()->init(TensorInfo(TensorShape(4096U, _batches), 1, _data_type, _fixed_point_position));
        act6_out.allocator()->init(TensorInfo(TensorShape(4096U, _batches), 1, _data_type, _fixed_point_position));
        // Layer 7
        fc7_out.allocator()->init(TensorInfo(TensorShape(4096U, _batches), 1, _data_type, _fixed_point_position));
        act7_out.allocator()->init(TensorInfo(TensorShape(4096U, _batches), 1, _data_type, _fixed_point_position));
        // Layer 8
        fc8_out.allocator()->init(TensorInfo(TensorShape(1000U, _batches), 1, _data_type, _fixed_point_position));

        // Configure Layers
        // Layer 1
        TensorType *b0 = _reshaped_weights ? nullptr : &b[0];
        conv1.configure(&input, &w[0], b0, &conv1_out, PadStrideInfo(4, 4, 0, 0), WeightsInfo(_reshaped_weights, 11U, 11U, 96U));
        act1.configure(&conv1_out, &act1_out, ActivationLayerInfo(ActivationLayerInfo::ActivationFunction::RELU));
        norm1.configure(&act1_out, &norm1_out, NormalizationLayerInfo(NormType::CROSS_MAP, 5, 0.0001f, 0.75f));
        pool1.configure(&norm1_out, &pool1_out, PoolingLayerInfo(PoolingType::MAX, 3, PadStrideInfo(2, 2, 0, 0)));
        // Layer 2
        conv21.configure(pool11_out.get(), w21.get(), b21.get(), conv21_out.get(), PadStrideInfo(1, 1, 2, 2), WeightsInfo(_reshaped_weights, 5U, 5U, 128U));
        conv22.configure(pool12_out.get(), w22.get(), b22.get(), conv22_out.get(), PadStrideInfo(1, 1, 2, 2), WeightsInfo(_reshaped_weights, 5U, 5U, 128U));
        act2.configure(&conv2_out, &act2_out, ActivationLayerInfo(ActivationLayerInfo::ActivationFunction::RELU));
        norm2.configure(&act2_out, &norm2_out, NormalizationLayerInfo(NormType::CROSS_MAP, 5, 0.0001f, 0.75f));
        pool2.configure(&norm2_out, &pool2_out, PoolingLayerInfo(PoolingType::MAX, 3, PadStrideInfo(2, 2, 0, 0)));
        // Layer 3
        TensorType *b2 = (_reshaped_weights && !_is_direct_conv) ? nullptr : &b[2];
        conv3.configure(&pool2_out, &w[2], b2, &conv3_out, PadStrideInfo(1, 1, 1, 1), WeightsInfo(_reshaped_weights, 3U, 3U, 384U));
        act3.configure(&conv3_out, &act3_out, ActivationLayerInfo(ActivationLayerInfo::ActivationFunction::RELU));
        // Layer 4
        conv41.configure(act31_out.get(), w41.get(), b41.get(), conv41_out.get(), PadStrideInfo(1, 1, 1, 1), WeightsInfo(_reshaped_weights, 3U, 3U, 192U));
        conv42.configure(act32_out.get(), w42.get(), b42.get(), conv42_out.get(), PadStrideInfo(1, 1, 1, 1), WeightsInfo(_reshaped_weights, 3U, 3U, 192U));
        act4.configure(&conv4_out, &act4_out, ActivationLayerInfo(ActivationLayerInfo::ActivationFunction::RELU));
        // Layer 5
        conv51.configure(act41_out.get(), w51.get(), b51.get(), conv51_out.get(), PadStrideInfo(1, 1, 1, 1), WeightsInfo(_reshaped_weights, 3U, 3U, 128U));
        conv52.configure(act42_out.get(), w52.get(), b52.get(), conv52_out.get(), PadStrideInfo(1, 1, 1, 1), WeightsInfo(_reshaped_weights, 3U, 3U, 128U));
        act5.configure(&conv5_out, &act5_out, ActivationLayerInfo(ActivationLayerInfo::ActivationFunction::RELU));
        pool5.configure(&act5_out, &pool5_out, PoolingLayerInfo(PoolingType::MAX, 3, PadStrideInfo(2, 2, 0, 0)));
        // Layer 6
        fc6.configure(&pool5_out, &w[5], &b[5], &fc6_out, true, _reshaped_weights);
        act6.configure(&fc6_out, &act6_out, ActivationLayerInfo(ActivationLayerInfo::ActivationFunction::RELU));
        // Layer 7
        fc7.configure(&act6_out, &w[6], &b[6], &fc7_out, true, _reshaped_weights);
        act7.configure(&fc7_out, &act7_out, ActivationLayerInfo(ActivationLayerInfo::ActivationFunction::RELU));
        // Layer 8
        fc8.configure(&act7_out, &w[7], &b[7], &fc8_out, true, _reshaped_weights);
        // Softmax
        smx.configure(&fc8_out, &output);
    }

    void allocate()
    {
        input.allocator()->allocate();
        output.allocator()->allocate();

        if(!_reshaped_weights)
        {
            for(auto &wi : w)
            {
                wi.allocator()->allocate();
            }

            for(auto &bi : b)
            {
                bi.allocator()->allocate();
            }
        }
        else
        {
            w[0].allocator()->allocate();
            w[2].allocator()->allocate();
            w[5].allocator()->allocate();
            w[6].allocator()->allocate();
            w[7].allocator()->allocate();

            b[5].allocator()->allocate();
            b[6].allocator()->allocate();
            b[7].allocator()->allocate();

            dynamic_cast<TensorType *>(w21.get())->allocator()->allocate();
            dynamic_cast<TensorType *>(w22.get())->allocator()->allocate();
            if(!_is_direct_conv)
            {
                dynamic_cast<TensorType *>(w41.get())->allocator()->allocate();
                dynamic_cast<TensorType *>(w42.get())->allocator()->allocate();
                dynamic_cast<TensorType *>(w51.get())->allocator()->allocate();
                dynamic_cast<TensorType *>(w52.get())->allocator()->allocate();
            }
            else
            {
                b[2].allocator()->allocate();
                b[3].allocator()->allocate();
                b[4].allocator()->allocate();
                w[3].allocator()->allocate();
                w[4].allocator()->allocate();
            }
        }

        conv1_out.allocator()->allocate();
        act1_out.allocator()->allocate();
        norm1_out.allocator()->allocate();
        pool1_out.allocator()->allocate();
        conv2_out.allocator()->allocate();
        act2_out.allocator()->allocate();
        norm2_out.allocator()->allocate();
        pool2_out.allocator()->allocate();
        conv3_out.allocator()->allocate();
        act3_out.allocator()->allocate();
        conv4_out.allocator()->allocate();
        act4_out.allocator()->allocate();
        conv5_out.allocator()->allocate();
        act5_out.allocator()->allocate();
        pool5_out.allocator()->allocate();
        fc6_out.allocator()->allocate();
        act6_out.allocator()->allocate();
        fc7_out.allocator()->allocate();
        act7_out.allocator()->allocate();
        fc8_out.allocator()->allocate();
    }

    /** Fills the trainable parameters and input with random data. */
    void fill_random()
    {
        library->fill_tensor_uniform(Accessor(input), 0);

        if(!_reshaped_weights)
        {
            for(unsigned int i = 0; i < w.size(); ++i)
            {
                library->fill_tensor_uniform(Accessor(w[i]), i + 1);
                library->fill_tensor_uniform(Accessor(b[i]), i + 10);
            }
        }
        else
        {
            library->fill_tensor_uniform(Accessor(w[0]), 1);
            library->fill_tensor_uniform(Accessor(w[2]), 2);

            library->fill_tensor_uniform(Accessor(w[5]), 3);
            library->fill_tensor_uniform(Accessor(b[5]), 4);
            library->fill_tensor_uniform(Accessor(w[6]), 5);
            library->fill_tensor_uniform(Accessor(b[6]), 6);
            library->fill_tensor_uniform(Accessor(w[7]), 7);
            library->fill_tensor_uniform(Accessor(b[7]), 8);

            library->fill_tensor_uniform(Accessor(*dynamic_cast<TensorType *>(w21.get())), 9);
            library->fill_tensor_uniform(Accessor(*dynamic_cast<TensorType *>(w22.get())), 10);

            if(!_is_direct_conv)
            {
                library->fill_tensor_uniform(Accessor(*dynamic_cast<TensorType *>(w41.get())), 11);
                library->fill_tensor_uniform(Accessor(*dynamic_cast<TensorType *>(w42.get())), 12);
                library->fill_tensor_uniform(Accessor(*dynamic_cast<TensorType *>(w51.get())), 13);
                library->fill_tensor_uniform(Accessor(*dynamic_cast<TensorType *>(w52.get())), 14);
            }
            else
            {
                library->fill_tensor_uniform(Accessor(w[3]), 11);
                library->fill_tensor_uniform(Accessor(b[3]), 12);
                library->fill_tensor_uniform(Accessor(w[4]), 13);
                library->fill_tensor_uniform(Accessor(b[4]), 14);
            }
        }
    }

#ifdef INTERNAL_ONLY
    /** Fills the trainable parameters from binary files
     *
     * @param weights Files names containing the weights data
     * @param biases  Files names containing the bias data
     */
    void fill(std::vector<std::string> weights, std::vector<std::string> biases)
    {
        ARM_COMPUTE_ERROR_ON(weights.size() != w.size());
        ARM_COMPUTE_ERROR_ON(biases.size() != b.size());
        ARM_COMPUTE_ERROR_ON(_reshaped_weights);

        for(unsigned int i = 0; i < weights.size(); ++i)
        {
            library->fill_layer_data(Accessor(w[i]), weights[i]);
            library->fill_layer_data(Accessor(b[i]), biases[i]);
        }
    }

    /** Feed input to network from file.
     *
     * @param name File name of containing the input data.
     */
    void feed(std::string name)
    {
        library->fill_layer_data(Accessor(input), name);
    }
#endif /* INTERNAL_ONLY */

    /** Get the classification results.
     *
     * @return Vector containing the classified labels
     */
    std::vector<unsigned int> get_classifications()
    {
        std::vector<unsigned int> classified_labels;
        Accessor                  output_accessor(output);

        Window window;
        window.set(Window::DimX, Window::Dimension(0, 1, 1));
        for(unsigned int d = 1; d < output_accessor.shape().num_dimensions(); ++d)
        {
            window.set(d, Window::Dimension(0, output_accessor.shape()[d], 1));
        }

        execute_window_loop(window, [&](const Coordinates & id)
        {
            int               max_idx = 0;
            float             val     = 0;
            const void *const out_ptr = output_accessor(id);
            for(unsigned int l = 0; l < output_accessor.shape().x(); ++l)
            {
                float curr_val = reinterpret_cast<const float *>(out_ptr)[l];
                if(curr_val > val)
                {
                    max_idx = l;
                    val     = curr_val;
                }
            }
            classified_labels.push_back(max_idx);
        });
        return classified_labels;
    }

    /** Clear all allocated memory from the tensor objects */
    void clear()
    {
        // Free allocations
        input.allocator()->free();
        output.allocator()->free();

        if(!_reshaped_weights)
        {
            for(auto &wi : w)
            {
                wi.allocator()->free();
            }

            for(auto &bi : b)
            {
                bi.allocator()->free();
            }
        }
        else
        {
            w[0].allocator()->free();
            w[2].allocator()->free();
            w[5].allocator()->free();
            w[6].allocator()->free();
            w[7].allocator()->free();

            b[5].allocator()->free();
            b[6].allocator()->free();
            b[7].allocator()->free();

            if(_is_direct_conv)
            {
                w[3].allocator()->free();
                w[4].allocator()->free();
                b[2].allocator()->free();
                b[3].allocator()->free();
                b[4].allocator()->free();
            }
        }

        w21.reset();
        w22.reset();
        b21.reset();
        b21.reset();
        w41.reset();
        w42.reset();
        b41.reset();
        b42.reset();
        w51.reset();
        w52.reset();
        b51.reset();
        b52.reset();

        conv1_out.allocator()->free();
        act1_out.allocator()->free();
        norm1_out.allocator()->free();
        pool1_out.allocator()->free();
        conv2_out.allocator()->free();
        act2_out.allocator()->free();
        norm2_out.allocator()->free();
        pool2_out.allocator()->free();
        conv3_out.allocator()->free();
        act3_out.allocator()->free();
        conv4_out.allocator()->free();
        act4_out.allocator()->free();
        conv5_out.allocator()->free();
        act5_out.allocator()->free();
        pool5_out.allocator()->free();
        fc6_out.allocator()->free();
        act6_out.allocator()->free();
        fc7_out.allocator()->free();
        act7_out.allocator()->free();
        fc8_out.allocator()->free();
    }

    /** Runs the model */
    void run()
    {
        // Layer 1
        conv1.run();
        act1.run();
        norm1.run();
        pool1.run();
        // Layer 2
        conv21.run();
        conv22.run();
        act2.run();
        norm2.run();
        pool2.run();
        // Layer 3
        conv3.run();
        act3.run();
        // Layer 4
        conv41.run();
        conv42.run();
        act4.run();
        // Layer 5
        conv51.run();
        conv52.run();
        act5.run();
        pool5.run();
        // Layer 6
        fc6.run();
        act6.run();
        // Layer 7
        fc7.run();
        act7.run();
        // Layer 8
        fc8.run();
        // Softmax
        smx.run();
    }

private:
    struct DirectConv
    {
        template <typename ConvolutionLayerFunction1 = ConvolutionLayerFunction, typename DirectConvolutionLayerFunction1 = DirectConvolutionLayerFunction>
        typename std::enable_if < !std::is_same<ConvolutionLayerFunction1, DirectConvolutionLayerFunction1>::value, void >::type
        configure(ITensorType *input, const ITensorType *weights, const ITensorType *biases, ITensorType *output, const PadStrideInfo &conv_info, const WeightsInfo &weights_info = WeightsInfo())
        {
            _func.configure(input, weights, biases, output, conv_info);
        }

        template <typename ConvolutionLayerFunction1 = ConvolutionLayerFunction, typename DirectConvolutionLayerFunction1 = DirectConvolutionLayerFunction>
        typename std::enable_if<std::is_same<ConvolutionLayerFunction1, DirectConvolutionLayerFunction1>::value, void>::type
        configure(ITensorType *input, const ITensorType *weights, const ITensorType *biases, ITensorType *output, const PadStrideInfo &conv_info, const WeightsInfo &weights_info = WeightsInfo())
        {
            _func.configure(input, weights, biases, output, conv_info, weights_info);
        }

        void run()
        {
            _func.run();
        }

        DirectConvolutionLayerFunction _func{};
    };

    DataType     _data_type{ DataType::UNKNOWN };
    int          _fixed_point_position{ 0 };
    unsigned int _batches{ 0 };
    bool         _reshaped_weights{ false };
    bool         _is_direct_conv{ !std::is_same<ConvolutionLayerFunction, DirectConvolutionLayerFunction>::value };

    ActivationLayerFunction     act1{}, act2{}, act3{}, act4{}, act5{}, act6{}, act7{};
    ConvolutionLayerFunction    conv1{}, conv21{}, conv22{};
    DirectConv                  conv3{}, conv41{}, conv42{}, conv51{}, conv52{};
    FullyConnectedLayerFunction fc6{}, fc7{}, fc8{};
    NormalizationLayerFunction  norm1{}, norm2{};
    PoolingLayerFunction        pool1{}, pool2{}, pool5{};
    SoftmaxLayerFunction        smx{};

    TensorType input{}, output{};
    std::array<TensorType, 8> w{ {} }, b{ {} };
    std::unique_ptr<ITensorType> w21{ nullptr }, w22{ nullptr }, b21{ nullptr }, b22{ nullptr };
    std::unique_ptr<ITensorType> w41{ nullptr }, w42{ nullptr }, b41{ nullptr }, b42{ nullptr };
    std::unique_ptr<ITensorType> w51{ nullptr }, w52{ nullptr }, b51{ nullptr }, b52{ nullptr };

    TensorType conv1_out{}, act1_out{}, norm1_out{}, pool1_out{};
    TensorType conv2_out{}, act2_out{}, pool2_out{}, norm2_out{};
    TensorType conv3_out{}, act3_out{};
    TensorType conv4_out{}, act4_out{};
    TensorType conv5_out{}, act5_out{}, pool5_out{};
    TensorType fc6_out{}, act6_out{};
    TensorType fc7_out{}, act7_out{};
    TensorType fc8_out{};

    std::unique_ptr<SubTensorType> pool11_out{}, pool12_out{};
    std::unique_ptr<SubTensorType> conv21_out{}, conv22_out{};
    std::unique_ptr<SubTensorType> act31_out{}, act32_out{};
    std::unique_ptr<SubTensorType> conv41_out{}, conv42_out{}, act41_out{}, act42_out{};
    std::unique_ptr<SubTensorType> conv51_out{}, conv52_out{};
};
} // namespace networks
} // namespace test
} // namespace arm_compute
#endif //__ARM_COMPUTE_TEST_MODEL_OBJECTS_ALEXNET_H__

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
#ifndef __ARM_COMPUTE_TEST_BENCHMARK_CONVOLUTION_LAYER_H__
#define __ARM_COMPUTE_TEST_BENCHMARK_CONVOLUTION_LAYER_H__

#include "TensorLibrary.h"
#include "Utils.h"
#include "dataset/ConvolutionLayerDataset.h"

#include <memory>

using namespace arm_compute;
using namespace arm_compute::test;
using namespace arm_compute::test::benchmark;

namespace arm_compute
{
namespace test
{
namespace benchmark
{
template <typename DataSet, typename TensorType, typename Accessor, typename Function, DataType dt = DataType::F32>
class ConvolutionLayer : public ::benchmark::Fixture
{
public:
    void SetUp(::benchmark::State &state) override
    {
        profiler.add(std::make_shared<WallClockTimer>());

        const ConvolutionLayerDataObject conv_obj = *(DataSet().begin() + state.range(0));

        // Set batched in source and destination shapes
        const unsigned int batches              = state.range(1);
        const unsigned int fixed_point_position = 4;
        TensorShape        src_shape            = conv_obj.src_shape;
        TensorShape        dst_shape            = conv_obj.dst_shape;
        src_shape.set(3 /* batch */, batches);
        dst_shape.set(3 /* batch */, batches);

        // Create tensors
        src     = create_tensor<TensorType>(src_shape, dt, 1, fixed_point_position);
        weights = create_tensor<TensorType>(conv_obj.weights_shape, dt, 1, fixed_point_position);
        bias    = create_tensor<TensorType>(conv_obj.bias_shape, dt, 1, fixed_point_position);
        dst     = create_tensor<TensorType>(dst_shape, dt, 1, fixed_point_position);

        // Create and configure function
        conv_layer = std::unique_ptr<Function>(new Function());
        conv_layer->configure(&src, &weights, &bias, &dst, conv_obj.info);

        // Allocate tensors
        src.allocator()->allocate();
        weights.allocator()->allocate();
        bias.allocator()->allocate();
        dst.allocator()->allocate();

        // Fill tensors
        library->fill_tensor_uniform(Accessor(src), 0);
        library->fill_tensor_uniform(Accessor(weights), 1);
        library->fill_tensor_uniform(Accessor(bias), 2);
    }

    void TearDown(::benchmark::State &state) override
    {
        conv_layer.reset();

        src.allocator()->free();
        weights.allocator()->free();
        bias.allocator()->free();
        dst.allocator()->free();

        profiler.submit(state);
    }

    std::unique_ptr<Function> conv_layer{ nullptr };
    Profiler                  profiler{};

private:
    TensorType src{};
    TensorType weights{};
    TensorType bias{};
    TensorType dst{};
};
} // namespace benchmark
} // namespace test
} // namespace arm_compute
#endif //__ARM_COMPUTE_TEST_BENCHMARK_CONVOLUTION_LAYER_H__

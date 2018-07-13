/*
 * Copyright (c) 2017-2018 ARM Limited.
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
#include "arm_compute/graph.h"
#include "support/ToolchainSupport.h"
#include "utils/CommonGraphOptions.h"
#include "utils/GraphUtils.h"
#include "utils/Utils.h"

using namespace arm_compute::utils;
using namespace arm_compute::graph::frontend;
using namespace arm_compute::graph_utils;

/** Example demonstrating how to implement LeNet's network using the Compute Library's graph API
 *
 * @param[in] argc Number of arguments
 * @param[in] argv Arguments ( [optional] Target (0 = NEON, 1 = OpenCL), [optional] Path to the weights folder, [optional] batches, [optional] Fast math for convolution layer (0 = DISABLED, 1 = ENABLED) )
 */
class GraphLenetExample : public Example
{
public:
    GraphLenetExample()
        : cmd_parser(), common_opts(cmd_parser), common_params(), graph(0, "LeNet")
    {
    }
    bool do_setup(int argc, char **argv) override
    {
        // Parse arguments
        cmd_parser.parse(argc, argv);

        // Consume common parameters
        common_params = consume_common_graph_parameters(common_opts);

        // Return when help menu is requested
        if(common_params.help)
        {
            cmd_parser.print_help(argv[0]);
            return false;
        }

        // Checks
        ARM_COMPUTE_EXIT_ON_MSG(arm_compute::is_data_type_quantized_asymmetric(common_params.data_type), "Unsupported data type!");
        ARM_COMPUTE_EXIT_ON_MSG(common_params.data_layout == DataLayout::NHWC, "Unsupported data layout!");

        // Print parameter values
        std::cout << common_params << std::endl;

        // Get trainable parameters data path
        std::string  data_path = common_params.data_path;
        unsigned int batches   = 4; /** Number of batches */

        //conv1 << pool1 << conv2 << pool2 << fc1 << act1 << fc2 << smx
        graph << common_params.target
              << common_params.fast_math_hint
              << InputLayer(TensorDescriptor(TensorShape(28U, 28U, 1U, batches), common_params.data_type), get_input_accessor(common_params))
              << ConvolutionLayer(
                  5U, 5U, 20U,
                  get_weights_accessor(data_path, "/cnn_data/lenet_model/conv1_w.npy"),
                  get_weights_accessor(data_path, "/cnn_data/lenet_model/conv1_b.npy"),
                  PadStrideInfo(1, 1, 0, 0))
              .set_name("conv1")
              << PoolingLayer(PoolingLayerInfo(PoolingType::MAX, 2, PadStrideInfo(2, 2, 0, 0))).set_name("pool1")
              << ConvolutionLayer(
                  5U, 5U, 50U,
                  get_weights_accessor(data_path, "/cnn_data/lenet_model/conv2_w.npy"),
                  get_weights_accessor(data_path, "/cnn_data/lenet_model/conv2_b.npy"),
                  PadStrideInfo(1, 1, 0, 0))
              .set_name("conv2")
              << PoolingLayer(PoolingLayerInfo(PoolingType::MAX, 2, PadStrideInfo(2, 2, 0, 0))).set_name("pool2")
              << FullyConnectedLayer(
                  500U,
                  get_weights_accessor(data_path, "/cnn_data/lenet_model/ip1_w.npy"),
                  get_weights_accessor(data_path, "/cnn_data/lenet_model/ip1_b.npy"))
              .set_name("ip1")
              << ActivationLayer(ActivationLayerInfo(ActivationLayerInfo::ActivationFunction::RELU)).set_name("relu")
              << FullyConnectedLayer(
                  10U,
                  get_weights_accessor(data_path, "/cnn_data/lenet_model/ip2_w.npy"),
                  get_weights_accessor(data_path, "/cnn_data/lenet_model/ip2_b.npy"))
              .set_name("ip2")
              << SoftmaxLayer().set_name("prob")
              << OutputLayer(get_output_accessor(common_params));

        // Finalize graph
        GraphConfig config;
        config.num_threads = common_params.threads;
        config.use_tuner   = common_params.enable_tuner;
        config.tuner_file  = common_params.tuner_file;

        graph.finalize(common_params.target, config);

        return true;
    }
    void do_run() override
    {
        // Run graph
        graph.run();
    }

private:
    CommandLineParser  cmd_parser;
    CommonGraphOptions common_opts;
    CommonGraphParams  common_params;
    Stream             graph;
};

/** Main program for LeNet
 *
 * @param[in] argc Number of arguments
 * @param[in] argv Arguments ( [optional] Target (0 = NEON, 1 = OpenCL, 2 = OpenCL with Tuner), [optional] Path to the weights folder, [optional] batches, [optional] Fast math for convolution layer (0 = DISABLED, 1 = ENABLED) )
 */
int main(int argc, char **argv)
{
    return arm_compute::utils::run_example<GraphLenetExample>(argc, argv);
}

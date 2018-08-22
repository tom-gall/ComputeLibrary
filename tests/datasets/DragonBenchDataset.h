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
//FIXME / INTERNAL_ONLY: This file should not be released!
#ifndef ARM_COMPUTE_DRAGONBENCH_DATASET
#define ARM_COMPUTE_DRAGONBENCH_DATASET

#include "utils/TypePrinter.h"

#include "arm_compute/core/TensorShape.h"

namespace arm_compute
{
namespace test
{
namespace datasets
{
template <typename ConfigsType, typename ConfigType>
class DragonBenchDataset
{
public:
    using type = std::tuple<ConfigType>;

    DragonBenchDataset(ConfigsType configs)
        : _configs(configs)
    {
    }

    struct iterator
    {
        iterator(typename std::vector<ConfigType>::const_iterator configs_it)
            : _configs_it{ std::move(configs_it) }
        {
        }

        std::string description() const
        {
            std::stringstream description;
            description << "network_name=" << _configs_it->network_name << ":";
            description << "layer_name=" << _configs_it->layer_name << ":";
            description << "id=" << _configs_it->id << ":";
            description << "Input_NCHW="
                        << _configs_it->ibatch << ','
                        << _configs_it->ch_in << ','
                        << _configs_it->dim_in_h << ','
                        << _configs_it->dim_in_w << ":";
            description << "Output_NCHW="
                        << _configs_it->ibatch << ','
                        << _configs_it->ch_out << ','
                        << _configs_it->dim_out_h << ','
                        << _configs_it->dim_out_w << ":";
            description << "Weights_HW="
                        << _configs_it->kern_h << ','
                        << _configs_it->kern_w << ":";
            description << "Stride_HW="
                        << _configs_it->stride_h << ','
                        << _configs_it->stride_w << ":";
            description << "Padding=" << _configs_it->padding << ":";
            return description.str();
        }

        DragonBenchDataset::type operator*() const
        {
            return std::make_tuple(*_configs_it);
        }

        iterator &operator++()
        {
            ++_configs_it;
            return *this;
        }

    private:
        typename std::vector<ConfigType>::const_iterator _configs_it;
    };

    iterator begin() const
    {
        return iterator(_configs.configs.begin());
    }

    int size() const
    {
        return _configs.num_configs;
    }

private:
    ConfigsType _configs;
};
} // namespace datasets
} // namespace test
} // namespace arm_compute
#endif /* ARM_COMPUTE_DRAGONBENCH_DATASET */
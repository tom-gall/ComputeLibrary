/*
 * Copyright (c) 2016, 2017 ARM Limited.
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
#include "arm_compute/core/CL/CLHelpers.h"
#include "arm_compute/core/CL/CLTypes.h"
#include "arm_compute/core/Error.h"
#include "arm_compute/core/Types.h"

#include <map>
#include <vector>

namespace
{
arm_compute::GPUTarget get_bifrost_target(const std::string &name)
{
    arm_compute::GPUTarget target = arm_compute::GPUTarget::MIDGARD;

    if(name == "G7")
    {
        target = arm_compute::GPUTarget::G70;
    }

    return target;
}

arm_compute::GPUTarget get_midgard_target(const std::string &name)
{
    arm_compute::GPUTarget target = arm_compute::GPUTarget::MIDGARD;

    if(name == "T6")
    {
        target = arm_compute::GPUTarget::T600;
    }
    else if(name == "T7")
    {
        target = arm_compute::GPUTarget::T700;
    }
    else if(name == "T8")
    {
        target = arm_compute::GPUTarget::T800;
    }

    return target;
}
} // namespace

namespace arm_compute
{
std::string get_cl_type_from_data_type(const DataType &dt)
{
    switch(dt)
    {
        case DataType::U8:
            return "uchar";
        case DataType::QS8:
            return "qs8";
        case DataType::S8:
            return "char";
        case DataType::U16:
            return "ushort";
        case DataType::S16:
            return "short";
        case DataType::QS16:
            return "qs16";
        case DataType::U32:
            return "uint";
        case DataType::S32:
            return "int";
        case DataType::U64:
            return "ulong";
        case DataType::S64:
            return "long";
        case DataType::F16:
            return "half";
        case DataType::F32:
            return "float";
        default:
            ARM_COMPUTE_ERROR("Unsupported input data type.");
            return "";
    }
}

std::string get_data_size_from_data_type(const DataType &dt)
{
    switch(dt)
    {
        case DataType::U8:
        case DataType::QS8:
        case DataType::S8:
            return "8";
        case DataType::U16:
        case DataType::S16:
        case DataType::QS16:
        case DataType::F16:
            return "16";
        case DataType::U32:
        case DataType::S32:
        case DataType::F32:
            return "32";
        case DataType::U64:
        case DataType::S64:
            return "64";
        default:
            ARM_COMPUTE_ERROR("Unsupported input data type.");
            return "0";
    }
}

std::string get_underlying_cl_type_from_data_type(const DataType &dt)
{
    switch(dt)
    {
        case DataType::QS8:
            return "char";
        case DataType::QS16:
            return "short";
        default:
            return get_cl_type_from_data_type(dt);
    }
}

const std::string &string_from_target(GPUTarget target)
{
    static std::map<GPUTarget, const std::string> gpu_target_map =
    {
        { GPUTarget::MIDGARD, "midgard" },
        { GPUTarget::BIFROST, "bifrost" },
        { GPUTarget::T600, "t600" },
        { GPUTarget::T700, "t700" },
        { GPUTarget::T800, "t800" },
        { GPUTarget::G70, "g70" }
    };

    return gpu_target_map[target];
}

GPUTarget get_target_from_device(cl::Device &device)
{
    const std::string name_mali("Mali-");
    GPUTarget         target{ GPUTarget::MIDGARD };

    size_t            name_size = 0;
    std::vector<char> name;

    // Query device name size
    cl_int err = clGetDeviceInfo(device.get(), CL_DEVICE_NAME, 0, nullptr, &name_size);
    ARM_COMPUTE_ERROR_ON_MSG((err != 0) || (name_size == 0), "clGetDeviceInfo failed to return valid information");
    // Resize vector
    name.resize(name_size);
    // Query device name
    err = clGetDeviceInfo(device.get(), CL_DEVICE_NAME, name_size, name.data(), nullptr);
    ARM_COMPUTE_ERROR_ON_MSG(err != 0, "clGetDeviceInfo failed to return valid information");
    ARM_COMPUTE_UNUSED(err);

    std::string name_str(name.begin(), name.end());
    auto        pos = name_str.find(name_mali);

    if(pos != std::string::npos)
    {
        ARM_COMPUTE_ERROR_ON_MSG((pos + name_mali.size() + 2) > name_str.size(), "Device name is shorter than expected.");
        std::string sub_name = name_str.substr(pos + name_mali.size(), 2);

        if(sub_name[0] == 'G')
        {
            target = get_bifrost_target(sub_name);
        }
        else if(sub_name[0] == 'T')
        {
            target = get_midgard_target(sub_name);
        }
        else
        {
            ARM_COMPUTE_INFO("Mali GPU unknown. Target is set to the default one.");
        }
    }
    else
    {
        ARM_COMPUTE_INFO("Can't find valid Mali GPU. Target is set to the default one.");
    }

    return target;
}

GPUTarget get_arch_from_target(GPUTarget target)
{
    return (target & GPUTarget::GPU_ARCH_MASK);
}

bool non_uniform_workgroup_support(const cl::Device &device)
{
    std::vector<char> extension;
    size_t            extension_size = 0;
    cl_int            err            = clGetDeviceInfo(device.get(), CL_DEVICE_EXTENSIONS, 0, nullptr, &extension_size);
    ARM_COMPUTE_ERROR_ON_MSG((err != 0) || (extension_size == 0), "clGetDeviceInfo failed to return valid information");
    // Resize vector
    extension.resize(extension_size);
    // Query extension
    err = clGetDeviceInfo(device.get(), CL_DEVICE_EXTENSIONS, extension_size, extension.data(), nullptr);
    ARM_COMPUTE_ERROR_ON_MSG(err != 0, "clGetDeviceInfo failed to return valid information");
    ARM_COMPUTE_UNUSED(err);

    std::string extension_str(extension.begin(), extension.end());
    auto        pos = extension_str.find("cl_arm_non_uniform_work_group_size");
    return (pos != std::string::npos);
}

CLVersion get_cl_version(const cl::Device &device)
{
    std::vector<char> version;
    size_t            version_size = 0;
    cl_int            err          = clGetDeviceInfo(device.get(), CL_DEVICE_VERSION, 0, nullptr, &version_size);
    ARM_COMPUTE_ERROR_ON_MSG((err != 0) || (version_size == 0), "clGetDeviceInfo failed to return valid information");
    // Resize vector
    version.resize(version_size);
    // Query version
    err = clGetDeviceInfo(device.get(), CL_DEVICE_VERSION, version_size, version.data(), nullptr);
    ARM_COMPUTE_ERROR_ON_MSG(err != 0, "clGetDeviceInfo failed to return valid information");
    ARM_COMPUTE_UNUSED(err);

    std::string version_str(version.begin(), version.end());
    if(version_str.find("OpenCL 2") != std::string::npos)
    {
        return CLVersion::CL20;
    }
    else if(version_str.find("OpenCL 1.2") != std::string::npos)
    {
        return CLVersion::CL12;
    }
    else if(version_str.find("OpenCL 1.1") != std::string::npos)
    {
        return CLVersion::CL11;
    }
    else if(version_str.find("OpenCL 1.0") != std::string::npos)
    {
        return CLVersion::CL10;
    }

    return CLVersion::UNKNOWN;
}

} // namespace arm_compute

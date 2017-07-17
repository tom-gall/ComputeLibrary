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
#include "framework/DatasetModes.h"
#include "framework/Macros.h"
#include "framework/command_line/CommandLineOptions.h"
#include "framework/command_line/CommandLineParser.h"
#include "framework/instruments/Instruments.h"
#include "framework/printers/Printers.h"
#include "support/ToolchainSupport.h"
#include "tests/TensorLibrary.h"

#ifdef ARM_COMPUTE_CL
#include "arm_compute/runtime/CL/CLScheduler.h"
#endif /* ARM_COMPUTE_CL */
#include "arm_compute/runtime/Scheduler.h"

#include <fstream>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <random>
#include <utility>

using namespace arm_compute;
using namespace arm_compute::test;

namespace arm_compute
{
namespace test
{
std::unique_ptr<TensorLibrary> library;
} // namespace test
} // namespace arm_compute

int main(int argc, char **argv)
{
#ifdef ARM_COMPUTE_CL
    CLScheduler::get().default_init();
#endif /* ARM_COMPUTE_CL */

    framework::Framework &framework = framework::Framework::get();

    framework::CommandLineParser parser;

    std::set<framework::InstrumentType> allowed_instruments
    {
        framework::InstrumentType::ALL,
        framework::InstrumentType::NONE,
    };

    for(const auto &type : framework.available_instruments())
    {
        allowed_instruments.insert(type);
    }

    std::set<framework::DatasetMode> allowed_modes
    {
        framework::DatasetMode::PRECOMMIT,
        framework::DatasetMode::NIGHTLY,
        framework::DatasetMode::ALL
    };

    std::set<framework::LogFormat> supported_log_formats
    {
        framework::LogFormat::NONE,
        framework::LogFormat::PRETTY,
        framework::LogFormat::JSON,
    };

    auto help = parser.add_option<framework::ToggleOption>("help");
    help->set_help("Show this help message");
    auto dataset_mode = parser.add_option<framework::EnumOption<framework::DatasetMode>>("mode", allowed_modes, framework::DatasetMode::ALL);
    dataset_mode->set_help("For managed datasets select which group to use");
    auto instruments = parser.add_option<framework::EnumListOption<framework::InstrumentType>>("instruments", allowed_instruments, std::initializer_list<framework::InstrumentType> { framework::InstrumentType::ALL });
    instruments->set_help("Set the profiling instruments to use");
    auto iterations = parser.add_option<framework::SimpleOption<int>>("iterations", 1);
    iterations->set_help("Number of iterations per test case");
    auto threads = parser.add_option<framework::SimpleOption<int>>("threads", 1);
    threads->set_help("Number of threads to use");
    auto log_format = parser.add_option<framework::EnumOption<framework::LogFormat>>("log-format", supported_log_formats, framework::LogFormat::PRETTY);
    log_format->set_help("Output format for measurements and failures");
    auto filter = parser.add_option<framework::SimpleOption<std::string>>("filter", ".*");
    filter->set_help("Regular expression to select test cases");
    auto filter_id = parser.add_option<framework::SimpleOption<std::string>>("filter-id", ".*");
    filter_id->set_help("Regular expression to select test cases by id");
    auto log_file = parser.add_option<framework::SimpleOption<std::string>>("log-file");
    log_file->set_help("Write output to file instead of to the console");
    auto throw_errors = parser.add_option<framework::ToggleOption>("throw-errors");
    throw_errors->set_help("Don't catch errors (useful for debugging)");
    auto seed = parser.add_option<framework::SimpleOption<std::random_device::result_type>>("seed", std::random_device()());
    seed->set_help("Global seed for random number generation");
    auto color_output = parser.add_option<framework::ToggleOption>("color-output", true);
    color_output->set_help("Produce colored output on the console");
    auto list_tests = parser.add_option<framework::ToggleOption>("list-tests", false);
    list_tests->set_help("List all test names");
    auto assets = parser.add_positional_option<framework::SimpleOption<std::string>>("assets");
    assets->set_help("Path to the assets directory");
    assets->set_required(true);

    try
    {
        parser.parse(argc, argv);

        if(help->is_set() && help->value())
        {
            parser.print_help(argv[0]);
            return 0;
        }

        if(!parser.validate())
        {
            return 1;
        }

        std::unique_ptr<framework::Printer> printer;
        std::ofstream                       log_stream;

        switch(log_format->value())
        {
            case framework::LogFormat::JSON:
                printer = support::cpp14::make_unique<framework::JSONPrinter>();
                break;
            case framework::LogFormat::NONE:
                break;
            case framework::LogFormat::PRETTY:
            default:
            {
                auto pretty_printer = support::cpp14::make_unique<framework::PrettyPrinter>();
                pretty_printer->set_color_output(color_output->value());
                printer = std::move(pretty_printer);
                break;
            }
        }

        if(printer != nullptr)
        {
            if(log_file->is_set())
            {
                log_stream.open(log_file->value());
                printer->set_stream(log_stream);
            }
        }

        library = support::cpp14::make_unique<TensorLibrary>(assets->value(), seed->value());
        Scheduler::get().set_num_threads(threads->value());

        printer->print_global_header();
        printer->print_entry("Seed", support::cpp11::to_string(seed->value()));
        printer->print_entry("Iterations", support::cpp11::to_string(iterations->value()));
        printer->print_entry("Threads", support::cpp11::to_string(threads->value()));
        {
            using support::cpp11::to_string;
            printer->print_entry("Dataset mode", to_string(dataset_mode->value()));
        }

        framework.init(instruments->value(), iterations->value(), dataset_mode->value(), filter->value(), filter_id->value());
        framework.set_printer(printer.get());
        framework.set_throw_errors(throw_errors->value());

        bool success = true;

        if(list_tests->value())
        {
            for(const auto &id : framework.test_ids())
            {
                std::cout << "[" << std::get<0>(id) << ", " << std::get<2>(id) << "] " << std::get<1>(id) << "\n";
            }
        }
        else
        {
            success = framework.run();
        }

        printer->print_global_footer();

        return (success ? 0 : 1);
    }
    catch(const std::exception &error)
    {
        std::cerr << error.what() << "\n";

        if(throw_errors->value())
        {
            throw;
        }

        return 1;
    }

    return 0;
}

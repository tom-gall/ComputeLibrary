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
#ifndef __ARM_COMPUTE_GRAPH2_GRAPH_CONTEXT_H__
#define __ARM_COMPUTE_GRAPH2_GRAPH_CONTEXT_H__

#include "arm_compute/graph2/Types.h"

#include "arm_compute/runtime/IMemoryManager.h"

#include <map>
#include <memory>

namespace arm_compute
{
namespace graph2
{
/** Contains structs required for memory management */
struct MemoryManagerContext
{
    Target                                       target = { Target::UNSPECIFIED }; /**< Target */
    std::shared_ptr<arm_compute::IMemoryManager> mm     = { nullptr };             /**< Memory manager */
};

/** Graph context **/
class GraphContext final
{
public:
    /** Constructor */
    GraphContext();
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    GraphContext(const GraphContext &) = delete;
    /** Default move constructor */
    GraphContext(GraphContext &&) = default;
    /** Prevent instances of this class from being copied (As this class contains pointers) */
    GraphContext &operator=(const GraphContext &) = delete;
    /** Default move assignment operator */
    GraphContext &operator=(GraphContext &&) = default;
    /** Graph configuration accessor
     *
     * @note Every alteration has to be done before graph finalization
     *
     * @return The graph configuration
     */
    const GraphConfig &config() const;
    /** Sets graph configuration
     *
     * @param[in] config Configuration to use
     */
    void set_config(const GraphConfig &config);
    /** Inserts a memory manager context
     *
     * @param[in] memory_ctx Memory manage context
     *
     * @return If the insertion succeeded else false
     */
    bool insert_memory_management_ctx(MemoryManagerContext &&memory_ctx);
    /** Gets a memory manager context for a given target
     *
     * @param[in] target To retrieve the management context
     *
     * @return Management context for the target if exists else nullptr
     */
    MemoryManagerContext *memory_management_ctx(Target target);
    /** Finalizes memory managers in graph context */
    void finalize();

private:
    GraphConfig _config;                                     /**< Graph configuration */
    std::map<Target, MemoryManagerContext> _memory_managers; /**< Memory managers for each target */
};
} // namespace graph2
} // namespace arm_compute
#endif /* __ARM_COMPUTE_GRAPH2_GRAPH_CONTEXT_H__ */

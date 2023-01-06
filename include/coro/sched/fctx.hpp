#ifndef CORO_INCLUDE_CORO_SCHED_FCTX_HPP_
#define CORO_INCLUDE_CORO_SCHED_FCTX_HPP_

// Coro 使用 Boost Context 的 fcontext 实现上下文切换。
// TODO(Vincil Lau): 暂时直接包含 Boost Context 的内部头文件。
#include <boost/context/detail/fcontext.hpp>

namespace coro {
namespace sched {

// 重新导出 fcontext 中的部分类型和函数。
using fcontext_t = boost::context::detail::fcontext_t;
using transfer_t = boost::context::detail::transfer_t;
using boost::context::detail::jump_fcontext;  // NOLINT
using boost::context::detail::make_fcontext;  // NOLINT

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_FCTX_HPP_

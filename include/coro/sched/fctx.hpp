#ifndef CORO_INCLUDE_CORO_SCHED_FCTX_HPP_
#define CORO_INCLUDE_CORO_SCHED_FCTX_HPP_

#include <boost/context/detail/fcontext.hpp>

namespace coro {
namespace sched {

using fcontext_t = boost::context::detail::fcontext_t;
using transfer_t = boost::context::detail::transfer_t;
using boost::context::detail::jump_fcontext;  // NOLINT
using boost::context::detail::make_fcontext;  // NOLINT

}  // namespace sched
}  // namespace coro

#endif  // CORO_INCLUDE_CORO_SCHED_FCTX_HPP_

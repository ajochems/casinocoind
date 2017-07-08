//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

//==============================================================================
/*
    2017-06-29  ajochems        Refactored for casinocoin
*/
//==============================================================================

#ifndef CASINOCOIN_BASICS_RANDOM_H_INCLUDED
#define CASINOCOIN_BASICS_RANDOM_H_INCLUDED

#include <casinocoin/basics/win32_workaround.h>
#include <casinocoin/beast/xor_shift_engine.h>
#include <beast/core/detail/is_call_possible.hpp>
#include <boost/thread/tss.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <random>
#include <limits>
#include <type_traits>

namespace casinocoin {

#ifndef __INTELLISENSE__
static_assert (
    std::is_integral <beast::xor_shift_engine::result_type>::value &&
    std::is_unsigned <beast::xor_shift_engine::result_type>::value,
        "The Casinocoin default PRNG engine must return an unsigned integral type.");

static_assert (
    std::numeric_limits<beast::xor_shift_engine::result_type>::max() >=
    std::numeric_limits<std::uint64_t>::max(),
        "The Casinocoin default PRNG engine return must be at least 64 bits wide.");
#endif

namespace detail {


// Determines if a type can be called like an Engine
template <class Engine, class Result = typename Engine::result_type>
using is_engine =
    beast::detail::is_call_possible<Engine, Result()>;
}

/** Return the default random engine.

    This engine is guaranteed to be deterministic, but by
    default will be randomly seeded. It is NOT cryptographically
    secure and MUST NOT be used to generate randomness that
    will be used for keys, secure cookies, IVs, padding, etc.

    Each thread gets its own instance of the engine which
    will be randomly seeded.
*/
inline
beast::xor_shift_engine&
default_prng ()
{
    static
    boost::thread_specific_ptr<beast::xor_shift_engine> engine;

    if (!engine.get())
    {
        std::random_device rng;

        std::uint64_t seed = rng();

        for (int i = 0; i < 6; ++i)
        {
            if (seed == 0)
                seed = rng();

            seed ^= (seed << (7 - i)) * rng();
        }

        engine.reset (new beast::xor_shift_engine (seed));
    }

    return *engine;
}

/** Return a uniformly distributed random integer.

    @param min The smallest value to return. If not specified
               the value defaults to 0.
    @param max The largest value to return. If not specified
               the value defaults to the largest value that
               can be represented.

    The randomness is generated by the specified engine (or
    the default engine if one is not specified). The result
    is only cryptographicallys secure if the PRNG engine is
    cryptographically secure.

    @note The range is always a closed interval, so calling
          rand_int(-5, 15) can return any integer in the
          closed interval [-5, 15]; similarly, calling
          rand_int(7) can return any integer in the closed
          interval [0, 7].
*/
/** @{ */
template <class Engine, class Integral>
std::enable_if_t<
    std::is_integral<Integral>::value &&
    detail::is_engine<Engine>::value,
Integral>
rand_int (
    Engine& engine,
    Integral min,
    Integral max)
{
    assert (max > min);

    // This should have no state and constructing it should
    // be very cheap. If that turns out not to be the case
    // it could be hand-optimized.
    return std::uniform_int_distribution<Integral>(min, max)(engine);
}

template <class Integral>
std::enable_if_t<std::is_integral<Integral>::value, Integral>
rand_int (
    Integral min,
    Integral max)
{
    return rand_int (default_prng(), min, max);
}

template <class Engine, class Integral>
std::enable_if_t<
    std::is_integral<Integral>::value &&
    detail::is_engine<Engine>::value,
Integral>
rand_int (
    Engine& engine,
    Integral max)
{
    return rand_int (engine, Integral(0), max);
}

template <class Integral>
std::enable_if_t<std::is_integral<Integral>::value, Integral>
rand_int (Integral max)
{
    return rand_int (default_prng(), max);
}

template <class Integral, class Engine>
std::enable_if_t<
    std::is_integral<Integral>::value &&
    detail::is_engine<Engine>::value,
Integral>
rand_int (
    Engine& engine)
{
    return rand_int (
        engine,
        std::numeric_limits<Integral>::max());
}

template <class Integral = int>
std::enable_if_t<std::is_integral<Integral>::value, Integral>
rand_int ()
{
    return rand_int (
        default_prng(),
        std::numeric_limits<Integral>::max());
}
/** @} */

/** Return a random boolean value */
/** @{ */
template <class Engine>
inline
bool
rand_bool (Engine& engine)
{
    return rand_int (engine, 1) == 1;
}

inline
bool
rand_bool ()
{
    return rand_bool (default_prng());
}
/** @} */

} // casinocoin

#endif // CASINOCOIN_BASICS_RANDOM_H_INCLUDED

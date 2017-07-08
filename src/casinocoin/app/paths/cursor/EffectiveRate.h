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

#ifndef CASINOCOIN_APP_PATHS_CURSOR_EFFECTIVERATE_H_INCLUDED
#define CASINOCOIN_APP_PATHS_CURSOR_EFFECTIVERATE_H_INCLUDED

#include <casinocoin/protocol/AccountID.h>
#include <casinocoin/protocol/Issue.h>
#include <casinocoin/protocol/Rate.h>
#include <boost/optional.hpp>

namespace casinocoin {
namespace path {

Rate
effectiveRate(
    Issue const& issue,
    AccountID const& account1,
    AccountID const& account2,
    boost::optional<Rate> const& rate);

} // path
} // casinocoin

#endif

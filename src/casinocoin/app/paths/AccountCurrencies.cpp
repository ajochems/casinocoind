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
    2017-06-27  ajochems        Refactored for casinocoin
*/
//==============================================================================

#include <BeastConfig.h>
#include <casinocoin/app/paths/AccountCurrencies.h>

namespace casinocoin {

hash_set<Currency> accountSourceCurrencies (
    AccountID const& account,
    std::shared_ptr<CasinocoinLineCache> const& lrCache,
    bool includeCSC)
{
    hash_set<Currency> currencies;

    // YYY Only bother if they are above reserve
    if (includeCSC)
        currencies.insert (cscCurrency());

    // List of casinocoin lines.
    auto& casinocoinLines = lrCache->getCasinocoinLines (account);

    for (auto const& item : casinocoinLines)
    {
        auto rspEntry = (CasinocoinState*) item.get ();
        assert (rspEntry);
        if (!rspEntry)
            continue;

        auto& saBalance = rspEntry->getBalance ();

        // Filter out non
        if (saBalance > zero
            // Have IOUs to send.
            || (rspEntry->getLimitPeer ()
                // Peer extends credit.
                && ((-saBalance) < rspEntry->getLimitPeer ()))) // Credit left.
        {
            currencies.insert (saBalance.getCurrency ());
        }
    }

    currencies.erase (badCurrency());
    return currencies;
}

hash_set<Currency> accountDestCurrencies (
    AccountID const& account,
    std::shared_ptr<CasinocoinLineCache> const& lrCache,
    bool includeCSC)
{
    hash_set<Currency> currencies;

    if (includeCSC)
        currencies.insert (cscCurrency());
    // Even if account doesn't exist

    // List of casinocoin lines.
    auto& casinocoinLines = lrCache->getCasinocoinLines (account);

    for (auto const& item : casinocoinLines)
    {
        auto rspEntry = (CasinocoinState*) item.get ();
        assert (rspEntry);
        if (!rspEntry)
            continue;

        auto& saBalance  = rspEntry->getBalance ();

        if (saBalance < rspEntry->getLimit ())                  // Can take more
            currencies.insert (saBalance.getCurrency ());
    }

    currencies.erase (badCurrency());
    return currencies;
}

} // casinocoin

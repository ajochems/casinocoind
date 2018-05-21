//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/casinocoin/casinocoind
    Copyright (c) 2018 CasinoCoin Foundation

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
    2018-05-14  jrojek          Created
*/
//==============================================================================

#ifndef CASINOCOIN_APP_MISC_CRNROUND_H
#define CASINOCOIN_APP_MISC_CRNROUND_H

#include <casinocoin/app/ledger/Ledger.h>
#include <casinocoin/app/misc/Validations.h>
#include <casinocoin/core/ConfigSections.h>
#include <casinocoin/protocol/Protocol.h>

namespace casinocoin {

class CRNRound
{
public:
    virtual ~CRNRound() = default;

    virtual Json::Value getJson (int) = 0;

    /** Returns a Json::objectValue. */
    virtual Json::Value getJson (uint256 const& ) = 0;

    /** Called when a new fully-validated ledger is accepted. */
//    void doValidatedLedger (std::shared_ptr<ReadView const> const& lastValidatedLedger)
//    {
//        if (needValidatedLedger (lastValidatedLedger->seq ()))
//            doValidatedLedger (lastValidatedLedger->seq (),
//                getEnabledAmendments (*lastValidatedLedger));
//    }

    /** Called to determine whether the amendment logic needs to process
        a new validated ledger. (If it could have changed things.)
    */
    virtual bool
    needValidatedLedger (LedgerIndex seq) = 0;

    virtual void
    doValidatedLedger (
        LedgerIndex ledgerSeq,
        std::set <NodeID> const& enabled) = 0;

    // Called by the consensus code when we need to
    // inject pseudo-transactions
    virtual std::map <NodeID, std::uint32_t>
    doVoting (
        NetClock::time_point closeTime,
        std::set <NodeID> const& enabledAmendments,
        majorityAmendments_t const& majorityAmendments,
        ValidationSet const& valSet) = 0;

    // Called by the consensus code when we need to
    // add feature entries to a validation
    virtual std::vector <uint256>
    doValidation (std::set <uint256> const& enabled) = 0;

    // The set of amendments to enable in the genesis ledger
    // This will return all known, non-vetoed amendments.
    // If we ever have two amendments that should not both be
    // enabled at the same time, we should ensure one is vetoed.
    virtual std::vector <uint256>
    getDesired () = 0;

    // The two function below adapt the API callers expect to the
    // internal amendment table API. This allows the amendment
    // table implementation to be independent of the ledger
    // implementation. These APIs will merge when the view code
    // supports a full ledger API

    void
    doValidation (std::shared_ptr <ReadView const> const& lastClosedLedger,
        STObject& baseValidation)
    {
        auto ourAmendments =
            doValidation (getEnabledAmendments(*lastClosedLedger));
        if (! ourAmendments.empty())
            baseValidation.setFieldV256 (sfAmendments,
               STVector256 (sfAmendments, ourAmendments));
    }

    void
    doVoting (
        std::shared_ptr <ReadView const> const& lastClosedLedger,
        ValidationSet const& parentValidations,
        std::shared_ptr<SHAMap> const& initialPosition)
    {
        // Ask implementation what to do
        auto actions = doVoting (
            lastClosedLedger->parentCloseTime(),
            getEnabledAmendments(*lastClosedLedger),
            getMajorityAmendments(*lastClosedLedger),
            parentValidations);

        // Inject appropriate pseudo-transactions
        for (auto const& it : actions)
        {
            STTx amendTx (ttAMENDMENT,
                [&it, seq = lastClosedLedger->seq() + 1](auto& obj)
                {
                    obj.setAccountID (sfAccount, AccountID());
                    obj.setFieldH256 (sfAmendment, it.first);
                    obj.setFieldU32 (sfLedgerSequence, seq);

                    if (it.second != 0)
                        obj.setFieldU32 (sfFlags, it.second);
                });

            Serializer s;
            amendTx.add (s);

            initialPosition->addGiveItem (
                std::make_shared <SHAMapItem> (
                    amendTx.getTransactionID(),
                    s.peekData()),
                true,
                false);
        }
    }

};

std::unique_ptr<AmendmentTable> make_AmendmentTable (
    std::chrono::seconds majorityTime,
    int majorityFraction,
    Section const& supported,
    Section const& enabled,
    Section const& vetoed,
    beast::Journal journal);

}  // casinocoin

#endif // CASINOCOIN_APP_MISC_CRNROUND_H

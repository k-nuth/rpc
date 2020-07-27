// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETSPENTINFO_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETSPENTINFO_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

inline
bool json_in_getspentinfo(nlohmann::json const& json_object, std::string& tx_id, size_t& index)
{
    if (json_object["params"].size() == 0)
        return false;
    try {
        auto temp = json_object["params"][0];
        if (temp.is_object()) {
            tx_id = temp["txid"];
            index = temp["index"];
        }
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;
}

template <typename Blockchain>
bool getspentinfo(nlohmann::json& json_object, int& error, std::string& error_code, std::string const& txid, size_t const& index, Blockchain const& chain)
{
#ifdef KTH_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
#endif

    kth::hash_digest hash;
    if (kth::decode_hash(hash, txid)) {
        kth::domain::chain::output_point point(hash, index);
        boost::latch latch(2);

        chain.fetch_spend(point, [&](const kth::code &ec, kth::domain::chain::input_point input) {
            if (ec == kth::error::success) {
                json_object["txid"] = kth::encode_hash(input.hash());
                json_object["index"] = input.index();
                {
                    boost::latch latch2(2);
                    chain.fetch_transaction(input.hash(), false, witness,
                        [&](const kth::code &ec, kth::transaction_const_ptr tx_ptr, size_t index,
                            size_t height) {
                        if (ec == kth::error::success) {
                            json_object["height"] = height;
                        }
                        latch2.count_down();
                    });
                    latch2.count_down_and_wait();
                }

            }
            else {
                error = kth::RPC_INVALID_PARAMETER;
                error_code = "Unable to get spent info";
            }
            latch.count_down();
        });
        latch.count_down_and_wait();
    }
    else {
        error = kth::RPC_INVALID_PARAMETER;
        error_code = "Invalid transaction hash";
    }

    if (error != 0)
        return false;
    return true;
}

template <typename Blockchain>
nlohmann::json process_getspentinfo(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string tx_id;
    size_t index;
    if ( ! json_in_getspentinfo(json_in, tx_id, index)) //if false return error
    {
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "getspentinfo\n"
            "\nReturns the txid and index where an output is spent.\n"
            "\nArguments:\n"
            "{\n"
            "  \"txid\" (string) The hex string of the txid\n"
            "  \"index\" (number) The start block height\n"
            "}\n"
            "\nResult:\n"
            "{\n"
            "  \"txid\"  (string) The transaction id\n"
            "  \"index\"  (number) The spending input index\n"
            "  ,...\n"
            "}\n";
        return container;
    }

    if (getspentinfo(result, error, error_code, tx_id, index, chain))
    {
        container["result"] = result;
        container["error"];
    }
    else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;

}


} //namespace kth

#endif //KTH_RPC_MESSAGES_BLOCKCHAIN_GETSPENTINFO_HPP_

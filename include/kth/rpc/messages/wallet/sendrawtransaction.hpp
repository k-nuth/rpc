// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_WALLET_SENDRAWTRANSACTION_HPP_
#define KTH_RPC_MESSAGES_WALLET_SENDRAWTRANSACTION_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

inline
bool json_in_sendrawtransaction(nlohmann::json const& json_object, std::string& tx_str, bool & allowhighfees) {
    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;
    try {
        tx_str = json_object["params"][0].get<std::string>();

        if (size == 2) {
            allowhighfees = json_object["params"][1].get<bool>();
        }
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;
}

template <typename Blockchain>
bool sendrawtransaction(nlohmann::json& json_object, int& error, std::string& error_code, std::string const & incoming_hex, bool allowhighfees, bool use_testnet_rules, Blockchain& chain) {
    //TODO: use allowhighfees
    auto const tx = std::make_shared<kd::message::transaction>();
    kth::data_chunk out;
    kth::decode_base16(out, incoming_hex);

    if (domain::entity_from_data(*tx, out, 1)) {
        boost::latch latch(2);
        chain.organize(tx, [&](const kth::code & ec) {
            if (ec) {
                // error = kth::RPC_VERIFY_ERROR;
                error = ec.value();
                error_code = "Failed to submit transaction.";
                json_object;
            } else {
                json_object = kth::encode_hash(tx->hash());
            }
            latch.count_down();
        });
        latch.count_down_and_wait();
    } else {
        error = kth::RPC_DESERIALIZATION_ERROR;
        error_code = "TX decode failed.";
    }

    if (error != 0){
        LOG_WARNING("rpc", "Failed to Submit Transaction [Error code: ", error, "]");
        return false;
    }
    
    return true;
}

template <typename Blockchain>
nlohmann::json process_sendrawtransaction(nlohmann::json const& json_in, Blockchain& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string tx_str;
    bool allowhighfees = false;
    if ( ! json_in_sendrawtransaction(json_in, tx_str, allowhighfees)) //if false return error
    {
        container["result"];
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "sendrawtransaction \"hexstring\" ( allowhighfees )\n"
            "\nSubmits raw transaction (serialized, hex-encoded) to local node "
            "and network.\n"
            "\nAlso see createrawtransaction and signrawtransaction calls.\n"
            "\nArguments:\n"
            "1. \"hexstring\"    (string, required) The hex string of the raw "
            "transaction)\n"
            "2. allowhighfees    (boolean, optional, default=false) Allow high "
            "fees\n"
            "\nResult:\n"
            "\"hex\"             (string) The transaction hash in hex\n";
        return container;
    }

    if (sendrawtransaction(result, error, error_code, tx_str, allowhighfees, use_testnet_rules, chain)) {
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

#endif //KTH_RPC_MESSAGES_WALLET_SENDRAWTRANSACTION_HPP_

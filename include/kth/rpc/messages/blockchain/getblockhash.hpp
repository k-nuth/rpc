// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASH_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASH_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

inline
bool json_in_getblockhash(nlohmann::json const& json_object, size_t& height) {
    if (json_object["params"].size() == 0)
        return false;
    try {
        height = json_object["params"][0].get<size_t>();
    }
    catch (const std::exception & e) {
        return false;
    }

    return true;
}

template <typename Blockchain>
bool getblockhash(nlohmann::json& json_object, int& error, std::string& error_code, const size_t height, Blockchain const& chain)
{
    kth::hash_digest hash;
    if (chain.get_block_hash(hash, height)){
        json_object = kth::encode_hash(hash);
        return true;
    } else return false;
}

template <typename Blockchain>
nlohmann::json process_getblockhash(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    size_t height;
    if ( ! json_in_getblockhash(json_in, height)) //if false return error
    {
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "getblockhash height\n"
            "\nReturns hash of block in best-block-chain at height provided.\n"
            "\nArguments:\n"
            "1. height         (numeric, required) The height index\n"
            "\nResult:\n"
            "\"hash\"         (string) The block hash\n";
        return container;
    }

    if (getblockhash(result, error, error_code, height, chain))
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

#endif //KTH_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASH_HPP_

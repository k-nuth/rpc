// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETDIFFICULTY_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETDIFFICULTY_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {


template <typename Blockchain>
bool getdifficulty(nlohmann::json& json_object, int& error, std::string& error_code, Blockchain const& chain) {
    json_object = std::get<2>(get_last_block_difficulty(chain));
    return true;
}


template <typename Blockchain>
nlohmann::json process_getdifficulty(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    if (getdifficulty(result, error, error_code, chain)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

}

#endif

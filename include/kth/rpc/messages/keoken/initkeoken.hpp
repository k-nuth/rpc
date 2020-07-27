// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_INITKEOKEN_HPP_
#define KTH_RPC_MESSAGES_INITKEOKEN_HPP_

#include <boost/thread/latch.hpp>

#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/json/json.hpp>
#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>

// #include <kth/node/full_node.hpp>

namespace kth {

template <typename KeokenManager>
bool initkeoken(nlohmann::json& json_object, bool use_testnet_rules, KeokenManager& keoken_manager) {
    keoken_manager.initialize_from_blockchain();
    json_object = "Keoken is starting";
    return true;
}

template <typename KeokenManager>
nlohmann::json process_initkeoken(nlohmann::json const& json_in, KeokenManager& keoken_manager, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    if (initkeoken(result, use_testnet_rules, keoken_manager)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_INITKEOKEN_HPP_

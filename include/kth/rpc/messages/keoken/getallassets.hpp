// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_GETALLASSETS_HPP_
#define KTH_RPC_MESSAGES_GETALLASSETS_HPP_

#include <boost/thread/latch.hpp>

#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/json/json.hpp>
#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>

#include <kth/rpc/messages/keoken/keokenutils.hpp>
namespace kth {

template <typename KeokenManager>
bool getallassets(nlohmann::json& json_object, bool use_testnet_rules, KeokenManager const& keoken_manager) {
    auto assets_list = keoken_manager.get_all_asset_addresses();
    size_t i = 0;
    for (auto const& asset : assets_list) {
        json_object[i]["asset_id"] = asset.asset_id;
        json_object[i]["asset_name"] = asset.asset_name;
        json_object[i]["asset_creator"] =  to_network_wallet(use_testnet_rules, asset.asset_creator).encoded();
        json_object[i]["amount"] = asset.amount;
        json_object[i]["asset_owner"] = to_network_wallet(use_testnet_rules, asset.amount_owner).encoded();
        ++i;
    }

    return true;
}

template <typename KeokenManager>
nlohmann::json process_getallassets(nlohmann::json const& json_in, KeokenManager const& keoken_manager, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    if (getallassets(result, use_testnet_rules, keoken_manager)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_GETALLASSETS_HPP_

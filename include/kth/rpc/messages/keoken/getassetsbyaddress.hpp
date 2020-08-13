// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_GETASSETSBYADDRESS_HPP_
#define KTH_RPC_MESSAGES_GETASSETSBYADDRESS_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <kth/rpc/messages/keoken/keokenutils.hpp>

namespace kth {

inline
bool json_in_getassetsbyaddress(nlohmann::json const& json_object, 
                        std::string& asset_owner) {
    if (json_object["params"].size() == 0)
        return false;
    try {
        asset_owner  = json_object["params"][0].get<std::string>();
        return true;
    } catch (const std::exception & e) {
        return false;
    }

    return true;
}


template <typename KeokenManager>
bool getassetsbyaddress(nlohmann::json& json_object,  int& error, std::string& error_code, std::string& asset_owner, bool use_testnet_rules, KeokenManager const& keoken_manager)
{
    kth::domain::wallet::payment_address wallet = str_to_network_wallet(false, asset_owner);

    if (wallet){
        auto assets_list = keoken_manager.get_assets_by_address(wallet);
        size_t i = 0;
        for(auto const& asset : assets_list) {
            json_object[i]["asset_id"] = asset.asset_id;
            json_object[i]["asset_name"] = asset.asset_name;
            json_object[i]["asset_creator"] = to_network_wallet(use_testnet_rules, asset.asset_creator).encoded();
            json_object[i]["amount"] = asset.amount;
            ++i;
        }
    } else {
        error = kth::RPC_PARSE_ERROR;
        error_code = "Unable to parse address";
    }

    return true;
}

template <typename KeokenManager>
nlohmann::json process_getassetsbyaddress(nlohmann::json const& json_in, KeokenManager const& keoken_manager, bool use_testnet_rules)
{
    nlohmann::json container;
    nlohmann::json result = nlohmann::json::array();
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;
    std::string asset_owner;

    if ( ! json_in_getassetsbyaddress(json_in, asset_owner)){
        container["result"];
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (getassetsbyaddress(result, error, error_code, asset_owner, use_testnet_rules, keoken_manager)) {
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

#endif //KTH_RPC_MESSAGES_GETASSETSBYADDRESS_HPP_

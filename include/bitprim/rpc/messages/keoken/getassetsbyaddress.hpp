/**
* Copyright (c) 2017-2018 Bitprim Inc.
*
* This file is part of bitprim-node.
*
* bitprim-node is free software: you can redistribute it and/or
* modify it under the terms of the GNU Affero General Public License with
* additional permissions to the one published by the Free Software
* Foundation, either version 3 of the License, or (at your option)
* any later version. For more information see LICENSE.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BITPRIM_RPC_MESSAGES_GETASSETSBYADDRESS_HPP_
#define BITPRIM_RPC_MESSAGES_GETASSETSBYADDRESS_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <bitcoin/node/full_node.hpp>


namespace bitprim {

inline
bool json_in_getassetsbyaddress(nlohmann::json const& json_object, 
                        libbitcoin::wallet::payment_address& asset_owner) {

    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;
    try {
        std::string wallet = json_object["asset_owner"];
        libbitcoin::wallet::payment_address read(wallet);
        asset_owner = read;    
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;

}

template <typename Node>
bool getassetsbyaddress(nlohmann::json& json_object, libbitcoin::wallet::payment_address const& asset_owner,bool use_testnet_rules, Node& node)
{
    auto assets_list = node->keoken_manager().get_assets_by_address(asset_owner);
    size_t i = 0;
    for(auto const& asset : assets_list) {
        json_object[i]["asset_id"] = asset.asset_id;
        json_object[i]["asset_name"] = asset.asset_name;
        json_object[i]["asset_creator"] = asset.asset_creator.encoded();
        json_object[i]["amount"] = asset.amount;
        ++i;
    }

    return true;
}

template <typename Node>
nlohmann::json process_getassetsbyaddress(nlohmann::json const& json_in, Node& node, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;
    libbitcoin::wallet::payment_address asset_owner;

    if (json_in_getassetsbyaddress(json_in, asset_owner) && getassetsbyaddress(result, asset_owner, use_testnet_rules, node))
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

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_GETASSETSBYADDRESS_HPP_

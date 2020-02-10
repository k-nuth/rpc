/**
* Copyright (c) 2016-2020 Knuth Project developers.
*
* This file is part of kth-node.
*
* kth-node is free software: you can redistribute it and/or
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

#ifndef KTH_RPC_MESSAGES_GETASSETS_HPP_
#define KTH_RPC_MESSAGES_GETASSETS_HPP_

#include <boost/thread/latch.hpp>

#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/json/json.hpp>
#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <kth/rpc/messages/keoken/keokenutils.hpp>

namespace kth {

template <typename KeokenManager>
bool getassets(nlohmann::json& json_object, bool use_testnet_rules, KeokenManager const& keoken_manager) {
    auto assets_list = keoken_manager.get_assets();
    size_t i = 0;
    for (auto const& asset : assets_list) {
        json_object[i]["asset_id"] = asset.asset_id;
        json_object[i]["asset_name"] = asset.asset_name;
        json_object[i]["asset_creator"] =  to_network_wallet(use_testnet_rules, asset.asset_creator).encoded();
        json_object[i]["amount"] = asset.amount;
        ++i;
    }

    return true;
}

template <typename KeokenManager>
nlohmann::json process_getassets(nlohmann::json const& json_in, KeokenManager const& keoken_manager, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    if (getassets(result, use_testnet_rules, keoken_manager)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_GETASSETS_HPP_
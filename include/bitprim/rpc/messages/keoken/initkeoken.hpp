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

#ifndef BITPRIM_RPC_MESSAGES_INITKEOKEN_HPP_
#define BITPRIM_RPC_MESSAGES_INITKEOKEN_HPP_

#include <boost/thread/latch.hpp>

#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/json/json.hpp>
#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>

// #include <bitcoin/node/full_node.hpp>

namespace bitprim {

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

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_INITKEOKEN_HPP_

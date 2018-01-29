/**
* Copyright (c) 2017 Bitprim developers (see AUTHORS)
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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSMEMPOOL_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSMEMPOOL_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <boost/thread/latch.hpp>

namespace bitprim {

//TODO 
inline
bool json_in_getaddressmempool(nlohmann::json const& json_object, std::vector<std::string>& payment_address) {
    return true;
}

template <typename Blockchain>
bool getaddressmempool(nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, Blockchain const& chain) {
    json_object = nlohmann::json::array();
    return true;
}

template <typename Blockchain>
nlohmann::json process_getaddressmempool(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<std::string> payment_addresses;
    if (!json_in_getaddressmempool(json_in, payment_addresses))
    {
        //load error code
        //return
    }

    if (getaddressmempool(result, error, error_code, payment_addresses, chain))
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

#endif //BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSMEMPOOL_HPP_

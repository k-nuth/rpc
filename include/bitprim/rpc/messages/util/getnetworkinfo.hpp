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

#ifndef BITPRIM_RPC_MESSAGES_UTIL_GETNETWORKINFO_HPP_
#define BITPRIM_RPC_MESSAGES_UTIL_GETNETWORKINFO_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/node/full_node.hpp>

#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

template <typename Node>
bool getnetworkinfo(nlohmann::json& json_object, int& error, std::string& error_code, bool use_testnet_rules, Node & node)
{

    static const int CLIENT_VERSION =
        1000000 * LIBBITCOIN_MAJOR_VERSION
        + 10000 * LIBBITCOIN_MINOR_VERSION
        + 100 * LIBBITCOIN_PATCH_VERSION;

    json_object["version"] = CLIENT_VERSION;

    //TODO 
    json_object["subversion"] = "/Bitprim:"+ std::string(LIBBITCOIN_VERSION);

    //TODO set protocol
    json_object["protocolversion"] = node->network_settings().protocol_maximum;

    //TODO set protocol
    json_object["localservices"] = "000000000000040d";
    json_object["localrelay"] = true;
    json_object["networkactive"] = true;

    json_object["timeoffset"] = 0;

    json_object["connections"] = node->connection_count();
    auto ip = node->network_settings().self;
    auto port = node->network_settings().inbound_port;

    json_object["networks"][0]["name"] = "ipv4";
    json_object["networks"][0]["limited"] = false;
    json_object["networks"][0]["reachable"] = true;
    json_object["networks"][0]["proxy"] = "";
    json_object["networks"][0]["proxy_randomize_credentials"] = false;
    json_object["incrementalfee"] = 0.01;
    json_object["relayfee"] = 0.01;
    
    json_object["localaddresses"][0]["address"] = ip.to_hostname();
    json_object["localaddresses"][0]["port"] = port;
    json_object["localaddresses"][0]["score"] = 1;
    json_object["warnings"] = "";

    return true;

}

template <typename Node>
nlohmann::json process_getnetworkinfo(nlohmann::json const& json_in, Node & node, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    if (getnetworkinfo(result, error, error_code, use_testnet_rules, node))
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

}

#endif

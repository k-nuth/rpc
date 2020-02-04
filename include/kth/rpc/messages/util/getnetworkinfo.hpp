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

#ifndef KTH_RPC_MESSAGES_UTIL_GETNETWORKINFO_HPP_
#define KTH_RPC_MESSAGES_UTIL_GETNETWORKINFO_HPP_

#include <knuth/rpc/json/json.hpp>
#include <bitcoin/network/user_agent.hpp>
#include <bitcoin/node/full_node.hpp>

#include <knuth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

template <typename Node>
bool getnetworkinfo(nlohmann::json& json_object, int& error, std::string& error_code, bool use_testnet_rules, Node & node)
{
    json_object["version"] = 001100;

    // json_object["subversion"] = "/Knuth:"+ std::string(KTH_CORE_VERSION);
    json_object["subversion"] = kth::network::get_user_agent();

    json_object["protocolversion"] = node.network_settings().protocol_maximum;

    //TODO fix format
    json_object["localservices"] = std::to_string(node.network_settings().services);
    json_object["localrelay"] = node.node_settings().refresh_transactions;
    json_object["networkactive"] = true;

    json_object["timeoffset"] = 0;

    json_object["connections"] = node.connection_count();
    auto ip = node.network_settings().self;
    auto port = node.network_settings().inbound_port;

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

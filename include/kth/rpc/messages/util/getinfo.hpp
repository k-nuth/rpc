// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_UTIL_GETINFO_HPP_
#define KTH_RPC_MESSAGES_UTIL_GETINFO_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>
#include <kth/node/full_node.hpp>

#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

template <typename Node>
bool getinfo(nlohmann::json& json_object, int& error, std::string& error_code, bool use_testnet_rules, Node & node)
{

#define CLIENT_VERSION_MAJOR 0
#define CLIENT_VERSION_MINOR 12
#define CLIENT_VERSION_REVISION 0
#define CLIENT_VERSION_BUILD 0
    static int const CLIENT_VERSION =
        1000000 * CLIENT_VERSION_MAJOR
        + 10000 * CLIENT_VERSION_MINOR
        + 100 * CLIENT_VERSION_REVISION
        + 1 * CLIENT_VERSION_BUILD;

    json_object["version"] = CLIENT_VERSION;

    json_object["protocolversion"] = 70013;

    auto last_block_data = get_last_block_difficulty(node.chain_kth());

    if (std::get<0>(last_block_data)) {
        json_object["blocks"] = std::get<1>(last_block_data);
    }

    json_object["timeoffset"] = 0;

    //TODO: get outbound + inbound connections from node
    json_object["connections"] = node.connection_count();

    json_object["proxy"] = "";

    json_object["difficulty"] = std::get<2>(last_block_data);

    //TODO: set testnet variable
    json_object["testnet"] = use_testnet_rules;

    //TODO: set minimun fee
    json_object["relayfee"] = 0.0;

    //TODO: check errors
    json_object["errors"] = "";

    return true;

}

template <typename Node>
nlohmann::json process_getinfo(nlohmann::json const& json_in, Node & node, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    if (getinfo(result, error, error_code, use_testnet_rules, node))
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

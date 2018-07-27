/**
 * Copyright (c) 2017-2018 Bitprim Inc.
 *
 * This file is part of Bitprim.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BITPRIM_RPC_MESSAGES_HPP_
#define BITPRIM_RPC_MESSAGES_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>
#include <bitprim/rpc/messages/messages.hpp>
#include <bitcoin/node/full_node.hpp>

#include <boost/thread/latch.hpp>
#include <bitcoin/bitcoin/multi_crypto_support.hpp>
#include <bitcoin/bitcoin/error.hpp>

namespace bitprim {

template <typename Blockchain>
using message_signature = nlohmann::json(*)(nlohmann::json const&, Blockchain const&, bool);

template <typename Blockchain>
using signature_map = std::unordered_map<std::string, message_signature<Blockchain>>;


template <typename Blockchain>
signature_map<Blockchain> load_signature_map() {

    return signature_map<Blockchain>  {
        {"getrawtransaction", process_getrawtransaction},
        { "getaddressbalance", process_getaddressbalance },
        { "getspentinfo", process_getspentinfo },
        { "getaddresstxids", process_getaddresstxids },
        { "getaddressdeltas", process_getaddressdeltas },
        { "getaddressutxos", process_getaddressutxos },
        { "getblockhashes", process_getblockhashes },
        { "getaddressmempool", process_getaddressmempool },
        { "getbestblockhash", process_getbestblockhash },
        { "getblock", process_getblock },
        { "getblockhash", process_getblockhash },
        { "getblockchaininfo", process_getblockchaininfo },
        { "getblockheader", process_getblockheader },
        { "getblockcount", process_getblockcount },
        { "getdifficulty", process_getdifficulty },
        { "getchaintips", process_getchaintips },
        { "validateaddress", process_validateaddress },
        { "getblocktemplate", process_getblocktemplate },
        { "getmininginfo", process_getmininginfo }
    };
}

template <typename Node, typename Blockchain>
nlohmann::json process_data_element(nlohmann::json const& json_in, bool use_testnet_rules,  Node & node, signature_map<Blockchain> const& signature_map) {
    
    auto key = json_in["method"].get<std::string>();

    //std::cout << "Processing json " << key << std::endl;
    //std::cout << "Detail json " << json_in << std::endl;

    auto it = signature_map.find(key);

    if (it != signature_map.end()) {
        return it->second(json_in, node->chain_bitprim(), use_testnet_rules);
    }
    
    if (key == "submitblock")
        return process_submitblock(json_in, node->chain_bitprim(), use_testnet_rules);

    if (key == "sendrawtransaction")
        return process_sendrawtransaction(json_in, node->chain_bitprim(), use_testnet_rules);

    if (key == "createtransaction")
        return process_createtransaction(json_in, node->chain_bitprim(), use_testnet_rules);

    if (key == "createsignature")
        return process_createsignature(json_in, node->chain_bitprim(), use_testnet_rules);

    if (key == "setsignature")
        return process_setsignature(json_in, node->chain_bitprim(), use_testnet_rules);
#ifdef WITH_KEOKEN
    if (key == "createasset")
        return process_createasset(json_in, node->chain_bitprim(), use_testnet_rules);

    if (key == "sendtoken")
        return process_sendtoken(json_in, node->chain_bitprim(), use_testnet_rules);

    if (key == "initkeoken")
        return process_initkeoken(json_in, node, use_testnet_rules);

    if (key == "getassets")
        return process_getassets(json_in, node, use_testnet_rules);

    if (key == "getassetsbyaddress")
        return process_getassetsbyaddress(json_in, node, use_testnet_rules);

    if (key == "getallassets")
        return process_getallassets(json_in, node, use_testnet_rules);
#endif
    if (key == "getinfo")
        return process_getinfo(json_in, node, use_testnet_rules);

    if (key == "getnetworkinfo")
        return process_getnetworkinfo(json_in, node, use_testnet_rules);

    //std::cout << key << " Command Not yet implemented." << std::endl;
    return nlohmann::json(); //TODO: error!

}

template <typename Node, typename Blockchain>
std::string process_data(nlohmann::json const& json_object, bool use_testnet_rules, Node & node, signature_map<Blockchain> const& signature_map) {
    //std::cout << "method: " << json_object["method"].get<std::string>() << "\n";
    //Bitprim-mining process data

    if (json_object.is_array()) {
        nlohmann::json res;
        size_t i = 0;
        for (const auto & method : json_object) {
            res[i] = process_data_element(method, use_testnet_rules, node, signature_map);
            ++i;
        }
        return res.dump();
    }
    else {
        return process_data_element(json_object, use_testnet_rules, node, signature_map).dump();
    }
}

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_HPP_

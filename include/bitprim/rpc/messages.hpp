// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_HPP_
#define KTH_RPC_MESSAGES_HPP_

#include <boost/thread/latch.hpp>

#ifdef KTH_USE_DOMAIN
#include <bitcoin/infrastructure/error.hpp>
#else
#include <bitcoin/bitcoin/error.hpp>
#endif // KTH_USE_DOMAIN


#include <bitcoin/bitcoin/multi_crypto_support.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>
// #include <bitcoin/node/full_node.hpp>

#include <knuth/rpc/json/json.hpp>
#include <knuth/rpc/messages/messages.hpp>

#ifdef KTH_WITH_KEOKEN
#include <knuth/keoken/manager.hpp>
#include <knuth/keoken/memory_state.hpp>
#endif

namespace kth {

// #ifdef KTH_WITH_KEOKEN
//     using keoken_manager_t = knuth::keoken::manager<knuth::keoken::memory_state>;
// #endif

template <typename Blockchain>
using message_signature = nlohmann::json(*)(nlohmann::json const&, Blockchain const&, bool);

template <typename Blockchain>
using signature_map = std::unordered_map<std::string, message_signature<Blockchain>>;


template <typename Blockchain>
signature_map<Blockchain> load_signature_map() {

    return signature_map<Blockchain>  {

          { "getblockhash", process_getblockhash }
        , { "validateaddress", process_validateaddress }

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW_BLOCKS) || defined(KTH_DB_NEW_FULL)
        , { "getblockhashes", process_getblockhashes }
        , { "getblock", process_getblock }
        , { "getblockheader", process_getblockheader }
#endif

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 
#endif

#if defined(KTH_DB_TRANSACTION_UNCONFIRMED) || defined(KTH_DB_NEW_FULL)
        , { "getaddresstxids", process_getaddresstxids }
        , { "getaddressmempool", process_getaddressmempool }
#endif

#if (defined(KTH_DB_LEGACY) && defined(KTH_DB_SPENDS)) || defined(KTH_DB_NEW_FULL)
        , { "getrawtransaction", process_getrawtransaction}
        , { "getspentinfo", process_getspentinfo }
#endif        

#if (defined(KTH_DB_LEGACY) && defined(KTH_DB_SPENDS) && defined(KTH_DB_HISTORY)) || defined(KTH_DB_NEW_FULL)
        , { "getaddressbalance", process_getaddressbalance }
        , { "getaddressdeltas", process_getaddressdeltas }
        , { "getaddressutxos", process_getaddressutxos }
#endif        

#ifdef KTH_WITH_MEMPOOL
        , { "getblocktemplate", process_getblocktemplate }
#endif
    };
}

template <typename Blockchain>
signature_map<Blockchain> load_signature_map_no_params() {

    return signature_map<Blockchain>  {
          { "getbestblockhash", process_getbestblockhash }
        , { "getblockchaininfo", process_getblockchaininfo }

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW_BLOCKS) || defined(KTH_DB_NEW_FULL)
        , { "getchaintips", process_getchaintips }
#endif

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 
        , { "getmininginfo", process_getmininginfo }
        , { "getdifficulty", process_getdifficulty }
#endif

        , { "getblockcount", process_getblockcount }
    };
}


#ifdef KTH_WITH_KEOKEN
template <typename Node, typename Blockchain, typename KeokenManager>
nlohmann::json process_data_element(nlohmann::json const& json_in, bool use_testnet_rules, Node& node, KeokenManager& keoken_manager, signature_map<Blockchain> const& signature_, signature_map<Blockchain> const& no_params_map) {
#else
template <typename Node, typename Blockchain>
nlohmann::json process_data_element(nlohmann::json const& json_in, bool use_testnet_rules, Node& node, signature_map<Blockchain> const& signature_, signature_map<Blockchain> const& no_params_map) {
#endif
    if (json_in.find("method") != json_in.end()) {
        auto key = json_in["method"].get<std::string>();

        //std::cout << "Processing json " << key << std::endl;
        //std::cout << "Detail json " << json_in << std::endl;

        auto it = signature_.find(key);
        if (it != signature_.end()) {
            if(json_in.find("params") != json_in.end()){
                return it->second(json_in, node.chain_kth(), use_testnet_rules);
            } else {
                nlohmann::json container;
                container["result"];
                container["error"]["code"] = 0;
                container["error"]["message"] = "Please enter `params`";
                return container;
            }
        }

        auto it_n = no_params_map.find(key);
        if ( it_n != no_params_map.end()) {
            return  it_n->second(json_in, node.chain_kth(), use_testnet_rules);
        }

#ifdef KTH_WITH_KEOKEN
        if (key == "initkeoken")
            return process_initkeoken(json_in, keoken_manager, use_testnet_rules);

        if (key == "getassets")
            return process_getassets(json_in, keoken_manager, use_testnet_rules);

        if (key == "getallassets")
            return process_getallassets(json_in, keoken_manager, use_testnet_rules);

        if (key == "getassetsbyaddress")
            return process_getassetsbyaddress(json_in, keoken_manager, use_testnet_rules);
#endif //KTH_WITH_KEOKEN

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 
        if (key == "getinfo")
            return process_getinfo(json_in, node, use_testnet_rules);
#endif

        if (key == "getnetworkinfo")
            return process_getnetworkinfo(json_in, node, use_testnet_rules);

        if (json_in.find("params") != json_in.end()) {
            if (key == "submitblock")
                return process_submitblock(json_in, node.chain_kth(), use_testnet_rules);

            if (key == "sendrawtransaction")
                return process_sendrawtransaction(json_in, node.chain_kth(), use_testnet_rules);

            if (key == "createtransaction")
                return process_createtransaction(json_in, node.chain_kth(), use_testnet_rules);

            if (key == "createsignature")
                return process_createsignature(json_in, node.chain_kth(), use_testnet_rules);

            if (key == "setsignature")
                return process_setsignature(json_in, node.chain_kth(), use_testnet_rules);

            if (key == "newwallet")
                return process_newwallet(json_in, node.chain_kth(), use_testnet_rules);

#ifdef KTH_WITH_KEOKEN
            if (key == "createasset")
                return process_createasset(json_in, use_testnet_rules);

            if (key == "sendtoken")
                return process_sendtoken(json_in, use_testnet_rules);

            if (key == "getkeokenaddress")
                return process_getkeokenaddress(json_in, node.chain_kth(), node.node_settings().keoken_genesis_height, use_testnet_rules);

            if (key == "getkeokenblock")
                return process_getkeokenblock(json_in, node.chain_kth(), use_testnet_rules);

#endif //KTH_WITH_KEOKEN

            nlohmann::json container;
            container["result"];
            container["error"]["code"] = knuth::RPC_INVALID_REQUEST;
            container["error"]["message"] = "Invalid or incomplete command";
            return container;
        } else {
            nlohmann::json container;
            container["result"];
            container["error"]["code"] = knuth::RPC_INVALID_REQUEST;
            container["error"]["message"] = "Invalid or incomplete command";
            return container;
        }


        
    }
    //std::cout << key << " Command Not yet implemented." << std::endl;
    return nlohmann::json(); //TODO: error!
}

#ifdef KTH_WITH_KEOKEN
template <typename Node, typename Blockchain, typename KeokenManager>
std::string process_data(nlohmann::json const& json_object, bool use_testnet_rules, Node& node, KeokenManager& keoken_manager, signature_map<Blockchain> const& signature_, signature_map<Blockchain> const& no_params_map) {
#else
template <typename Node, typename Blockchain>
std::string process_data(nlohmann::json const& json_object, bool use_testnet_rules, Node& node, signature_map<Blockchain> const& signature_, signature_map<Blockchain> const& no_params_map) {
#endif
    //std::cout << "method: " << json_object["method"].get<std::string>() << "\n";
    //Mining process data

    if (json_object.is_array()) {
        nlohmann::json res;
        size_t i = 0;
        for (auto const & method : json_object) {

#ifdef KTH_WITH_KEOKEN
            res[i] = process_data_element(method, use_testnet_rules, node, keoken_manager, signature_, no_params_map);
#else
            res[i] = process_data_element(method, use_testnet_rules, node, signature_, no_params_map);
#endif
            ++i;
        }
        return res.dump();
    }
    else {
#ifdef KTH_WITH_KEOKEN
        return process_data_element(json_object, use_testnet_rules, node, keoken_manager, signature_, no_params_map).dump();
#else
        return process_data_element(json_object, use_testnet_rules, node, signature_, no_params_map).dump();
#endif
    }
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_HPP_

// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_WALLET_CREATEASSET_HPP_
#define KTH_RPC_MESSAGES_WALLET_CREATEASSET_HPP_

#include <boost/thread/latch.hpp>

#include <kth/rpc/json/json.hpp>
#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
include <kth/keoken/wallet/create_transaction.hpp>

// #include <kth/blockchain/interface/block_chain.hpp>

namespace kth {

inline
bool json_in_createasset(nlohmann::json const& json_object, 
                        std::vector<kth::domain::chain::input_point>& outputs_to_spend,  
                        kth::domain::wallet::payment_address& asset_owner, uint64_t& utxo_satoshis, 
                        std::string& asset_name, uint64_t& asset_amount) {

    auto const & size = json_object["params"].size();

    if (size == 0) {
        return false;
    }

    try {
        kth::hash_digest hash_to_spend;
        for (auto const& o : json_object["params"]["origin"]) {
          kth::decode_hash(hash_to_spend, o["output_hash"]);
          outputs_to_spend.push_back({hash_to_spend, o["output_index"]});
        }

        std::string wallet = json_object["params"]["asset_owner"];
        kth::domain::wallet::payment_address read(wallet);
        asset_owner = read;
        utxo_satoshis = json_object["params"]["utxo_satoshis"];
        asset_name = json_object["params"]["asset_name"];
        asset_amount = json_object["params"]["asset_amount"];
    
    } catch (const std::exception & e) {
        return false;
    }
    return true;

}

// template <typename Blockchain>
inline
bool createasset(nlohmann::json& json_object, int& error, std::string& error_code,
                        std::vector<kth::domain::chain::input_point>& outputs_to_spend,  
                        kth::domain::wallet::payment_address& asset_owner, uint64_t& utxo_satoshis, 
                        std::string& asset_name, uint64_t& asset_amount, bool use_testnet_rules) {
    json_object = kth::encode_base16(kth::keoken::wallet::tx_encode_create_asset(outputs_to_spend, asset_owner, utxo_satoshis, asset_name, asset_amount).second.to_data(true));
    return true;
}

// template <typename Blockchain>
inline
nlohmann::json process_createasset(nlohmann::json const& json_in, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;


    std::vector<kth::domain::chain::input_point> outputs_to_spend;
    kth::domain::wallet::payment_address asset_owner;
    uint64_t utxo_satoshis; 
    std::string asset_name;
    uint64_t asset_amount;

    if ( ! json_in_createasset(json_in, outputs_to_spend, asset_owner, utxo_satoshis, asset_name, asset_amount)) { //if false return error
        container["result"];
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "Parse error.";
        return container;
    }

    if (createasset(result, error, error_code, outputs_to_spend, asset_owner, utxo_satoshis, asset_name, asset_amount, use_testnet_rules)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_WALLET_CREATEASSET_HPP_

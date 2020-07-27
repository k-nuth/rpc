// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_WALLET_SETSIGNATURE_HPP_
#define KTH_RPC_MESSAGES_WALLET_SETSIGNATURE_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <kth/domain/wallet/transaction_functions.hpp>

namespace kth {


inline
bool json_in_set_signature(nlohmann::json const& json_object, 
                              kth::ec_secret& private_key, std::string& json_signature,
                              kth::domain::chain::transaction& tx, uint32_t& index) {

    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;
    try {
        // Priv key
        private_key = kth::create_secret_from_seed(json_object["params"]["seed"]);
        json_signature = json_object["params"]["signature"];
        // TX
        kth::data_chunk raw_data;
        kth::decode_base16(raw_data, json_object["params"]["tx"]);
        domain::entity_from_data(tx, raw_data);

        // Index
        index = json_object["params"]["index"];
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;

}

template <typename Blockchain>
bool set_signature(nlohmann::json& json_object, int& error, std::string& error_code,
                              kth::ec_secret& private_key, std::string& json_signature,
                              kth::domain::chain::transaction& tx, uint32_t& index, 
                              bool use_testnet_rules, Blockchain& chain)
{

    auto pub_key = kth::secret_to_compressed_public(private_key);
    // Redeem script for P2KH [SIGNATURE][PUBKEY]
    kth::domain::chain::script input_script;
    input_script.from_string("[" + json_signature + "] [" + pub_key.encoded() + "]");

    json_object = kth::encode_base16(kth::domain::wallet::input_set(input_script, tx, index).second.to_data(true));
    return true;
}

template <typename Blockchain>
nlohmann::json process_setsignature(nlohmann::json const& json_in, Blockchain& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

  kth::ec_secret private_key;
  std::string json_signature;
  kth::domain::chain::transaction tx;
  uint32_t index;
    if ( ! json_in_set_signature(json_in, private_key, json_signature, tx,  index)) //if false return error
    {
        container["result"];
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (set_signature(result, error, error_code, private_key, json_signature,  tx, index, use_testnet_rules, chain))
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

} //namespace kth

#endif //KTH_RPC_MESSAGES_WALLET_SETSIGNATURE_HPP_

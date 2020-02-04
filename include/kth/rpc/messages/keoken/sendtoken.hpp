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

#ifndef KTH_RPC_MESSAGES_WALLET_SENDTOKEN_HPP_
#define KTH_RPC_MESSAGES_WALLET_SENDTOKEN_HPP_

#include <boost/thread/latch.hpp>

#include <kth/rpc/json/json.hpp>
#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
include <kth/keoken/wallet/create_transaction.hpp>

// #include <kth/blockchain/interface/block_chain.hpp>

namespace kth {

inline
bool json_in_sendtoken(nlohmann::json const& json_object, 
                        std::vector<kth::chain::input_point>& outputs_to_spend,  
                        kth::wallet::payment_address& asset_owner, uint64_t& utxo_satoshis,
                        kth::wallet::payment_address& token_receiver, uint64_t& dust,
                        kth::keoken::asset_id_t& asset_id,
                        kth::keoken::amount_t& asset_amount) {

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
        kth::wallet::payment_address read(wallet);
        asset_owner = read;
        utxo_satoshis = json_object["params"]["utxo_satoshis"];

        std::string wallet_receiver = json_object["params"]["token_receiver"];
        kth::wallet::payment_address read_receiver(wallet_receiver);
        token_receiver = read_receiver;
        dust = json_object["params"]["dust"];

        asset_id = json_object["params"]["asset_id"];
        asset_amount = json_object["params"]["asset_amount"];
    
    } catch (const std::exception & e) {
        return false;
    }
    return true;
}

// template <typename Blockchain>
inline
bool sendtoken(nlohmann::json& json_object, int& error, std::string& error_code,
                        std::vector<kth::chain::input_point>& outputs_to_spend,  
                        kth::wallet::payment_address const& asset_owner, uint64_t& utxo_satoshis,
                        kth::wallet::payment_address const& token_receiver, uint64_t const& dust,
                        kth::keoken::asset_id_t& asset_id,
                        kth::keoken::amount_t& asset_amount, bool use_testnet_rules) {
    json_object = kth::encode_base16(kth::keoken::wallet::tx_encode_send_token(outputs_to_spend, asset_owner, utxo_satoshis, token_receiver, dust, asset_id, asset_amount).second.to_data(true));
    return true;
}

// template <typename Blockchain>
inline
nlohmann::json process_sendtoken(nlohmann::json const& json_in, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;


    std::vector<kth::chain::input_point> outputs_to_spend;
    kth::wallet::payment_address asset_owner;
    uint64_t utxo_satoshis;
    kth::wallet::payment_address token_receiver;
    uint64_t dust;
    kth::keoken::asset_id_t asset_id;
    kth::keoken::amount_t asset_amount;

    if ( ! json_in_sendtoken(json_in, outputs_to_spend, asset_owner, utxo_satoshis, token_receiver, dust, asset_id, asset_amount)) { //if false return error
        container["result"];
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "Parse error.";
        return container;
    }

    if (sendtoken(result, error, error_code, outputs_to_spend, asset_owner, utxo_satoshis, token_receiver, dust, asset_id, asset_amount, use_testnet_rules)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }
    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_WALLET_SENDTOKEN_HPP_

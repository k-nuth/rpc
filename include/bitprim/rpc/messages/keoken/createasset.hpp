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

#ifndef BITPRIM_RPC_MESSAGES_WALLET_CREATEASSET_HPP_
#define BITPRIM_RPC_MESSAGES_WALLET_CREATEASSET_HPP_

#include <boost/thread/latch.hpp>

#include <bitprim/rpc/json/json.hpp>
#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <bitprim/keoken/wallet/create_transaction.hpp>

// #include <bitcoin/blockchain/interface/block_chain.hpp>

namespace bitprim {

inline
bool json_in_createasset(nlohmann::json const& json_object, 
                        std::vector<libbitcoin::chain::input_point>& outputs_to_spend,  
                        libbitcoin::wallet::payment_address& asset_owner, uint64_t& utxo_satoshis, 
                        std::string& asset_name, uint64_t& asset_amount) {

    auto const & size = json_object["params"].size();

    if (size == 0) {
        return false;
    }

    try {
        libbitcoin::hash_digest hash_to_spend;
        for (auto const& o : json_object["params"]["origin"]) {
          libbitcoin::decode_hash(hash_to_spend, o["output_hash"]);
          outputs_to_spend.push_back({hash_to_spend, o["output_index"]});
        }

        std::string wallet = json_object["params"]["asset_owner"];
        libbitcoin::wallet::payment_address read(wallet);
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
                        std::vector<libbitcoin::chain::input_point>& outputs_to_spend,  
                        libbitcoin::wallet::payment_address& asset_owner, uint64_t& utxo_satoshis, 
                        std::string& asset_name, uint64_t& asset_amount, bool use_testnet_rules) {
    json_object = libbitcoin::encode_base16(bitprim::keoken::wallet::tx_encode_create_asset(outputs_to_spend, asset_owner, utxo_satoshis, asset_name, asset_amount).second.to_data(true));
    return true;
}

// template <typename Blockchain>
inline
nlohmann::json process_createasset(nlohmann::json const& json_in, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;


    std::vector<libbitcoin::chain::input_point> outputs_to_spend;
    libbitcoin::wallet::payment_address asset_owner;
    uint64_t utxo_satoshis; 
    std::string asset_name;
    uint64_t asset_amount;

    if ( ! json_in_createasset(json_in, outputs_to_spend, asset_owner, utxo_satoshis, asset_name, asset_amount)) { //if false return error
        container["result"];
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
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

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_WALLET_CREATEASSET_HPP_

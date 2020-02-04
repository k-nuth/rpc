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

#ifndef KTH_RPC_MESSAGES_WALLET_CREATESIGNATURE_HPP_
#define KTH_RPC_MESSAGES_WALLET_CREATESIGNATURE_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <kth/domain/wallet/transaction_functions.hpp>

namespace kth {

inline
bool json_in_create_signature(nlohmann::json const& json_object, 
                              kth::ec_secret& private_key, kth::chain::script& output_script,
                              kth::chain::transaction& tx, uint64_t& amount, uint32_t& index) {

    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;
    try {
        // Priv key
        private_key = kth::create_secret_from_seed(json_object["params"]["seed"]);
        // Script
        kth::data_chunk raw_script;
        kth::decode_base16(raw_script, json_object["params"]["script"]);
        output_script.from_data(raw_script, false);
        // TX
        kth::data_chunk raw_data;
        kth::decode_base16(raw_data, json_object["params"]["tx"]);
        tx.from_data(raw_data);
        // Amount
        amount = json_object["params"]["amount"];
        // Index
        index = json_object["params"]["index"];
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;

}

template <typename Blockchain>
bool create_signature(nlohmann::json& json_object, int& error, std::string& error_code,
                              kth::ec_secret& private_key, kth::chain::script& output_script,
                              kth::chain::transaction& tx, uint64_t& amount, uint32_t& index, 
                              bool use_testnet_rules, Blockchain& chain)
{
    json_object = kth::encode_base16(kth::wallet::input_signature_bch(private_key, output_script, tx, amount, index).second);
    return true;
}

template <typename Blockchain>
nlohmann::json process_createsignature(nlohmann::json const& json_in, Blockchain& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

  kth::ec_secret private_key;
  kth::chain::script output_script;
  kth::chain::transaction tx;
  uint64_t amount;
  uint32_t index;
    if (!json_in_create_signature(json_in, private_key, output_script, tx, amount, index)) //if false return error
    {
        container["result"];
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (create_signature(result, error, error_code, private_key, output_script,  tx, amount, index, use_testnet_rules, chain))
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

#endif //KTH_RPC_MESSAGES_WALLET_CREATESIGNATURE_HPP_

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

#ifndef BITPRIM_RPC_MESSAGES_WALLET_CREATESIGNATURE_HPP_
#define BITPRIM_RPC_MESSAGES_WALLET_CREATESIGNATURE_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <bitcoin/bitcoin/wallet/transaction_functions.hpp>

namespace bitprim {

inline
bool json_in_create_signature(nlohmann::json const& json_object, 
                              libbitcoin::ec_secret& private_key, libbitcoin::chain::script& output_script,
                              libbitcoin::chain::transaction& tx, uint64_t& amount, uint32_t& index) {

    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;
    try {
        // Priv key
        private_key = bitprim::create_secret_from_seed(json_object["params"]["seed"]);
        // Script
        libbitcoin::data_chunk raw_script;
        libbitcoin::decode_base16(raw_script, json_object["params"]["script"]);
        output_script.from_data(raw_script, false);
        // TX
        libbitcoin::data_chunk raw_data;
        libbitcoin::decode_base16(raw_data, json_object["params"]["tx"]);
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
                              libbitcoin::ec_secret& private_key, libbitcoin::chain::script& output_script,
                              libbitcoin::chain::transaction& tx, uint64_t& amount, uint32_t& index, 
                              bool use_testnet_rules, Blockchain& chain)
{
    json_object = libbitcoin::encode_base16(libbitcoin::wallet::input_signature_bch(private_key, output_script, tx, amount, index).second);
    return true;
}

template <typename Blockchain>
nlohmann::json process_createsignature(nlohmann::json const& json_in, Blockchain& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

  libbitcoin::ec_secret private_key;
  libbitcoin::chain::script output_script;
  libbitcoin::chain::transaction tx;
  uint64_t amount;
  uint32_t index;
    if (!json_in_create_signature(json_in, private_key, output_script, tx, amount, index)) //if false return error
    {
        container["result"];
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
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

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_WALLET_CREATESIGNATURE_HPP_

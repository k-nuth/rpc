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

#ifndef KTH_RPC_MESSAGES_WALLET_SETSIGNATURE_HPP_
#define KTH_RPC_MESSAGES_WALLET_SETSIGNATURE_HPP_

#include <knuth/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <knuth/rpc/messages/error_codes.hpp>
#include <knuth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <bitcoin/bitcoin/wallet/transaction_functions.hpp>

namespace bitprim {


inline
bool json_in_set_signature(nlohmann::json const& json_object, 
                              libbitcoin::ec_secret& private_key, std::string& json_signature,
                              libbitcoin::chain::transaction& tx, uint32_t& index) {

    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;
    try {
        // Priv key
        private_key = knuth::create_secret_from_seed(json_object["params"]["seed"]);
        json_signature = json_object["params"]["signature"];
        // TX
        libbitcoin::data_chunk raw_data;
        libbitcoin::decode_base16(raw_data, json_object["params"]["tx"]);
        tx.from_data(raw_data);
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
                              libbitcoin::ec_secret& private_key, std::string& json_signature,
                              libbitcoin::chain::transaction& tx, uint32_t& index, 
                              bool use_testnet_rules, Blockchain& chain)
{

    auto pub_key = knuth::secret_to_compressed_public(private_key);
    // Redeem script for P2KH [SIGNATURE][PUBKEY]
    libbitcoin::chain::script input_script;
    input_script.from_string("[" + json_signature + "] [" + pub_key.encoded() + "]");

    json_object = libbitcoin::encode_base16(libbitcoin::wallet::input_set(input_script, tx, index).second.to_data(true));
    return true;
}

template <typename Blockchain>
nlohmann::json process_setsignature(nlohmann::json const& json_in, Blockchain& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

  libbitcoin::ec_secret private_key;
  std::string json_signature;
  libbitcoin::chain::transaction tx;
  uint32_t index;
    if (!json_in_set_signature(json_in, private_key, json_signature, tx,  index)) //if false return error
    {
        container["result"];
        container["error"]["code"] = knuth::RPC_PARSE_ERROR;
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

} //namespace bitprim

#endif //KTH_RPC_MESSAGES_WALLET_SETSIGNATURE_HPP_

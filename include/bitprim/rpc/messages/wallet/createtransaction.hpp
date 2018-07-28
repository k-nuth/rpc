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

#ifndef BITPRIM_RPC_MESSAGES_WALLET_CREATETRANSACTION_HPP_
#define BITPRIM_RPC_MESSAGES_WALLET_CREATETRANSACTION_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <bitcoin/bitcoin/wallet/transaction_functions.hpp>

namespace bitprim {

inline
bool json_in_createtransaction(nlohmann::json const& json_object, 
                              std::vector<libbitcoin::chain::input_point>& outputs_to_spend,  
                              std::vector<std::pair<libbitcoin::wallet::payment_address, uint64_t>>& outputs,
                              libbitcoin::chain::output::list& extra_outputs) {
    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;
    try {
      libbitcoin::hash_digest hash_to_spend;
      for (auto const& o : json_object["params"]["origin"]) {
        libbitcoin::decode_hash(hash_to_spend, o["output_hash"]);
        outputs_to_spend.push_back({hash_to_spend, o["output_index"]});
      }

      for (auto const& d : json_object["params"]["dests"]) {
        // Implicit json to string conversion
        std::string addr = d["addr"];
        outputs.push_back({libbitcoin::wallet::payment_address(addr), d["amount"]});
      }

      for (auto const& extra : json_object["params"]["extra_outputs"]){
        libbitcoin::data_chunk script_string;
        libbitcoin::decode_base16(script_string, extra["script"]);
        libbitcoin::chain::script script;
        script.from_data(script_string, false);
        extra_outputs.push_back({extra["amount"], script});
      }
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;

}

template <typename Blockchain>
bool createtransaction(nlohmann::json& json_object, int& error, std::string& error_code,
                       std::vector<libbitcoin::chain::input_point>& outputs_to_spend,
                       std::vector<std::pair<libbitcoin::wallet::payment_address, uint64_t>>& outputs,
                       libbitcoin::chain::output::list& extra_outputs, bool use_testnet_rules, Blockchain& chain)
{
    json_object = libbitcoin::encode_base16(libbitcoin::wallet::tx_encode(outputs_to_spend, outputs, extra_outputs).second.to_data(true));
    return true;
}

template <typename Blockchain>
nlohmann::json process_createtransaction(nlohmann::json const& json_in, Blockchain& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;


    std::vector<libbitcoin::chain::input_point> outputs_to_spend;
    std::vector<std::pair<libbitcoin::wallet::payment_address, uint64_t>> outputs;
    libbitcoin::chain::output::list extra_outputs;

    if (!json_in_createtransaction(json_in, outputs_to_spend, outputs, extra_outputs)) //if false return error
    {
        container["result"];
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (createtransaction(result, error, error_code, outputs_to_spend, outputs, extra_outputs, use_testnet_rules, chain))
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

#endif //BITPRIM_RPC_MESSAGES_WALLET_CREATETRANSACTION_HPP_

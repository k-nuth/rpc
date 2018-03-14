/**
* Copyright (c) 2017 Bitprim developers (see AUTHORS)
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

#ifndef BITPRIM_RPC_MESSAGES_MINING_GETMININGINFO_HPP_
#define BITPRIM_RPC_MESSAGES_MINING_GETMININGINFO_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/utils.hpp>
#include <bitcoin/bitcoin/multi_crypto_support.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

    template <typename Blockchain>
    bool getmininginfo(nlohmann::json& json_object, int& error, std::string& error_code, bool use_testnet_rules, Blockchain const& chain)
    {
        auto last_block_data = get_last_block_difficulty(chain);

        if (std::get<0>(last_block_data)) {
            json_object["blocks"] = std::get<1>(last_block_data);
        }

        //TODO: check size and weight on mainnet, testnet is sending 0s
        json_object["currentblocksize"] = libbitcoin::get_max_block_size();
        json_object["currentblockweight"] = libbitcoin::get_max_block_size();
        json_object["currentblocktx"] = 0;

        json_object["difficulty"] = std::get<2>(last_block_data);
        //TODO: check errors
        json_object["errors"] = "";

        //TODO: calculate networkhashps
        json_object["networkhashps"] = 0.0;
        //TODO: calculate pooledtx
        json_object["pooledtx"] = 0;

        json_object["testnet"] = use_testnet_rules;

        //TODO: libbitcoin does not support regtest
        if (use_testnet_rules) {
            json_object["chain"] = "test";
        }
        else {
            json_object["chain"] = "main";
        }
        return true;
    }

    template <typename Blockchain>
    nlohmann::json process_getmininginfo(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
    {
        nlohmann::json container, result;
        container["id"] = json_in["id"];

        int error = 0;
        std::string error_code;

        if (getmininginfo(result, error, error_code, use_testnet_rules, chain))
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


}
#endif

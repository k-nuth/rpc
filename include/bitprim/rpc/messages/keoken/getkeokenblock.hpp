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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETKEOKENBLOCK_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETKEOKENBLOCK_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <bitprim/rpc/messages/keoken/keokenutils.hpp>

namespace bitprim {

inline
bool json_in_getkeokenblock(nlohmann::json const& json_object, std::string & hash) {
    if (json_object["params"].size() == 0)
        return false;

    try {
        hash = json_object["params"][0];
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;
}

template <typename Blockchain>
bool getkeokenblock(nlohmann::json& json_object, int& error, std::string& error_code, const std::string & block_hash, Blockchain const& chain) {
#ifdef BITPRIM_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
#endif
    nlohmann::json transactions;

    libbitcoin::hash_digest hash;
    if (libbitcoin::decode_hash(hash, block_hash)) {
        boost::latch latch(2);
        chain.fetch_block_keoken(hash, witness, [&](const libbitcoin::code &ec, libbitcoin::header_const_ptr header, size_t height, std::shared_ptr <std::vector <libbitcoin::transaction_const_ptr>> keoken_txs, uint64_t serialized_size)
        {
            if (ec == libbitcoin::error::success) {
                //json_object["hash"] = block_hash;

                json_object["hash"] = block_hash;

                size_t top_height;
                chain.get_last_height(top_height);
                json_object["confirmations"] = top_height - height + 1;

                json_object["size"] = serialized_size;
                json_object["height"] = height;
                json_object["version"] = header->version();
                // TODO: encode the version to base 16
                json_object["versionHex"] = header->version();
                json_object["merkleroot"] = libbitcoin::encode_hash(header->merkle());
                json_object["time"] = header->timestamp();
                // TODO: get real median time
                json_object["mediantime"] = header->timestamp();
                json_object["nonce"] = header->nonce();
                // TODO: encode bits to base 16
                json_object["bits"] = header->bits();
                json_object["difficulty"] = bits_to_difficulty(header->bits());
                // TODO: validate that proof is chainwork
                // Optimizate the encoded to base 16
                std::stringstream ss;
                ss << std::setfill('0')
                   << std::nouppercase
                   << std::hex
                   << header->proof();
                json_object["chainwork"] = ss.str();
                json_object["previousblockhash"] = libbitcoin::encode_hash(header->previous_block_hash());

                json_object["nextblockhash"];

                libbitcoin::hash_digest nexthash;
                if(chain.get_block_hash(nexthash, height+1))
                  json_object["nextblockhash"] = libbitcoin::encode_hash(nexthash);


                int i = 0;
                for(auto& tx : (*keoken_txs)){
                    transactions[i] = decode_keoken(chain, tx);
                    ++i;
                }
                json_object["transactions"] = transactions;

            }
            else {
                if (ec == libbitcoin::error::not_found)
                {
                    error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
                    error_code = "Block not found";
                }
                else {
                    error = bitprim::RPC_INTERNAL_ERROR;
                    error_code = "Can't read block from disk";
                }
            }
            latch.count_down();
        });
        latch.count_down_and_wait();
    }
    else {
        error = bitprim::RPC_INVALID_PARAMETER;
        error_code = "Invalid block hash";
    }

    if (error != 0)
        return false;
    return true;

}


template <typename Blockchain>
nlohmann::json process_getkeokenblock(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{

    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string hash;
    if (!json_in_getkeokenblock(json_in, hash)) //if false return error
    {
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (getkeokenblock(result, error, error_code, hash, chain))
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

#endif //BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETKEOKENBLOCK_HPP_

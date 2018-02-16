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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCK_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCK_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

inline
bool json_in_getblock(nlohmann::json const& json_object, std::string & hash, bool & verbose) {
    if (json_object["params"].size() == 0)
        return false;
    verbose = true;
    try {
        hash = json_object["params"][0];
        if (json_object["params"].size() == 2) {
            verbose = json_object["params"][1];
        }
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;
}

template <typename Blockchain>
bool getblock(nlohmann::json& json_object, int& error, std::string& error_code, const std::string & block_hash, bool verbose, Blockchain const& chain) {
    libbitcoin::hash_digest hash;
    if (libbitcoin::decode_hash(hash, block_hash)) {
        if(verbose)
        {

            boost::latch latch(2);
            chain.fetch_block_txs_size(hash, [&](const libbitcoin::code &ec, libbitcoin::block_const_ptr block, 
                const libbitcoin::hash_list& txs, uint64_t serialized_size,size_t height) 
            {
                if (ec == libbitcoin::error::success) {
                        json_object["hash"] = block_hash;

                        size_t top_height;
                        chain.get_last_height(top_height);
                        json_object["confirmations"] = top_height - height + 1;

                        json_object["size"] = serialized_size;
                        json_object["height"] = height;
                        json_object["version"] = block->header().version();
                        // TODO: encode the version to base 16
                        json_object["versionHex"] = block->header().version();
                        json_object["merkleroot"] = libbitcoin::encode_hash(block->header().merkle());

                        int i = 0;
                        for (const auto & txns : txs) {
                            json_object["tx"][i] = libbitcoin::encode_hash(txns);
                            ++i;
                        }

                        json_object["time"] = block->header().timestamp();
                        // TODO: get real median time
                        json_object["mediantime"] = block->header().timestamp();
                        json_object["nonce"] = block->header().nonce();
                        // TODO: encode bits to base 16
                        json_object["bits"] = block->header().bits();
                        json_object["difficulty"] = bits_to_difficulty(block->header().bits());
                        // TODO: validate that proof is chainwork
                        // Optimizate the encoded to base 16
                        std::stringstream ss;
                        ss << std::setfill('0')
                            << std::nouppercase
                            << std::hex
                            << block->proof();
                        json_object["chainwork"] = ss.str();
                        json_object["previousblockhash"] = libbitcoin::encode_hash(block->header().previous_block_hash());

                        json_object["nextblockhash"];

                        libbitcoin::hash_digest nexthash;
                        if(chain.get_block_hash(nexthash, height+1))
                            json_object["nextblockhash"] = libbitcoin::encode_hash(nexthash);
                    
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
    } else {
        boost::latch latch(2);
        chain.fetch_block(hash, [&](const libbitcoin::code &ec, libbitcoin::block_const_ptr block, size_t height) {
            if (ec == libbitcoin::error::success) {
                json_object = libbitcoin::encode_base16(block->to_data(0));
            } else {
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
nlohmann::json process_getblock(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{

    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string hash;
    bool verbose;
    if (!json_in_getblock(json_in, hash, verbose)) //if false return error
    {
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "getblock \"blockhash\" ( verbose )\n"
            "\nIf verbose is false, returns a string that is serialized, "
            "hex-encoded data for block 'hash'.\n"
            "If verbose is true, returns an Object with information about "
            "block <hash>.\n"
            "\nArguments:\n"
            "1. \"blockhash\"          (string, required) The block hash\n"
            "2. verbose                (boolean, optional, default=true) true "
            "for a json object, false for the hex encoded data\n"
            "\nResult (for verbose = true):\n"
            "{\n"
            "  \"hash\" : \"hash\",     (string) the block hash (same as "
            "provided)\n"
            "  \"confirmations\" : n,   (numeric) The number of confirmations, "
            "or -1 if the block is not on the main chain\n"
            "  \"size\" : n,            (numeric) The block size\n"
            "  \"height\" : n,          (numeric) The block height or index\n"
            "  \"version\" : n,         (numeric) The block version\n"
            "  \"versionHex\" : \"00000000\", (string) The block version "
            "formatted in hexadecimal\n"
            "  \"merkleroot\" : \"xxxx\", (string) The merkle root\n"
            "  \"tx\" : [               (array of string) The transaction ids\n"
            "     \"transactionid\"     (string) The transaction id\n"
            "     ,...\n"
            "  ],\n"
            "  \"time\" : ttt,          (numeric) The block time in seconds "
            "since epoch (Jan 1 1970 GMT)\n"
            "  \"mediantime\" : ttt,    (numeric) The median block time in "
            "seconds since epoch (Jan 1 1970 GMT)\n"
            "  \"nonce\" : n,           (numeric) The nonce\n"
            "  \"bits\" : \"1d00ffff\", (string) The bits\n"
            "  \"difficulty\" : x.xxx,  (numeric) The difficulty\n"
            "  \"chainwork\" : \"xxxx\",  (string) Expected number of hashes "
            "required to produce the chain up to this block (in hex)\n"
            "  \"previousblockhash\" : \"hash\",  (string) The hash of the "
            "previous block\n"
            "  \"nextblockhash\" : \"hash\"       (string) The hash of the "
            "next block\n"
            "}\n"
            "\nResult (for verbose=false):\n"
            "\"data\"             (string) A string that is serialized, "
            "hex-encoded data for block 'hash'.\n";
        return container;
    }

    if (getblock(result, error, error_code, hash, verbose, chain))
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

#endif //BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCK_HPP_

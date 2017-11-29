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

#include <bitprim/rpc/messages/blockchain/getblock.hpp>
#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool json_in_getblock(nlohmann::json const& json_object, std::string & hash, bool & verbose) {
    if (json_object["params"].size() == 0)
        return false;
    verbose = true;
    hash = json_object["params"][0];
    if (json_object["params"].size() == 2) {
        verbose = json_object["params"][1];
    }
    return true;
}

bool getblock(nlohmann::json& json_object, int& error, std::string& error_code, const std::string & block_hash, bool verbose, libbitcoin::blockchain::block_chain const& chain)
{
    libbitcoin::hash_digest hash;
    if(libbitcoin::decode_hash(hash, block_hash)){
        boost::latch latch(2);
        chain.fetch_block(hash, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block, size_t height){
            if (ec == libbitcoin::error::success) {
                if (!verbose) {
                    json_object = libbitcoin::encode_base16(block->to_data(0));
                } else {
                    json_object["hash"] = libbitcoin::encode_hash(block->hash());

                    size_t top_height;
                    chain.get_last_height(top_height);
                    json_object["confirmations"] = top_height - height + 1;

                    json_object["size"] = block->serialized_size(0);
                    json_object["height"] = height;
                    json_object["version"] = block->header().version();
                    // TODO: encode the version to base 16
                    json_object["versionHex"] = block->header().version();
                    json_object["merkleroot"] = libbitcoin::encode_hash(block->header().merkle());

                    int i = 0;
                    for (const auto & txns : block->transactions()) {
                        json_object["tx"][i] = libbitcoin::encode_hash(txns.hash());
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

                    boost::latch latch2(2);
                    chain.fetch_block(height + 1, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block_2, size_t) {
                        if (ec == libbitcoin::error::success) {
                            json_object["nextblockhash"] = libbitcoin::encode_hash(block_2->header().hash());
                        }
                        latch2.count_down();
                    });
                    latch2.count_down_and_wait();
                }
            } else {
                if(ec == libbitcoin::error::not_found)
                {
                    error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
                    error_code = "Block not found";
                } else {
                    error = bitprim::RPC_INTERNAL_ERROR;
                    error_code = "Can't read block from disk";
                } 
            }
            latch.count_down();
        });
        latch.count_down_and_wait();

    } else {
        error = bitprim::RPC_INVALID_PARAMETER;
        error_code = "Invalid block hash";
    }

    if(error !=0)
        return false;
    return true;

}

nlohmann::json process_getblock(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string hash;
    bool verbose;
    if (!json_in_getblock(json_in, hash, verbose)) //if false return error
    {
        //load error code
        //return
    }

    if (getblock(result, error, error_code, hash, verbose, chain))
    {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

}

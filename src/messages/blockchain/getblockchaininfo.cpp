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

#include <bitprim/rpc/messages/blockchain/getblockchaininfo.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool getblockchaininfo(nlohmann::json& json_object, int& error, std::string& error_code, libbitcoin::blockchain::block_chain const& chain)
{
    json_object["chain"] = "main";
    
    size_t top_height;
    libbitcoin::message::header::ptr top;
    chain.get_last_height(top_height);

    boost::latch latch(2);
    chain.fetch_block(top_height, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block, size_t height){
        if (ec == libbitcoin::error::success) {
                json_object["blocks"] = height;
                json_object["headers"] = height;
                json_object["bestblockhash"] = libbitcoin::encode_hash(block->hash());
                json_object["difficulty"] = bits_to_difficulty(block->header().bits());
                json_object["mediantime"] = block->header().timestamp(); //TODO Get medianpasttime
                json_object["verificationprogress"] = 1;
                std::stringstream ss;
                ss << std::setfill('0')
                   << std::nouppercase
                   << std::hex
                   << block->proof();
                json_object["chainwork"] = ss.str();
                json_object["pruned"] = false;
                json_object["pruneheight"] = 0;
                json_object["softforks"] = nlohmann::json::array(); //TODO Check softforks
                json_object["bip9_softforks"] = nlohmann::json::array(); //TODO Check softforks
            
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
    return true;
}

nlohmann::json process_getblockchaininfo(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules )
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    if (getblockchaininfo(result, error, error_code, chain))
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

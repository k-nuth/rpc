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
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {


bool json_in_getblockhash(nlohmann::json const& json_object, size_t& height){
    if (json_object["params"].size() == 0)
        return false;
    height = json_object["params"][0];
    return true;
}

bool getblockhash(nlohmann::json& json_object, int& error, std::string& error_code, const size_t height, libbitcoin::blockchain::block_chain const& chain)
{
    libbitcoin::message::header::ptr header;
    getblockheader(height, header, chain);
    json_object = libbitcoin::encode_hash(header->hash());
    return true;
}


nlohmann::json process_getblockhash(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error;
    std::string error_code;

    size_t height;
    if (!json_in_getblockhash(json_in, height)) //if false return error
    {
        //load error code
        //return
    }

    if (getblockhash(result, error, error_code, height, chain))
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

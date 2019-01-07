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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETHEADER_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETHEADER_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

inline
bool json_in_getheader(nlohmann::json const& json_object, size_t & height) {
    if (json_object["params"].size() == 0)
        return false;
    try {
        height = json_object["params"][0];
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;
}

    template <typename Blockchain>
    bool getheader(nlohmann::json& json_object, int& error, std::string& error_code, Blockchain const& chain, size_t const& requested_height)
    {
        json_object["chain"] = "main";

        size_t top_height;
        libbitcoin::message::header::ptr top;

        boost::latch latch(2);
        
        chain.fetch_block_header(requested_height, [&](const libbitcoin::code &ec, libbitcoin::header_ptr header, size_t height) {
            if (ec == libbitcoin::error::success) {
                json_object["blocks"] = height;
                json_object["height"] = height;
                json_object["hash"] = libbitcoin::encode_hash(header->hash());
                json_object["difficulty"] = bits_to_difficulty(header->bits());
                json_object["mediantime"] = header->timestamp(); //TODO Get medianpasttime
		json_object["time"] = header->timestamp(); //TODO Get medianpasttime
                json_object["verificationprogress"] = 1;
                std::stringstream ss;
                ss << std::setfill('0')
                    << std::nouppercase
                    << std::hex
                    << header->proof();
                json_object["chainwork"] = ss.str();
                json_object["pruned"] = false;
                json_object["pruneheight"] = 0;
                json_object["softforks"] = nlohmann::json::array(); //TODO Check softforks
                json_object["bip9_softforks"] = nlohmann::json::array(); //TODO Check softforks
		json_object["previousblockhash"] = libbitcoin::encode_hash(header->previous_block_hash());

            } else {
                // The block doesn't exist
                json_object["blocks"] = 0;
            }
            latch.count_down();
        });
        latch.count_down_and_wait();
        return true;
    }

    template <typename Blockchain>
    nlohmann::json process_getheader(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
    {
        nlohmann::json container, result;
        container["id"] = json_in["id"];

        int error = 0;
        std::string error_code;

        size_t height;
        if (!json_in_getheader(json_in, height)) //if false return error
        {
            container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
            container["error"]["message"] = "getheader \"height\"\n";
            return container;
        }

        if (getheader(result, error, error_code, chain, height))
        {
            if (result["blocks"] == 0) {
                container["result"] = result;
                container["error"]["code"] = 0;
                container["error"]["message"] = "The height is not in the chain";
            }else {
                container["result"] = result;
                container["error"];
            }
        }
        else {
            container["error"]["code"] = error;
            container["error"]["message"] = error_code;
        }

        return container;
    }

}

#endif

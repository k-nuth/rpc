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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSTXIDS_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSTXIDS_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

namespace bitprim {

    bool json_in_getaddresstxids(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height);
    bool getaddresstxids (nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, size_t const& start_height, size_t const& end_height, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json process_getaddresstxids(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules = false);

}

#endif
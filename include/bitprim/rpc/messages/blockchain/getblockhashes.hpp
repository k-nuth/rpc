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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASHES_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASHES_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

//getrawtransaction
namespace bitprim {

    bool json_in_getblockhashes(nlohmann::json const& json_object, uint32_t& time_high, uint32_t& time_low, bool& no_orphans, bool& logical_times);
    bool getblockhashes(nlohmann::json& json_object, int& error, std::string& error_code, uint32_t time_high, uint32_t time_low, bool no_orphans, bool logical_times, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json process_getblockhashes(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules = false);

}

#endif

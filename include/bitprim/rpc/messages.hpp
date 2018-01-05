/**
 * Copyright (c) 2017 Bitprim developers (see AUTHORS)
 *
 * This file is part of Bitprim.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BITPRIM_RPC_MESSAGES_HPP_
#define BITPRIM_RPC_MESSAGES_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

namespace bitprim {

using message_signature = nlohmann::json(*)(nlohmann::json const&, libbitcoin::blockchain::block_chain const&, bool);
using signature_map = std::unordered_map<std::string, message_signature>;
std::string process_data(nlohmann::json const& json_object, bool use_testnet_rules, libbitcoin::blockchain::block_chain& chain, signature_map const& signature_map);
signature_map load_signature_map();

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_HPP_

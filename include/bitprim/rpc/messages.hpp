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
#include <bitprim/rpc/messages/messages.hpp>

namespace bitprim {


template <typename Blockchain>
using message_signature = nlohmann::json(*)(nlohmann::json const&, Blockchain const&, bool);

template <typename Blockchain>
using signature_map = std::unordered_map<std::string, message_signature<Blockchain>>;


template <typename Blockchain> 
std::string process_data(nlohmann::json const& json_object, bool use_testnet_rules, Blockchain& chain, signature_map<Blockchain> const& signature_map);

// std::string process_data(nlohmann::json const& json_object, bool use_testnet_rules, libbitcoin::blockchain::block_chain& chain, signature_map const& signature_map);


template <typename Blockchain>
signature_map<Blockchain> load_signature_map() {

	return signature_map<Blockchain>  {
		{"getrawtransaction", process_getrawtransaction}
	};

/*
	return signature_map<Blockchain>  {
		{"getrawtransaction", process_getrawtransaction},
		{ "getaddressbalance", process_getaddressbalance },
		{ "getspentinfo", process_getspentinfo },
		{ "getaddresstxids", process_getaddresstxids },
		{ "getaddressdeltas", process_getaddressdeltas },
		{ "getaddressutxos", process_getaddressutxos },
		{ "getblockhashes", process_getblockhashes },
		{ "getinfo", process_getinfo },
		{ "getaddressmempool", process_getaddressmempool },
		{ "getbestblockhash", process_getbestblockhash },
		{ "getblock", process_getblock },
		{ "getblockhash", process_getblockhash },
		{ "getblockchaininfo", process_getblockchaininfo },
		{ "getblockheader", process_getblockheader },
		{ "getblockcount", process_getblockcount },
		{ "getdifficulty", process_getdifficulty },
		{ "getchaintips", process_getchaintips },
		{ "validateaddress", process_validateaddress },
		{ "getblocktemplate", process_getblocktemplate },
		{ "getmininginfo", process_getmininginfo }
	};*/
}

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_HPP_

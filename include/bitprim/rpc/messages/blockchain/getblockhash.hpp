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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASH_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASH_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

	bool json_in_getblockhash(nlohmann::json const& json_object, size_t& height) {
		if (json_object["params"].size() == 0)
			return false;
		try {
			height = json_object["params"][0].get<size_t>();
		}
		catch (const std::exception & e) {
			return false;
		}

		return true;
	}

	template <typename Blockchain>
	bool getblockhash(nlohmann::json& json_object, int& error, std::string& error_code, const size_t height, Blockchain const& chain)
	{
		libbitcoin::message::header::ptr header;
		if (getblockheader(height, header, chain) != libbitcoin::error::success) {
			error_code = "Block height out of range";
			error = -8;
			return false;
		}
		json_object = libbitcoin::encode_hash(header->hash());
		return true;

	}

	template <typename Blockchain>
	nlohmann::json process_getblockhash(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
	{
		nlohmann::json container, result;
		container["id"] = json_in["id"];

		int error = 0;
		std::string error_code;

		size_t height;
		if (!json_in_getblockhash(json_in, height)) //if false return error
		{
			container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
			container["error"]["message"] = "getblockhash height\n"
				"\nReturns hash of block in best-block-chain at height provided.\n"
				"\nArguments:\n"
				"1. height         (numeric, required) The height index\n"
				"\nResult:\n"
				"\"hash\"         (string) The block hash\n";
			return container;
		}

		if (getblockhash(result, error, error_code, height, chain))
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

}
#endif

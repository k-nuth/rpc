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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBESTBLOCKHASH_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBESTBLOCKHASH_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

	template <typename Blockchain>
	bool getbestblockhash(nlohmann::json& json_object, int& error, std::string& error_code, Blockchain const& chain)
	{
		size_t top_height;
		libbitcoin::message::header::ptr top;
		chain.get_last_height(top_height);
		getblockheader(top_height, top, chain);

		json_object = libbitcoin::encode_hash(top->hash());
		return true;
	}

	template <typename Blockchain>
	nlohmann::json process_getbestblockhash(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
	{
		nlohmann::json container, result;
		container["id"] = json_in["id"];

		int error = 0;
		std::string error_code;

		if (getbestblockhash(result, error, error_code, chain))
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

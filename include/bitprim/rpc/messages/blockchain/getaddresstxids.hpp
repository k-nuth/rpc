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

#include <bitprim/rpc/messages/error_codes.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

	bool json_in_getaddresstxids(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height) {
		if (json_object["params"].size() == 0)
			return false;

		start_height = 0;
		end_height = libbitcoin::max_size_t;
		try {
			auto temp = json_object["params"][0];
			if (temp.is_object()) {
				if (!temp["start"].is_null()) {
					start_height = temp["start"];
				}
				if (!temp["end"].is_null()) {
					end_height = temp["end"];
				}

				for (const auto & addr : temp["addresses"]) {
					payment_address.push_back(addr);
				}
			}
			else {
				//Only one address:
				payment_address.push_back(json_object["params"][0].get<std::string>());
			}
		}
		catch (const std::exception & e) {
			return false;
		}
		return true;
	}

	template <typename Blockchain>
	bool getaddresstxids(nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, size_t const& start_height, size_t const& end_height, Blockchain const& chain)
	{
		int i = 0;
		for (const auto & payment_address : payment_addresses) {
			libbitcoin::wallet::payment_address address(payment_address);
			if (address)
			{
				boost::latch latch(2);
				chain.fetch_txns(address, INT_MAX, start_height,
					[&](const libbitcoin::code &ec, const std::vector<libbitcoin::hash_digest>& history_list) {
					if (ec == libbitcoin::error::success) {
						// TODO: remove this if the new code pass the tests
						//                                    for (const auto & history : history_list) {
						//                                        libbitcoin::hash_digest temp;
						//                                        if (history.height < end_height) {
						//                                            // If a txns spend more than one utxo from the address, just add the first one
						//                                            temp = history.point.hash();
						//                                            if (std::find (unique_data.begin(), unique_data.end(), history.point.hash()) == unique_data.end()){
						//                                                unique_data.push_back(temp);
						//                                            }
						//                                        }
						//                                    }
						for (auto it = history_list.rbegin(); it != history_list.rend(); ++it) {
							json_object[i] = libbitcoin::encode_hash(*it);
							++i;
						}
					}
					else
					{
						error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
						error_code = "No information available for address " + address;
					}
					latch.count_down();
				});
				latch.count_down_and_wait();
			}
			else {
				error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
				error_code = "Invalid address";
			}
		}
		if (error != 0)
			return false;

		return true;
	}

	template <typename Blockchain>
	nlohmann::json process_getaddresstxids(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
	{
		nlohmann::json container;
		nlohmann::json result = nlohmann::json::array();

		container["id"] = json_in["id"];

		int error = 0;
		std::string error_code;

		std::vector<std::string> payment_address;
		size_t start_height;
		size_t end_height;
		if (!json_in_getaddresstxids(json_in, payment_address, start_height, end_height))
		{
			container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
			container["error"]["message"] = "getaddresstxids\n"
				"\nReturns the txids for an address(es) (requires addressindex to be enabled).\n"
				"\nArguments:\n"
				"{\n"
				"  \"addresses\"\n"
				"    [\n"
				"      \"address\"  (string) The base58check encoded address\n"
				"      ,...\n"
				"    ]\n"
				"  \"start\" (number) The start block height\n"
				"  \"end\" (number) The end block height\n"
				"}\n"
				"\nResult:\n"
				"[\n"
				"  \"transactionid\"  (string) The transaction id\n"
				"  ,...\n"
				"]\n";
			return container;
		}

		if (getaddresstxids(result, error, error_code, payment_address, start_height, end_height, chain))
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

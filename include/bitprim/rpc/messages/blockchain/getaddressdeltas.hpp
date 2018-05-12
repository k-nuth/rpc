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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSDELTAS_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSDELTAS_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

inline
bool json_in_getaddressdeltas(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height, bool& include_chain_info)
{
    // Example:
    // curl --user bitcoin:local321 --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getaddressdeltas", "params": [{"addresses": ["mvBZZ2s7yCQmRN7FoYkHDXdXpWqtFydXUg"], "start":1, "end":1192283}] }' -H 'content-type: text/plain;' http://127.0.0.1:8332/

    if (json_object["params"].size() == 0)
        return false;

    start_height = 0;
    end_height = libbitcoin::max_size_t;
    include_chain_info = false;
    try {
        auto temp = json_object["params"][0];
        if (temp.is_object()) {
            if (!temp["start"].is_null()) {
                start_height = temp["start"].get<size_t>();
            }
            if (!temp["end"].is_null()) {
                end_height = temp["end"].get<size_t>();
            }
            if (!temp["chainInfo"].is_null()) {
                include_chain_info = temp["chainInfo"].get<bool>();
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
bool getaddressdeltas(nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, size_t const& start_height, size_t const& end_height, const bool include_chain_info, Blockchain const& chain)
{
#ifdef BITPRIM_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
#endif

    int i = 0;
    for (const auto & payment_address : payment_addresses) {
        libbitcoin::wallet::payment_address address(payment_address);
        if (address)
        {
            boost::latch latch(2);
            chain.fetch_history(address, INT_MAX, 0, [&](const libbitcoin::code &ec,
                libbitcoin::chain::history_compact::list history_compact_list) {
                if (ec == libbitcoin::error::success) {
                    for (const auto & history : history_compact_list) {
                        if (history.kind == libbitcoin::chain::point_kind::output &&
                            history.height >= start_height && history.height <= end_height) {
                            //It's an output
                            boost::latch latch2(2);
                            //Fetch txn to get the blockindex and height
                            chain.fetch_transaction(history.point.hash(), false, witness,
                                [&](const libbitcoin::code &ec,
                                    libbitcoin::transaction_const_ptr tx_ptr, size_t index,
                                    size_t height) {
                                if (ec == libbitcoin::error::success) {
                                    if (height >= start_height && height <= end_height) {
                                        json_object[i]["txid"] = libbitcoin::encode_hash(
                                            history.point.hash());
                                        json_object[i]["index"] = history.point.index();
                                        json_object[i]["address"] = address.encoded();
                                        json_object[i]["blockindex"] = index;
                                        json_object[i]["height"] = height;
                                        json_object[i]["satoshis"] = std::to_string(history.value);
                                        ++i;
                                    }
                                }
                                else {
                                    error = bitprim::RPC_DATABASE_ERROR;
                                    error_code = "Error fetching transaction.";
                                }
                                latch2.count_down();
                            });
                            latch2.count_down_and_wait();

                            //Check if it was spent and get the txn data
                            boost::latch latch3(2);
                            chain.fetch_spend(history.point, [&](const libbitcoin::code &ec,
                                libbitcoin::chain::input_point input) {
                                if (ec == libbitcoin::error::success) {
                                    boost::latch latch4(2);
                                    chain.fetch_transaction(input.hash(), false, witness,
                                        [&](const libbitcoin::code &ec,
                                            libbitcoin::transaction_const_ptr tx_ptr,
                                            size_t index,
                                            size_t height) {
                                        if (ec == libbitcoin::error::success) {
                                            if (height >= start_height &&
                                                height <= end_height) {
                                                json_object[i]["txid"] = libbitcoin::encode_hash(
                                                    input.hash());
                                                json_object[i]["index"] = input.index();
                                                json_object[i]["address"] = address.encoded();
                                                json_object[i]["blockindex"] = index;
                                                json_object[i]["height"] = height;
                                                json_object[i]["satoshis"] =
                                                    "-" + std::to_string(history.value);
                                                ++i;
                                            }
                                        }
                                        else {
                                            error = bitprim::RPC_DATABASE_ERROR;
                                            error_code = "Error fetching transaction.";
                                        }
                                        latch4.count_down();
                                    });
                                    latch4.count_down_and_wait();
                                }
                                latch3.count_down();
                            });
                            latch3.count_down_and_wait();
                        }
                    }
                }
                else {
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
nlohmann::json process_getaddressdeltas(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{
    nlohmann::json container;
    nlohmann::json result = nlohmann::json::array();

    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<std::string> payment_address;
    size_t start_height;
    size_t end_height;
    bool include_chain_info;
    if (!json_in_getaddressdeltas(json_in, payment_address, start_height, end_height, include_chain_info))
    {
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "getaddressdeltas\n"
            "\nReturns all changes for an address (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ]\n"
            "  \"start\" (number) The start block height\n"
            "  \"end\" (number) The end block height\n"
            "  \"chainInfo\" (boolean) Include chain info in results, only applies if start and end specified\n"
            "}\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"satoshis\"  (number) The difference of satoshis\n"
            "    \"txid\"  (string) The related txid\n"
            "    \"index\"  (number) The related input or output index\n"
            "    \"height\"  (number) The block height\n"
            "    \"address\"  (string) The base58check encoded address\n"
            "  }\n"
            "]\n";
        return container;
    }

    if (getaddressdeltas(result, error, error_code, payment_address, start_height, end_height, include_chain_info, chain))
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

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSDELTAS_HPP_

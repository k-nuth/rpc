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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSBALANCE_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSBALANCE_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

inline
bool json_in_getaddressbalance(nlohmann::json const& json_object, std::vector<std::string>& address)
{
    if (json_object["params"].size() == 0) {
        return false;
    }
    else {
        try {
            auto temp = json_object["params"][0];
            if (temp.is_object()) {
                for (const auto & addr : temp["addresses"]) {
                    address.push_back(addr);
                }
            }
            else {
                //Only one address:
                address.push_back(json_object["params"][0].get<std::string>());
            }
        }
        catch (const std::exception & e) {
            return false;
        }
    }
    return true;
}

template <typename Blockchain>
bool getaddressbalance(nlohmann::json& json_result, int& error, std::string& error_code, std::vector<std::string> const& addresses, Blockchain const& chain)
{
    uint64_t balance = 0;
    uint64_t received = 0;
    for (const auto & address : addresses) {
        libbitcoin::wallet::payment_address payment_address(address);
        if (payment_address)
        {
            boost::latch latch(2);
            chain.fetch_history(payment_address, INT_MAX, 0, [&](const libbitcoin::code &ec, libbitcoin::chain::history_compact::list history_compact_list) {
                if (ec == libbitcoin::error::success) {
                    for (const auto & history : history_compact_list) {
                        if (history.kind == libbitcoin::chain::point_kind::output) {
                            received += history.value;
                            boost::latch latch2(2);
                            chain.fetch_spend(history.point, [&](const libbitcoin::code &ec, libbitcoin::chain::input_point input) {
                                if (ec == libbitcoin::error::not_found) {
                                    // Output not spent
                                    balance += history.value;
                                }
                                latch2.count_down();
                            });
                            latch2.count_down_and_wait();
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
        else
        {
            error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
            error_code = "Invalid address";
        }
    }
    if (error != 0)
        return false;

    json_result["balance"] = balance;
    json_result["received"] = received;
    return true;
}

template <typename Blockchain>
nlohmann::json process_getaddressbalance(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<std::string> payment_addresses;
    if (!json_in_getaddressbalance(json_in, payment_addresses)) //if false return error
    {
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "getaddressbalance\n"
            "\nReturns the balance for an address(es) (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ]\n"
            "}\n"
            "\nResult:\n"
            "{\n"
            "  \"balance\"  (string) The current balance in satoshis\n"
            "  \"received\"  (string) The total number of satoshis received (including change)\n"
            "}\n";
        return container;
    }

    if (getaddressbalance(result, error, error_code, payment_addresses, chain))
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

#endif //BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSBALANCE_HPP_

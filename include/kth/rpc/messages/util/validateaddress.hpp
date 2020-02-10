/**
* Copyright (c) 2016-2020 Knuth Project developers.
*
* This file is part of kth-node.
*
* kth-node is free software: you can redistribute it and/or
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

#ifndef KTH_RPC_MESSAGES_UTIL_VALIDATEADDRESS_HPP_
#define KTH_RPC_MESSAGES_UTIL_VALIDATEADDRESS_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>


namespace kth {

inline
bool json_in_validateaddress(nlohmann::json const& json_object, std::string& raw_address) {
    if (json_object["params"].size() == 0)
        return false;
    try {
        raw_address = json_object["params"][0];
    } catch (const std::exception & e) {
        return false;
    }
    return true;
}

template <typename Blockchain>
bool validateaddress(nlohmann::json& json_object, int& error, std::string& error_code, std::string& raw_address, Blockchain const& chain) {
    kth::wallet::payment_address payment_address(raw_address);

    int ver = (int)payment_address.version();
    if (ver == 111 /*testnet*/ || ver == 0 /*mainnet btc*/ || ver == 48 /*mainnet ltc*/) {
        //PAY TO PUBLIC KEY HASH
        json_object["isvalid"] = true;
        json_object["address"] = raw_address;
        json_object["scriptPubKey"] = "76a914" + kth::encode_base16(payment_address.hash()) + "88ac";
    } else if (ver == 196 /*testnet*/ || ver == 5 /*mainnet btc*/ || ver == 5 /*mainnet ltc*/) {
        //PAY TO SCRIPT HASH
        json_object["isvalid"] = true;
        json_object["address"] = raw_address;
        json_object["scriptPubKey"] = "a914" + kth::encode_base16(payment_address.hash()) + "87";
    } else {
        //TODO: VALIDATE WIF
        json_object["isvalid"] = false;
        json_object["address"];
        json_object["scriptPubKey"];
    }

    return true;
}

template <typename Blockchain>
nlohmann::json process_validateaddress(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string raw_address;
    if ( ! json_in_validateaddress(json_in, raw_address)) { //if false return error
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "validateaddress \"address\"\n"
            "\nReturn information about the given bitcoin address.\n"
            "\nArguments:\n"
            "1. \"address\"     (string, required) The bitcoin address to "
            "validate\n"
            "\nResult:\n"
            "{\n"
            "  \"isvalid\" : true|false,       (boolean) If the address is "
            "valid or not. If not, this is the only property returned.\n"
            "  \"address\" : \"address\", (string) The bitcoin address "
            "validated\n"
            "  \"scriptPubKey\" : \"hex\",       (string) The hex encoded "
            "scriptPubKey generated by the address\n"
            "}\n";
        return container;
    }

    if (validateaddress(result, error, error_code, raw_address, chain)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_UTIL_VALIDATEADDRESS_HPP_
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

#ifndef BITPRIM_RPC_MESSAGES_KEOKEN_GETKEOKENADDRESS_HPP_
#define BITPRIM_RPC_MESSAGES_KEOKEN_GETKEOKENADDRESS_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <bitprim/rpc/messages/keoken/keokenutils.hpp>

namespace bitprim {

inline
bool json_in_getkeokenaddress(nlohmann::json const& json_object, std::string& payment_address, size_t& block_init ,size_t& index_start, size_t& index_end)
{
    // Example:
    // curl --user bitcoin:local321 --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getaddressdeltas", "params": [{"addresses": ["mvBZZ2s7yCQmRN7FoYkHDXdXpWqtFydXUg"], "start":1, "end":1192283}] }' -H 'content-type: text/plain;' http://127.0.0.1:8332/

    if (json_object["params"].size() == 0)
        return false;

    index_start = 0;
    index_end = libbitcoin::max_size_t;
    block_init = 0;
    try {
        auto temp = json_object["params"];
        if (temp.is_object()) {
            if (!temp["index_start"].is_null()) {
                index_start = temp["index_start"].get<size_t>();
            }
            if (!temp["index_end"].is_null()) {
                index_end = temp["index_end"].get<size_t>();
            }
            if (!temp["block_init"].is_null()) {
                block_init = temp["block_init"].get<size_t>();
            }
            if (!temp["address"].is_null()) {
                payment_address = temp["address"].get<std::string>();
            }
        }
    }
    catch (const std::exception & e) {
        return false;
    }

    return true;
}

template <typename Blockchain>
bool getkeokenaddress(nlohmann::json& json_object, int& error, std::string& error_code, std::string const& payment_address, size_t& block_init ,size_t& index_start, size_t& index_end, Blockchain const& chain)
{
#ifdef BITPRIM_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
#endif

    int i = 0;
    libbitcoin::wallet::payment_address address(payment_address);
    if (address)
    {
        boost::latch latch(2);
        chain.fetch_keoken_history(address, INT_MAX, 0, [&](const libbitcoin::code &ec,
            std::vector<libbitcoin::transaction_const_ptr>& keoken_txs) {
                if (ec == libbitcoin::error::success) {
                    //TODO PAGINADO
                    int i = 0;
                    for(auto& tx : keoken_txs){
                        json_object[i] = bitprim::decode_keoken(chain, tx);
                        ++i;
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

    if (error != 0)
        return false;

    return true;
}

template <typename Blockchain>
nlohmann::json process_getkeokenaddress(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{
    nlohmann::json container;
    nlohmann::json result = nlohmann::json::array();

    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string payment_address;
    size_t index_start;
    size_t index_end;
    size_t first_block_height;
    if (!json_in_getkeokenaddress(json_in, payment_address, first_block_height, index_start, index_end))
    {
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (getkeokenaddress(result, error, error_code, payment_address, first_block_height, index_start, index_end, chain))
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

#endif //BITPRIM_RPC_MESSAGES_KEOKEN_GETKEOKENADDRESS_HPP_

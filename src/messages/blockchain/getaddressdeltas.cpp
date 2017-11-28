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

#include <bitprim/rpc/messages/blockchain/getaddressdeltas.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool json_in_getaddressdeltas(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height, bool& include_chain_info)
{
    if (json_object["params"].size() == 0)
        return false;

    start_height = 0;
    end_height = libbitcoin::max_size_t;
    include_chain_info = false;

    auto temp = json_object["params"][0];
    if (temp.is_object()){
        if (!temp["start"].is_null()){
            start_height = temp["start"][0];
        }
        if (!temp["end"].is_null()){
            end_height = temp["end"][0];
        }
        if(!temp["chainInfo"].is_null()){
            include_chain_info = temp["chainInfo"][0];
        }

        for (const auto & addr : temp["addresses"]){
            payment_address.push_back(addr);
        }
    } else {
        //Only one address:
        payment_address.push_back(json_object["params"][0]);
    }
    return true;
}

bool getaddressdeltas (nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, size_t const& start_height, size_t const& end_height, const bool include_chain_info, libbitcoin::blockchain::block_chain const& chain)
{
    int i = 0;
    for(const auto & payment_address : payment_addresses) {
        libbitcoin::wallet::payment_address address(payment_address);
        boost::latch latch(2);
        chain.fetch_history(address, INT_MAX, 0, [&](const libbitcoin::code &ec,
                                                     libbitcoin::chain::history_compact::list history_compact_list) {
            if (ec == libbitcoin::error::success) {
                auto const history_list = expand(history_compact_list);
                for (const auto & history : history_list) {
                    if (history.spend.hash() == libbitcoin::null_hash && history.output_height >= start_height &&
                        history.output_height <= end_height) {
                        //Es outpoint
                        // Need to look for the txns because the position the block is required
                        //                                    json_object[i]["txid"] = history.output.hash();
                        //                                    json_object[i]["index"] = history.output.index();
                        //                                    json_object[i]["address"] = address.encoded();
                        //                                    json_object[i]["height"] = history.spend_height;
                        //                                    json_object[i]["satoshis"] = history.value;
                        //                                    ++i;
                        {
                            boost::latch latch2(2);
                            chain.fetch_transaction(history.output.hash(), false,
                                                    [&](const libbitcoin::code &ec,
                                                        libbitcoin::transaction_const_ptr tx_ptr, size_t index,
                                                        size_t height) {
                                                        if (ec == libbitcoin::error::success) {
                                                            if (height >= start_height && height <= end_height) {
                                                                json_object[i]["txid"] = libbitcoin::encode_hash(
                                                                        history.output.hash());
                                                                json_object[i]["index"] = history.output.index();
                                                                json_object[i]["address"] = address.encoded();
                                                                json_object[i]["blockindex"] = index;
                                                                json_object[i]["height"] = height;
                                                                json_object[i]["satoshis"] = std::to_string(
                                                                        history.value);
                                                                ++i;
                                                            }
                                                        } else {
                                                            std::cout << "Tx not found" << std::endl;
                                                        }
                                                        latch2.count_down();
                                                    });
                            latch2.count_down_and_wait();
                        }

                        // Check if it was spent
                        {
                            boost::latch latch2(2);
                            chain.fetch_spend(history.output, [&](const libbitcoin::code &ec,
                                                                  libbitcoin::chain::input_point input) {
                                if (ec == libbitcoin::error::success) {
                                    // TODO check height without calling fetch_transaction
                                    boost::latch latch3(2);
                                    chain.fetch_transaction(input.hash(), false,
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
                                                                    } else {
                                                                        // Do nothing out of limits
                                                                        std::cout << "\tInput > height"
                                                                                  << std::endl;
                                                                    }
                                                                } else {
                                                                    std::cout << "Tx not found" << std::endl;
                                                                }
                                                                latch3.count_down();
                                                            });
                                    latch3.count_down_and_wait();
                                }
                                latch2.count_down();
                            });
                            latch2.count_down_and_wait();
                        }
                    }
                }
            } else {
                std::cout << "No Encontrado" << std::endl;
            }
            latch.count_down();
        });
        latch.count_down_and_wait();
    }
    return true;
}

nlohmann::json process_getaddressdeltas(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error;
    std::string error_code;

    std::vector<std::string> payment_address;
    size_t start_height;
    size_t end_height;
    bool include_chain_info;
    if (!json_in_getaddressdeltas(json_in, payment_address, start_height, end_height, include_chain_info)) 
    {
        //load error code
        //return
    }

    if (getaddressdeltas(result, error, error_code, payment_address, start_height, end_height, include_chain_info, chain))
    {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

}

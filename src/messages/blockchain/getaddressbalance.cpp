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

#include <bitprim/rpc/messages/blockchain/getaddressbalance.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool json_in_getaddressbalance(nlohmann::json const& json_object, std::vector<std::string>& address)
{
    if (json_object["params"].size() == 0)
        return false;
    else {
        auto temp = json_object["params"][0];
        if (temp.is_object()){
            for (const auto & addr : temp["addresses"]){
                std::cout << addr << std::endl;
                address.push_back(addr);
            }
        } else {
            //Only one address:
            address.push_back(json_object["params"][0]);
        }
    }
    return true;
}

bool getaddressbalance (nlohmann::json& json_result, int& error, std::string& error_code, std::vector<std::string> const& addresses, libbitcoin::blockchain::block_chain const& chain)
{
    uint64_t balance = 0;
    uint64_t received = 0;
    for(const auto & address : addresses){
        libbitcoin::wallet::payment_address payment_address(address);
        boost::latch latch(2);
        chain.fetch_history(payment_address, INT_MAX, 0, [&](const libbitcoin::code &ec, libbitcoin::chain::history_compact::list history_compact_list) {
            if (ec == libbitcoin::error::success) {
                for(const auto & history : history_compact_list) {
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
                std::cout << "No Encontrado" << std::endl;
            }
            latch.count_down();
        });
        latch.count_down_and_wait();
    }
    if(error != 0)
        return false;

    json_result["balance"] = balance;
    json_result["received"] = received;
    return true;
}

nlohmann::json process_getaddressbalance(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<std::string> payment_addresses;
    if(!json_in_getaddressbalance(json_in, payment_addresses)) //if false return error
    {
        //load error code
        //return
    }

    if(getaddressbalance(result, error, error_code, payment_addresses, chain))
    {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}


} //namespace bitprim

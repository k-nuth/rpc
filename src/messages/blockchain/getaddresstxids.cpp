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

#include <bitprim/rpc/messages/blockchain/getaddresstxids.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool json_in_getaddresstxids(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height){
    if (json_object["params"].size() == 0)
        return false;

    start_height = 0;
    end_height = libbitcoin::max_size_t;

    auto temp = json_object["params"][0];
    if (temp.is_object()){
        if (!temp["start"].is_null()){
            start_height = temp["start"][0];
        }
        if (!temp["end"].is_null()){
            end_height = temp["end"][0];
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

bool getaddresstxids (nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, size_t const& start_height, size_t const& end_height, libbitcoin::blockchain::block_chain const& chain)
{
    int i = 0;
    for(const auto & payment_address : payment_addresses) {
        libbitcoin::wallet::payment_address address(payment_address);
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
                                    for (auto it = history_list.rbegin(); it != history_list.rend(); ++it){
                                        json_object[i] = libbitcoin::encode_hash(*it);
                                        ++i;
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
nlohmann::json process_getaddresstxids(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error;
    std::string error_code;

    std::vector<std::string> payment_address;
    size_t start_height;
    size_t end_height;
    if (!json_in_getaddresstxids(json_in, payment_address, start_height, end_height)) 
    {
        //load error code
        //return
    }

    if (getaddresstxids(result, error, error_code, payment_address, start_height, end_height, chain))
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




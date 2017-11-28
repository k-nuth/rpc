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

#include <bitprim/rpc/messages/blockchain/getspentinfo.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool json_in_getspentinfo(nlohmann::json const& json_object, std::string& tx_id, size_t& index)
{
    if (json_object["params"].size() == 0)
        return false;
    auto temp = json_object["params"][0];
    if (temp.is_object()){
        tx_id = temp["txid"];
        index = temp["index"];
    }
    return true;
}

bool getspentinfo (nlohmann::json& json_object, int& error, std::string& error_code, std::string const& txid, size_t const& index, libbitcoin::blockchain::block_chain const& chain)
{
    libbitcoin::hash_digest hash;
    libbitcoin::decode_hash(hash, txid);
    libbitcoin::chain::output_point point(hash, index);
    boost::latch latch(2);

    chain.fetch_spend(point, [&](const libbitcoin::code &ec, libbitcoin::chain::input_point input) {
        if (ec == libbitcoin::error::success) {
            json_object["txid"] = libbitcoin::encode_hash(input.hash());
            json_object["index"] = input.index();
            {
                boost::latch latch2(2);
                chain.fetch_transaction(input.hash(), false,
                                         [&](const libbitcoin::code &ec, libbitcoin::transaction_const_ptr tx_ptr, size_t index,
                                             size_t height) {
                                             if (ec == libbitcoin::error::success) {
                                                 json_object["height"] = height;
                                             }
                                             latch2.count_down();
                                         });
                latch2.count_down_and_wait();
            }

        }
        else {
            std::cout << "No Encontrado" << std::endl;
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
    return true;
}
nlohmann::json process_getspentinfo(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error;
    std::string error_code;

    std::string tx_id;
    size_t index;
    if (!json_in_getspentinfo(json_in, tx_id, index)) //if false return error
    {
        //load error code
        //return
    }

    if (getspentinfo(result, error, error_code, tx_id, index, chain))
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

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

#include <bitprim/rpc/messages/wallet/sendrawtransaction.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool json_in_sendrawtransaction(nlohmann::json const& json_object, std::string& tx_str, bool & allowhighfees){
    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;

    tx_str = json_object["params"][0].get<std::string>();

    if (size == 2) {
        allowhighfees = json_object["params"][1].get<bool>();
    }

    return true;
}

bool sendrawtransaction(nlohmann::json& json_object, int& error, std::string& error_code, std::string const & incoming_hex, bool allowhighfees, bool use_testnet_rules, libbitcoin::blockchain::block_chain& chain)
{
    //TODO: use allowhighfees
    const auto tx = std::make_shared<bc::message::transaction>();
    libbitcoin::data_chunk out;
    libbitcoin::decode_base16(out,incoming_hex);
    tx->from_data(1, out);
    //TODO: error TX decode failed if from_data failed

    boost::latch latch(2);
    chain.organize(tx, [&](const libbitcoin::code & ec){
        if (ec){
            //TODO: error when sending the txn
            json_object;
        }else {
            json_object=libbitcoin::encode_hash(tx->hash());
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
    return true;
}

nlohmann::json process_sendrawtransaction(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain& chain, bool use_testnet_rules )
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error;
    std::string error_code;

    std::string tx_str;
    bool allowhighfees = false;
    if (!json_in_sendrawtransaction(json_in, tx_str, allowhighfees)) //if false return error
    {
        container["result"];
        container["error"]["code"] = -1;
        container["error"]["message"] = "sendrawtransaction \"hexstring\" ( allowhighfees )\n"
                "\nSubmits raw transaction (serialized, hex-encoded) to local node "
                "and network.\n"
                "\nAlso see createrawtransaction and signrawtransaction calls.\n"
                "\nArguments:\n"
                "1. \"hexstring\"    (string, required) The hex string of the raw "
                "transaction)\n"
                "2. allowhighfees    (boolean, optional, default=false) Allow high "
                "fees\n"
                "\nResult:\n"
                "\"hex\"             (string) The transaction hash in hex\n";
        return container;
    }

    if (sendrawtransaction(result, error, error_code, tx_str, allowhighfees, use_testnet_rules, chain))
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

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

#include <bitprim/rpc/messages/blockchain/getaddressmempool.hpp>
#include <boost/thread/latch.hpp>
#include <bitprim/rpc/messages/error_codes.hpp>

namespace bitprim {

//TODO 
bool json_in_getaddressmempool(nlohmann::json const& json_object, std::vector<std::string>& payment_address)
{
    // Example:
    // curl --user bitcoin:local321 --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getaddressmempool", "params": [{"addresses": ["mkR6TH68C5DXgjQDn7eDjTRFwNvo5V5zdA"]}] }' -H 'content-type: text/plain;' http://127.0.0.1:18332/

    if (json_object["params"].size() == 0)
        return false;

    try {
        auto temp = json_object["params"][0];
        if (temp.is_object()){
            for (const auto & addr : temp["addresses"]){
                payment_address.push_back(addr);
            }
        } else {
            //Only one address:
            payment_address.push_back(json_object["params"][0].get<std::string>());
        }
    } catch (const std :: exception & e) {
        return false;
    }

    return true;
}

bool getaddressmempool (nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    json_object = nlohmann::json::array();
    const auto res = chain.fetch_mempool_addrs(payment_addresses, use_testnet_rules);
    if (res.size() == 0){
        error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
        error_code = "No information available for address";
        return false;
    }
    size_t i = 0;
    for (auto const& r : res) {
        json_object[i]["address"] = std::get<0>(r);
        json_object[i]["txid"] = std::get<1>(r);
        json_object[i]["index"] = std::get<2>(r);
        json_object[i]["satoshis"] = std::get<3>(r);
        json_object[i]["timestamp"] = std::get<4>(r);
        if (std::get<5>(r) != ""){
            json_object[i]["prevtxid"] = std::get<5>(r);
            json_object[i]["prevout"] = std::get<6>(r);
        }
        ++i;
    }
    return true;
}


nlohmann::json process_getaddressmempool(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<std::string> payment_addresses;
    if (!json_in_getaddressmempool(json_in, payment_addresses)) 
    {
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "getaddressmempool\n"
                "\nReturns all mempool deltas for an address (requires addressindex to be enabled).\n"
                "\nArguments:\n"
                "{\n"
                "  \"addresses\"\n"
                "    [\n"
                "      \"address\"  (string) The base58check encoded address\n"
                "      ,...\n"
                "    ]\n"
                "}\n"
                "\nResult:\n"
                "[\n"
                "  {\n"
                "    \"address\"  (string) The base58check encoded address\n"
                "    \"txid\"  (string) The related txid\n"
                "    \"index\"  (number) The related input or output index\n"
                "    \"satoshis\"  (number) The difference of satoshis\n"
                "    \"timestamp\"  (number) The time the transaction entered the mempool (seconds)\n"
                "    \"prevtxid\"  (string) The previous txid (if spending)\n"
                "    \"prevout\"  (string) The previous transaction output index (if spending)\n"
                "  }\n"
                "]\n";
        return container;
    }

    if (getaddressmempool(result, error, error_code, payment_addresses, chain, use_testnet_rules))
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

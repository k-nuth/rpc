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

#include <bitprim/rpc/messages.hpp>
#include <bitprim/rpc/messages/messages.hpp>
#include <boost/thread/latch.hpp>
#include <bitcoin/bitcoin/bitcoin_cash_support.hpp>
#include <bitcoin/bitcoin/error.hpp>

namespace bitprim { namespace rpc {


// FIRST MESSAGE:

nlohmann::json process_data_element(nlohmann::json const& json_in, bool use_testnet_rules, libbitcoin::blockchain::block_chain& chain) {
    std::cout << "Processing json " << json_in["method"].get<std::string>() << std::endl;
    std::cout << "Detail json " << json_in << std::endl;

    if (json_in["method"].get<std::string>() == "getrawtransaction"){
        return process_getrawtransaction(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getaddressbalance"){
        return process_getaddressbalance(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getspentinfo"){
        return process_getspentinfo(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getaddresstxids"){
        return process_getaddresstxids(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getaddressdeltas"){
        return process_getaddressdeltas(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getaddressutxos"){
        return process_getaddressutxos(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getblockhashes"){
        return process_getblockhashes(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getinfo"){
        return process_getinfo(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getaddressmempool"){
        std::vector<std::string> payment_address;
        json_in_getaddressmempool(json_in, payment_address);
        return json_out(json_in, getaddressmempool(payment_address, chain));
    }

    if (json_in["method"].get<std::string>() == "getbestblockhash"){
        return process_getbestblockhash(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getblock"){
        return process_getblock(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getblockhash"){
        return process_getbestblockhash(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getblockchaininfo"){
        return process_getblockchaininfo(json_in, chain, use_testnet_rules);
    }

    if (json_in["method"].get<std::string>() == "getblockheader"){
        return process_getblockheader(json_in, chain, use_testnet_rules);
    }

    if(json_in["method"].get<std::string>() == "getblockcount")
        return process_getblockcount (json_in, chain, use_testnet_rules);

    if(json_in["method"].get<std::string>() == "getdifficulty")
        return process_getdifficulty (json_in, chain, use_testnet_rules);

    if(json_in["method"].get<std::string>() == "getchaintips")
        return process_getchaintips (json_in, chain, use_testnet_rules);

    if(json_in["method"].get<std::string>() == "validateaddress")
        return process_validateaddress (json_in, chain, use_testnet_rules);    
    
    if(json_in["method"].get<std::string>() == "getblocktemplate")
        return process_getblocktemplate (json_in, chain, use_testnet_rules);    

    if(json_in["method"].get<std::string>() == "getmininginfo")
        return process_getmininginfo (json_in, chain, use_testnet_rules);    

    if(json_in["method"].get<std::string>() == "submitblock")
        return process_submitblock  (json_in, chain, use_testnet_rules);

    if(json_in["method"].get<std::string>() == "sendrawtransaction")
        return process_sendrawtransaction  (json_in, chain, use_testnet_rules);

    std::cout << json_in["method"].get<std::string>() << " Command Not yet implemented." << std::endl;
    return nlohmann::json(); //TODO: error!
}

nlohmann::json json_out(nlohmann::json const& json_in, nlohmann::json const& json_resp){
    nlohmann::json container;
    container["result"] = json_resp;
    container["error"];
    container["id"] = json_in["id"];
    return container;
}

void copy_id(nlohmann::json const& json_in, nlohmann::json & json_resp){
    json_resp["id"] = json_in["id"];
}

bool json_in_getaddressmempool(nlohmann::json const& json_object, std::vector<std::string>& payment_address){
    return true;
}

std::string process_data(nlohmann::json const& json_object, bool use_testnet_rules, libbitcoin::blockchain::block_chain& chain) {
    //std::cout << "method: " << json_object["method"].get<std::string>() << "\n";
    //Bitprim-mining process data
    if (json_object.is_array()) {
        nlohmann::json res;
        size_t i = 0;
        for (const auto & method : json_object) {
            res[i] = process_data_element(method, use_testnet_rules, chain);
            ++i;
        }
        return res.dump();
    } else {
        return process_data_element(json_object, use_testnet_rules, chain).dump();
    }
}


nlohmann::json getaddressmempool(std::vector<std::string> const& addresses, libbitcoin::blockchain::block_chain const& chain) {
    nlohmann::json json_resp;
    json_resp = nlohmann::json::array();
    return json_resp;

}

}} // namespace bitprim::rpc

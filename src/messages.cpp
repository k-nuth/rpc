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
#include <boost/thread/latch.hpp>
#include <bitcoin/bitcoin/bitcoin_cash_support.hpp>
#include <bitcoin/bitcoin/error.hpp>

namespace bitprim { namespace rpc {


// FIRST MESSAGE:

nlohmann::json process_data_element(nlohmann::json const& json_in, bool use_testnet_rules, libbitcoin::blockchain::block_chain& chain) {
    std::cout << "Processing json " << json_in["method"].get<std::string>() << std::endl;
    std::cout << "Detail json " << json_in << std::endl;

    if (json_in["method"].get<std::string>() == "getrawtransaction"){
        std::string tx_id;
        bool verbose;
        json_in_getrawtransaction(json_in, tx_id, verbose); //if false return error
        auto temp = getrawtransaction(tx_id, verbose, chain, use_testnet_rules);
        copy_id(json_in, temp);
        return temp;
    }

    if (json_in["method"].get<std::string>() == "getaddressbalance"){
        std::vector<std::string> payment_addresses;
        json_in_getaddressbalance(json_in, payment_addresses); //if false return error
        return json_out(json_in, getaddressbalance(payment_addresses, chain));
    }

    if (json_in["method"].get<std::string>() == "getspentinfo"){
        std::string tx_id;
        size_t index;
        json_in_getspentinfo(json_in, tx_id, index); //if false return error
        return json_out(json_in, getspentinfo(tx_id, index, chain));
    }

    if (json_in["method"].get<std::string>() == "getaddresstxids"){
        std::vector<std::string> payment_address;
        size_t start_height;
        size_t end_height;
        json_in_getaddresstxids(json_in, payment_address, start_height, end_height); //if false return error
        return json_out(json_in, getaddresstxids(payment_address, start_height, end_height, chain));
    }

    if (json_in["method"].get<std::string>() == "getaddressdeltas"){
        std::vector<std::string> payment_address;
        size_t start_height;
        size_t end_height;
        bool include_chain_info;
        json_in_getaddressdeltas(json_in, payment_address, start_height, end_height, include_chain_info); //if false return error
        return json_out(json_in, getaddressdeltas(payment_address, start_height, end_height, include_chain_info, chain));
    }

    if (json_in["method"].get<std::string>() == "getaddressutxos"){
        std::vector<std::string> payment_address;
        bool chain_info;
        json_in_getaddressutxos(json_in, payment_address, chain_info); //if false return error
        return json_out(json_in, getaddressutxos(payment_address, chain_info, chain));
    }

    if (json_in["method"].get<std::string>() == "getblockhashes"){
        uint32_t time_high;
        uint32_t time_low;
        bool no_orphans;
        bool logical_times;
        json_in_getblockhashes(json_in, time_high, time_low, no_orphans, logical_times); //if false return error
        return json_out(json_in, getblockhashes(time_high, time_low, no_orphans, logical_times, chain));
    }

    if (json_in["method"].get<std::string>() == "getinfo"){
        return json_out(json_in, getinfo(chain));
    }

    if (json_in["method"].get<std::string>() == "getaddressmempool"){
        std::vector<std::string> payment_address;
        json_in_getaddressmempool(json_in, payment_address);
        return json_out(json_in, getaddressmempool(payment_address, chain));
    }

    if (json_in["method"].get<std::string>() == "getbestblockhash"){
        return json_out(json_in, getbestblockhash(chain));
    }

    if (json_in["method"].get<std::string>() == "getblock"){
        std::string hash;
        bool verbose;
        json_in_getblock(json_in, hash, verbose);
        auto temp = getblock(hash, verbose, chain);
        copy_id(json_in, temp);
        return temp;
    }

    if (json_in["method"].get<std::string>() == "getblockhash"){
        size_t height;
        json_in_getblockhash(json_in, height);
        return json_out(json_in, getblockhash(height, chain));
    }

    if (json_in["method"].get<std::string>() == "getblockchaininfo"){
        return json_out(json_in, getblockchaininfo(chain));
    }

    if (json_in["method"].get<std::string>() == "getblockheader"){
        std::string hash;
        bool verbose;
        json_in_getblockheader(json_in, hash, verbose);
        return json_out(json_in, rpc_getblockheader(hash, verbose, chain));
    }

    if(json_in["method"].get<std::string>() == "getblockcount")
        return json_out(json_in, getblockcount(chain));
    if(json_in["method"].get<std::string>() == "getdifficulty")
        return json_out(json_in, getdifficulty(chain));
    if(json_in["method"].get<std::string>() == "getchaintips")
        return json_out(json_in, getchaintips(chain));

    if(json_in["method"].get<std::string>() == "validateaddress")
    {
        std::string raw_address;
        json_in_validateaddress(json_in, raw_address);
        return json_out(json_in, validateaddress(raw_address ,chain));
    }

    if(json_in["method"].get<std::string>() == "getblocktemplate")
        return json_out(json_in, getblocktemplate(use_testnet_rules, libbitcoin::get_max_block_size(libbitcoin::is_bitcoin_cash()) - 20000, std::chrono::seconds(30), chain));

    if(json_in["method"].get<std::string>() == "getmininginfo")
        return json_out(json_in, getmininginfo(use_testnet_rules, chain));

    if(json_in["method"].get<std::string>() == "submitblock")
    {
        std::string block_str;
        if(json_in_submitblock(json_in, block_str))
            return json_out(json_in, submitblock(use_testnet_rules, block_str, chain));
        else {
            nlohmann::json resp;
            resp["result"];
            resp["error"]["code"] = -1;
            resp["error"]["message"] = "submitblock \"hexdata\" ( \"jsonparametersobject\" )\n\nAttempts to submit new block to network.\nThe 'jsonparametersobject' parameter is currently ignored.\nSee https://en.bitcoin.it/wiki/BIP_0022 for full specification.\n\nArguments\n1. \"hexdata\"    (string, required) the hex-encoded block data to submit\n2. \"jsonparametersobject\"     (string, optional) object of optional parameters\n    {\n      \"workid\" : \"id\"    (string, optional) if the server provided a workid, it MUST be included with submissions\n    }\n\nResult:\n\nExamples:\n> bitcoin-cli submitblock \"mydata\"\n> curl --user myusername --data-binary '{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"submitblock\", \"params\": [\"mydata\"] }' -H 'content-type: text/plain;' http://127.0.0.1:8332/\n";
            resp["id"] = json_in["id"];
            return resp;
        }
    }

    if(json_in["method"].get<std::string>() == "sendrawtransaction")
    {
        std::string tx_str;
        bool allowhighfees = false;
        if(json_in_sendrawtransaction(json_in, tx_str, allowhighfees))
            return json_out(json_in, sendrawtransaction(use_testnet_rules, tx_str, allowhighfees, chain));
        else {
            nlohmann::json resp;
            resp["result"];
            resp["error"]["code"] = -1;
            resp["error"]["message"] = "sendrawtransaction \"hexstring\" ( allowhighfees )\n"
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
            resp["id"] = json_in["id"];
            return resp;
        }
    }

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


bool json_in_getrawtransaction(nlohmann::json const& json_object, std::string& tx_id, bool& verbose){
    if (json_object["params"].size() == 0)
        return false;
    verbose = false;
    tx_id = json_object["params"][0];
    if (json_object["params"].size() == 2) {
        if (json_object["params"][1].is_boolean()){
            verbose = json_object["params"][1];
        } else if (json_object["params"][1].is_number() && json_object["params"][1] == 1){
            verbose = true;
        }
    }
    return true;
}

bool json_in_getaddressbalance(nlohmann::json const& json_object, std::vector<std::string>& address){
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

bool json_in_getspentinfo(nlohmann::json const& json_object, std::string& tx_id, size_t& index){
    if (json_object["params"].size() == 0)
        return false;
    auto temp = json_object["params"][0];
    if (temp.is_object()){
        tx_id = temp["txid"];
        index = temp["index"];
    }
    return true;
}


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

bool json_in_getaddressdeltas(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height, bool& include_chain_info){

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

bool json_in_getaddressutxos(nlohmann::json const& json_object, std::vector<std::string>& payment_address, bool& chain_info){
    if (json_object["params"].size() == 0)
        return false;

    chain_info = false;

    auto temp = json_object["params"][0];
    if (temp.is_object()){
        if(!temp["chainInfo"].is_null()){
            chain_info  = temp["chainInfo"][0];
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


bool json_in_getblockhashes(nlohmann::json const& json_object, uint32_t& time_high, uint32_t& time_low, bool& no_orphans, bool& logical_times){
    if (json_object["params"].size() == 0)
        return false;

    time_high = json_object["params"][0].get<uint32_t>();
    time_low = json_object["params"][1].get<uint32_t>();
    no_orphans = true;
    logical_times = false;
    // TODO: params[2] should work if only one parameter is sent, fix the size() == 2
    if (!json_object["params"][2].is_null() && json_object["params"][2].is_object() && json_object["params"][2].size() ==2){
        no_orphans = json_object["params"][2]["noOrphans"];
        logical_times = json_object["params"][2]["logicalTimes"];
    }
    return true;

}

bool json_in_getblock(nlohmann::json const& json_object, std::string & hash, bool & verbose) {
    if (json_object["params"].size() == 0)
        return false;
    verbose = true;
    hash = json_object["params"][0];
    if (json_object["params"].size() == 2) {
        verbose = json_object["params"][1];
    }
    return true;
}

bool json_in_getaddressmempool(nlohmann::json const& json_object, std::vector<std::string>& payment_address){
    return true;
}

bool json_in_getblockhash(nlohmann::json const& json_object, size_t& height){
    if (json_object["params"].size() == 0)
        return false;
    height = json_object["params"][0];
    return true;
}

bool json_in_getblockheader(nlohmann::json const& json_object, std::string & hash, bool & verbose) {
    if (json_object["params"].size() == 0)
        return false;
    verbose = true;
    hash = json_object["params"][0];
    if (json_object["params"].size() == 2) {
        verbose = json_object["params"][1];
    }
    return true;
}

bool json_in_validateaddress(nlohmann::json const& json_object, std::string& raw_address){
    if (json_object["params"].size() == 0)
        return false;
    else
    {
        raw_address = json_object["params"][0];
    }
    return true;
}

bool json_in_submitblock(nlohmann::json const& json_object, std::string& block_hex_str){
    if (json_object["params"].size() == 0)
        return false;

    block_hex_str = json_object["params"][0].get<std::string>();
    return true;
}

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



// RPC CODE
// ----------------------------------------------------------------------------
// TODO: move this code to a better place

std::string get_txn_type(const libbitcoin::chain::script& script){
    auto pattern = script.pattern();
    // The first operations access must be method-based to guarantee the cache.
    if (pattern == libbitcoin::machine::script_pattern::null_data)
        return "nulldata";

    if (pattern == libbitcoin::machine::script_pattern::pay_multisig)
        return "pay_multisig";

    if (pattern == libbitcoin::machine::script_pattern::pay_public_key)
        return "pay_public_key";

    if (pattern == libbitcoin::machine::script_pattern::pay_key_hash)
        return "pay_key_hash";

    if (pattern == libbitcoin::machine::script_pattern::pay_script_hash)
        return "pay_script_hash";

    if (pattern == libbitcoin::machine::script_pattern::sign_multisig)
        return "sign_multisig";

    if (pattern == libbitcoin::machine::script_pattern::sign_public_key)
        return "sign_public_key";

    if (pattern == libbitcoin::machine::script_pattern::sign_key_hash)
        return "sign_key_hash";

    if (pattern == libbitcoin::machine::script_pattern::sign_script_hash)
        return "sign_script_hash";

    return "non_standard";
}

nlohmann::json getrawtransaction (std::string const& txid, const bool verbose, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules /* = false*/) {
//    TO TEST RAW TXNS:
//    const data_chunk data = libbitcoin::to_chunk(base16_literal("01000000010506344de69d47e432eb0174500d6e188a9e63c1e84a9e8796ec98c99b7559f701000000fdfd00004730440220695a28c42daa23c13e192e36a20d03a2a79994e0fe1c3c6b612d0ae23743064602200ca19003e7c1ce0cecb0bbfba9a825fc3b83cf54e4c3261cd15f080d24a8a5b901483045022100aa9096ce71995c24545694f20ab0482099a98c99b799c706c333c521e51db66002206578f023fa46f4a863a6fa7f18b95eebd1a91fcdf6ce714e8795d902bd6b682b014c69522102b66fcb1064d827094685264aaa90d0126861688932eafbd1d1a4ba149de3308b21025cab5e31095551582630f168280a38eb3a62b0b3e230b20f8807fc5463ccca3c21021098babedb3408e9ac2984adcf2a8e4c48e56a785065893f76d0fa0ff507f01053aeffffffff01c8af0000000000001976a91458b7a60f11a904feef35a639b6048de8dd4d9f1c88ac00000000"));
//    auto temp = libbitcoin::message::transaction::factory_from_data(0,data);
//    auto* tx_new = new libbitcoin::message::transaction(temp);
//    auto tx_ptr = libbitcoin::message::transaction::const_ptr(tx_new);
    libbitcoin::hash_digest hash;
    libbitcoin::decode_hash(hash, txid);

    nlohmann::json json_object;
    nlohmann::json json_error;

    if (verbose) {
        boost::latch latch(2);
        chain.fetch_transaction(hash, false,
                                 [&](const libbitcoin::code &ec, libbitcoin::transaction_const_ptr tx_ptr, size_t index,
                                     size_t height) {
                                     if (ec == libbitcoin::error::success) {

                                         json_object["hex"] = libbitcoin::encode_base16(tx_ptr->to_data(/*version is not used*/ 0 ));
                                         json_object["txid"] = txid;
                                         json_object["hash"] = libbitcoin::encode_hash(tx_ptr->hash());
                                         json_object["size"] = tx_ptr->serialized_size(/*version is not used*/ 0 );
                                         json_object["version"] = tx_ptr->version();
                                         json_object["locktime"] = tx_ptr->locktime();

                                         int vin = 0;
                                         for(const auto & in: tx_ptr->inputs()){
                                             if (tx_ptr->is_coinbase()){
                                                 json_object["vin"][vin]["coinbase"] = libbitcoin::encode_base16(in.script().to_data(0));
                                             } else {
                                                 json_object["vin"][vin]["txid"] = libbitcoin::encode_hash(in.previous_output().hash());
                                                 json_object["vin"][vin]["vout"] = in.previous_output().index();
                                                 json_object["vin"][vin]["scriptSig"]["asm"] = in.script().to_string(0);
                                                 json_object["vin"][vin]["scriptSig"]["hex"] = libbitcoin::encode_base16(in.script().to_data(0));

                                                 boost::latch latch_address(2);
                                                 chain.fetch_transaction(in.previous_output().hash(), false,
                                                                         [&](const libbitcoin::code &ec, libbitcoin::transaction_const_ptr tx_ptr, size_t index,
                                                                             size_t height) {
                                                                             if (ec == libbitcoin::error::success) {
                                                                                 auto const & output = tx_ptr->outputs()[in.previous_output().index()];
                                                                                 json_object["vin"][vin]["address"] = output.address(use_testnet_rules).encoded();
                                                                                 json_object["vin"][vin]["value"] = output.value() / (double) 100000000;
                                                                                 json_object["vin"][vin]["valueSat"] = output.value();
                                                                             }
                                                                             latch_address.count_down();
                                                                         });
                                                 latch_address.count_down_and_wait();
                                             }
                                             json_object["vin"][vin]["sequence"] = in.sequence();
                                             ++vin;
                                         }

                                         int i = 0;
                                         for(const auto & out: tx_ptr->outputs()){
                                             json_object["vout"][i]["value"] = out.value() / (double) 100000000;
                                             json_object["vout"][i]["valueSat"] = out.value();
                                             json_object["vout"][i]["n"] = i;
                                             json_object["vout"][i]["scriptPubKey"]["asm"] = out.script().to_string(0);
                                             json_object["vout"][i]["scriptPubKey"]["hex"] = libbitcoin::encode_base16(out.script().to_data(0));

                                             uint8_t reqsig = 1;
                                             std::string type = get_txn_type (out.script());
                                             if (type == "pay_multisig" || type == "sign_multisig"){
                                                 // TODO: check if it's working for multisig (see ExtractDestinations in bitcoind)
                                                 reqsig = static_cast<uint8_t>(out.script().operations()[0].code());
                                             }
                                             json_object["vout"][i]["scriptPubKey"]["reqSigs"] = (int) reqsig;
                                             json_object["vout"][i]["scriptPubKey"]["type"] = type;
                                             json_object["vout"][i]["scriptPubKey"]["addresses"][0] = out.address(use_testnet_rules).encoded();

                                             // SPENT INFO
                                             auto spent = getspentinfo(libbitcoin::encode_hash(tx_ptr->hash()), i, chain);
                                             if (!spent.empty()) {
                                                 json_object["vout"][i]["spentTxId"] = spent["txid"];
                                                 json_object["vout"][i]["spentIndex"] = spent["index"];
                                                 json_object["vout"][i]["spentHeight"] = spent["height"];
                                             }
                                             ++i;
                                         }
                                         if (index != libbitcoin::database::transaction_database::unconfirmed) {
                                             //confirmed txn
                                             boost::latch latch2(2);
                                             chain.fetch_block_header(height, [&](std::error_code const &ec,
                                                                                   libbitcoin::message::header::ptr header,
                                                                                   size_t) {
                                                 if (ec == libbitcoin::error::success){
                                                     json_object["blockhash"] = libbitcoin::encode_hash(header->hash());
                                                     json_object["height"] = height;
                                                     boost::latch latch3(2);
                                                     chain.fetch_last_height(
                                                             [&](std::error_code const &ec, size_t last_height) {
                                                                 json_object["confirmations"] = 1 + last_height - height;
                                                                 latch3.count_down();
                                                             });
                                                     latch3.count_down_and_wait();
                                                     json_object["time"] = header->timestamp();
                                                     json_object["blocktime"] = header->timestamp();
                                                     latch2.count_down();
                                                 } else {
                                                     // Mempool txn (It should not enter here, the unconfirmed index should avoid this code)
                                                     json_object["height"] = -1;
                                                     json_object["confirmations"] = 0;
                                                     latch2.count_down();
                                                 }
                                             });

                                             latch2.count_down_and_wait();
                                         } else {
                                             //unconfirmed txn
                                             json_object["height"] = -1;
                                             json_object["confirmations"] = 0;
                                         }
                                     } else {
                                         json_error["code"] = -5;
                                         json_error["message"] = "No information available about transaction";
                                     }
                                     latch.count_down();
                                 });
        latch.count_down_and_wait();
    } else {
        // No verbose
        boost::latch latch(2);
        chain.fetch_transaction(hash, false,
                                 [&](const libbitcoin::code &ec, libbitcoin::transaction_const_ptr tx_ptr, size_t index,
                                     size_t height) {
                                     if (ec == libbitcoin::error::success) {
                                         json_object = libbitcoin::encode_base16(tx_ptr->to_data(/*version is not used*/ 0));
                                     } else {
                                         json_error["code"] = -5;
                                         json_error["message"] = "No information available about transaction";
                                     }
                                     latch.count_down();
                                 });
        latch.count_down_and_wait();
    }
    nlohmann::json container;
    container["result"] = json_object;
    container["error"] = json_error;
    return container;
}

nlohmann::json getspentinfo (std::string const& txid, size_t const& index, libbitcoin::blockchain::block_chain const& chain) {
    libbitcoin::hash_digest hash;
    libbitcoin::decode_hash(hash, txid);
    libbitcoin::chain::output_point point(hash, index);
    boost::latch latch(2);

    nlohmann::json json_object;
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
    return json_object;
}

//Should be vector <libbitcoin::wallet::payment_address> addresses
nlohmann::json getaddresstxids(std::vector<std::string> const& payment_addresses, size_t const& start_height, size_t const& end_height, libbitcoin::blockchain::block_chain const& chain){
    nlohmann::json json_object;
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
    return json_object;
}

libbitcoin::chain::history::list expand(libbitcoin::chain::history_compact::list& compact) {
    libbitcoin::chain::history::list result;
    result.reserve(compact.size());

    // Process and remove all outputs.
    for (auto output = compact.begin(); output != compact.end();)
    {
        if (output->kind == libbitcoin::chain::point_kind::output)
        {
            libbitcoin::chain::history row;

            // Move the output to the result.
            row.output = std::move(output->point);
            row.output_height = output->height;
            row.value = output->value;

            // Initialize the spend to null.
            row.spend = libbitcoin::chain::input_point{ libbitcoin::null_hash, libbitcoin::chain::point::null_index };
            row.temporary_checksum = row.output.checksum();

            // Store the result and erase the output.
            result.emplace_back(std::move(row));
            output = compact.erase(output);
            continue;
        }

        // Skip the spend.
        ++output;
    }

    // TODO: reduce to output set with distinct checksums, as a fault signal.
    ////std::sort(result.begin(), result.end());
    ////result.erase(std::unique(result.begin(), result.end()), result.end());

    // All outputs have been removed, process the spends.
    for (auto& spend: compact)
    {
        auto found = false;

        // Update outputs with the corresponding spends.
        // This relies on the lucky avoidance of checksum hash collisions :<.
        // Ordering is insufficient since the server may write concurrently.
        for (auto& row: result)
        {
            // The temporary_checksum is a union with spend_height, so we must
            // guard against reading temporary_checksum unless spend is null.
            if (row.spend.is_null() &&
                row.temporary_checksum == spend.previous_checksum)
            {
                // Move the spend to the row of its correlated output.
                row.spend = std::move(spend.point);
                row.spend_height = spend.height;

                found = true;
                break;
            }
        }

        // This will only happen if the history height cutoff comes between an
        // output and its spend. In this case we return just the spend.
        // This is not strictly sufficient because of checksum hash collisions,
        // So this miscorrelation must be discarded as a fault signal.
        if (!found)
        {
            libbitcoin::chain::history row;

            // Initialize the output to null.
            row.output = libbitcoin::chain::output_point{ libbitcoin::null_hash, libbitcoin::chain::point::null_index };
            row.output_height = libbitcoin::max_uint64;
            row.value = libbitcoin::max_uint64;

            // Move the spend to the row.
            row.spend = std::move(spend.point);
            row.spend_height = spend.height;
            result.emplace_back(std::move(row));
        }
    }

    compact.clear();
    result.shrink_to_fit();

    // Clear all remaining checksums from unspent rows.
    for (auto& row: result)
        if (row.spend.is_null())
            row.spend_height = libbitcoin::max_uint64;

    // TODO: sort by height and index of output, spend or both in order.
    return result;
}

nlohmann::json getaddressdeltas(std::vector<std::string> const& payment_addresses, size_t const& start_height, size_t const& end_height, const bool include_chain_info, libbitcoin::blockchain::block_chain const& chain){
    nlohmann::json json_object;
    int i = 0;
    for(const auto & payment_address : payment_addresses) {
        libbitcoin::wallet::payment_address address(payment_address);
        boost::latch latch(2);
        chain.fetch_history(address, INT_MAX, 0, [&](const libbitcoin::code &ec,
                                                     libbitcoin::chain::history_compact::list history_compact_list) {
            if (ec == libbitcoin::error::success) {
                auto const history_list = bitprim::expand(history_compact_list);
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
    return json_object;
}

nlohmann::json getaddressbalance(std::vector<std::string> const& addresses, libbitcoin::blockchain::block_chain const& chain) {
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
    nlohmann::json json_object;
    json_object["balance"] = balance;
    json_object["received"] = received;
    return json_object;
}

nlohmann::json getaddressutxos(std::vector<std::string> const& payment_addresses, const bool chain_info, libbitcoin::blockchain::block_chain const& chain) {
    boost::latch latch(2);
    nlohmann::json utxos;

    int i = 0;
    for(const auto & payment_address : payment_addresses) {
        libbitcoin::wallet::payment_address address(payment_address);
        chain.fetch_history(address, INT_MAX, 0, [&](const libbitcoin::code &ec,
                                                     libbitcoin::chain::history_compact::list history_compact_list) {
            if (ec == libbitcoin::error::success) {
                auto const history_list = bitprim::expand(history_compact_list);
                for (const auto & history : history_list) {
                    if (history.spend.hash() == libbitcoin::null_hash) {
                        //Es outpoint
                        //Error de wallets no se guarda history.spend
                        //Lo busco en la DB
                        {
                            boost::latch latch2(2);
                            chain.fetch_spend(history.output, [&](const libbitcoin::code &ec,
                                                                  libbitcoin::chain::input_point input) {
                                if (ec == libbitcoin::error::not_found) {
                                    utxos[i]["address"] = address.encoded();
                                    utxos[i]["txid"] = libbitcoin::encode_hash(history.output.hash());
                                    utxos[i]["outputIndex"] = history.output.index();
                                    utxos[i]["script"] = history.output.validation.cache.script().to_data(0);
                                    utxos[i]["satoshis"] = history.value;
                                    utxos[i]["height"] = history.output_height;
                                    ++i;
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

    nlohmann::json json_object;

    if (chain_info) {
        json_object["utxos"] = utxos;

        size_t height;
        boost::latch latch2(2);
        chain.fetch_last_height([&] (const libbitcoin::code &ec, size_t last_height){
            if (ec == libbitcoin::error::success) {
                height = last_height;
            }
            latch2.count_down();
        });
        latch2.count_down_and_wait();

        boost::latch latch3(2);
        chain.fetch_block(height, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block, size_t){
            if (ec == libbitcoin::error::success) {
                json_object["height"] = height;
                json_object["hash"] = libbitcoin::encode_hash(block->hash());
            }
            latch3.count_down();
        });
        latch3.count_down_and_wait();

    } else {
        json_object = utxos;
    }

    return json_object;
}

void getblockheader(size_t i,libbitcoin::message::header::ptr& header, libbitcoin::blockchain::block_chain const& chain){
    boost::latch latch(2);

    chain.fetch_block_header(i, [&](const libbitcoin::code &ec, libbitcoin::message::header::ptr h, size_t height) {
        if (ec == libbitcoin::error::success) {
            header = h;
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
}

void update_mid(size_t top_height, size_t low_height, size_t& mid, libbitcoin::message::header::ptr& header, libbitcoin::blockchain::block_chain const& chain){
    mid = (top_height + low_height) / 2;
    getblockheader(mid, header, chain);
}

nlohmann::json getblockhashes(uint32_t time_high, uint32_t time_low, bool no_orphans, bool logical_times, libbitcoin::blockchain::block_chain const& chain){
   nlohmann::json json_object = nlohmann::json::array();
    if(time_high < time_low)
        return json_object;

    libbitcoin::message::header::ptr genesis, top, mid_header;
    getblockheader(0, genesis, chain);
    uint32_t time_genesis = genesis->timestamp();

    if(time_high < time_genesis)
        return json_object;

    if (time_low < time_genesis) time_low = time_genesis;


    size_t top_height;
    chain.get_last_height(top_height);
    getblockheader(top_height, top, chain);
    uint32_t time_top = top->timestamp();

    if(time_top < time_low)
        return json_object;

    if (time_high > time_top) time_high = time_top;

    //TODO: return error when time_high - time_low >= 2*60*60*24 (2 days)

    size_t low_height = 0;
    size_t mid;
    update_mid(top_height, low_height, mid, mid_header, chain);

    while (!((mid_header->timestamp() >= time_low) && (mid_header->timestamp() <= time_high))){
        if (mid_header->timestamp() > time_high){
            top_height = mid;
            update_mid(top_height, low_height, mid, mid_header, chain);
        }
        if (mid_header->timestamp() < time_low){
            low_height = mid;
            update_mid(top_height, low_height, mid, mid_header, chain);
        }
    }

    libbitcoin::message::header::ptr last_header_found = mid_header;
    uint32_t last_time_found = mid_header->timestamp();
    size_t last_height = mid;

    std::deque<std::pair<libbitcoin::hash_digest, uint32_t>> hashes;

    while (last_time_found <= time_high && last_height <= top_height){
        hashes.push_back(std::make_pair(last_header_found->hash(), last_header_found->timestamp()));
        last_height++;
        getblockheader(last_height, last_header_found, chain);
        last_time_found = last_header_found->timestamp();
    }

    last_height = mid - 1;
    getblockheader(last_height, last_header_found, chain);
    last_time_found = last_header_found->timestamp();

    while (last_time_found >= time_low && last_height >=0){
        hashes.push_front(std::make_pair(last_header_found->hash(), last_header_found->timestamp()));
        last_height--;
        getblockheader(last_height, last_header_found, chain);
        last_time_found = last_header_found->timestamp();
    }


    int i = 0;
    if (!logical_times) {
        for (const auto & h : hashes) {
            json_object[i] = libbitcoin::encode_hash(h.first);
            ++i;
        }
    } else {
        for (const auto & h : hashes) {
            json_object[i]["blockhash"] = libbitcoin::encode_hash(h.first);
            json_object[i]["timestamp"] = h.second;
            ++i;
        }
    }
    return json_object;
}

std::tuple<bool ,size_t, double> get_last_block_difficulty(libbitcoin::blockchain::block_chain const& chain){

    double diff = 1.0;
    size_t top_height;
    libbitcoin::message::header::ptr top = nullptr;
    bool success = false;
    if(chain.get_last_height(top_height)){
        getblockheader(top_height, top, chain);
        if(top != nullptr){
            success = true;
            auto bits = top->bits();
            int shift = (bits >> 24) & 0xff;
            diff = (double)0x0000ffff / (double)(bits & 0x00ffffff);
            while (shift < 29) {
                diff *= 256.0;
                ++shift;
            }
            while (shift > 29) {
                diff /= 256.0;
                --shift;
            }
        }
    }
    return std::make_tuple(success, top_height, diff);
}



nlohmann::json getinfo(libbitcoin::blockchain::block_chain const& chain) {
    nlohmann::json json_resp;

#define CLIENT_VERSION_MAJOR 0
#define CLIENT_VERSION_MINOR 12
#define CLIENT_VERSION_REVISION 0
#define CLIENT_VERSION_BUILD 0
    static const int CLIENT_VERSION =
            1000000 * CLIENT_VERSION_MAJOR
            +   10000 * CLIENT_VERSION_MINOR
            +     100 * CLIENT_VERSION_REVISION
            +       1 * CLIENT_VERSION_BUILD;

    json_resp["version"] = CLIENT_VERSION;

    json_resp["protocolversion"] = 70013;

    auto last_block_data = get_last_block_difficulty(chain);

    if (std::get<0>(last_block_data)) {
        json_resp["blocks"] = std::get<1>(last_block_data);
    }

    json_resp["timeoffset"] = 0;

    //TODO: get outbound + inbound connections from node
    json_resp["connections"] = 16;

    json_resp["proxy"] = "";

    json_resp["difficulty"] = std::get<2>(last_block_data);

    //TODO: set testnet variable
    json_resp["testnet"] = true;

    //TODO: set minimun fee
    json_resp["relayfee"] = 0.0;

    //TODO: check errors
    json_resp["errors"] = "";

    return json_resp;
}


nlohmann::json getaddressmempool(std::vector<std::string> const& addresses, libbitcoin::blockchain::block_chain const& chain) {
    nlohmann::json json_resp;
    json_resp = nlohmann::json::array();
    return json_resp;

}

nlohmann::json getbestblockhash(libbitcoin::blockchain::block_chain const& chain) {
    nlohmann::json json_resp;

    size_t top_height;
    libbitcoin::message::header::ptr top;
    chain.get_last_height(top_height);
    getblockheader(top_height, top, chain);
    
    json_resp = libbitcoin::encode_hash(top->hash());
    return json_resp;

}

double bits_to_difficulty (const uint32_t & bits){
    double diff = 1.0;
    int shift = (bits >> 24) & 0xff;
    diff = (double)0x0000ffff / (double)(bits & 0x00ffffff);
    while (shift < 29) {
        diff *= 256.0;
        ++shift;
    }
    while (shift > 29) {
        diff /= 256.0;
        --shift;
    }
    return diff;
}

nlohmann::json getblock(const std::string & block_hash, bool verbose, libbitcoin::blockchain::block_chain const& chain) {
    nlohmann::json json_resp;
    nlohmann::json json_error;

    libbitcoin::hash_digest hash;
    libbitcoin::decode_hash(hash, block_hash);

    boost::latch latch(2);
    chain.fetch_block(hash, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block, size_t height){
        if (ec == libbitcoin::error::success) {
            if (!verbose) {
                json_resp = libbitcoin::encode_base16(block->to_data(0));
            } else {
                json_resp["hash"] = libbitcoin::encode_hash(block->hash());

                size_t top_height;
                chain.get_last_height(top_height);
                json_resp["confirmations"] = top_height - height + 1;

                json_resp["size"] = block->serialized_size(0);
                json_resp["height"] = height;
                json_resp["version"] = block->header().version();
                // TODO: encode the version to base 16
                json_resp["versionHex"] = block->header().version();
                json_resp["merkleroot"] = libbitcoin::encode_hash(block->header().merkle());

                int i = 0;
                for (const auto & txns : block->transactions()) {
                    json_resp["tx"][i] = libbitcoin::encode_hash(txns.hash());
                    ++i;
                }

                json_resp["time"] = block->header().timestamp();
                // TODO: get real median time
                json_resp["mediantime"] = block->header().timestamp();
                json_resp["nonce"] = block->header().nonce();
                // TODO: encode bits to base 16
                json_resp["bits"] = block->header().bits();
                json_resp["difficulty"] = bits_to_difficulty(block->header().bits());
                // TODO: validate that proof is chainwork
                // Optimizate the encoded to base 16
                std::stringstream ss;
                ss << std::setfill('0')
                   << std::nouppercase
                   << std::hex
                   << block->proof();
                json_resp["chainwork"] = ss.str();
                json_resp["previousblockhash"] = libbitcoin::encode_hash(block->header().previous_block_hash());
                json_resp["nextblockhash"];

                boost::latch latch2(2);
                chain.fetch_block(height + 1, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block_2, size_t) {
                    if (ec == libbitcoin::error::success) {
                        json_resp["nextblockhash"] = libbitcoin::encode_hash(block_2->header().hash());
                    }
                    latch2.count_down();
                });
                latch2.count_down_and_wait();
            }
        } else {
            json_error["code"] = -8;
            json_error["message"] = "Block height out of range";
        }
        latch.count_down();
    });
    latch.count_down_and_wait();

    nlohmann::json container;
    container["result"] = json_resp;
    container["error"] = json_error;
    return container;
}

nlohmann::json getblockhash(const size_t height, libbitcoin::blockchain::block_chain const& chain){
    nlohmann::json json_resp;

    libbitcoin::message::header::ptr header;
    getblockheader(height, header, chain);
    
    json_resp = libbitcoin::encode_hash(header->hash());
    return json_resp;

}

nlohmann::json getblockchaininfo(libbitcoin::blockchain::block_chain const& chain){
    nlohmann::json json_resp;
    json_resp["chain"] = "main";
    
    size_t top_height;
    libbitcoin::message::header::ptr top;
    chain.get_last_height(top_height);

    boost::latch latch(2);
    chain.fetch_block(top_height, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block, size_t height){
        if (ec == libbitcoin::error::success) {
                json_resp["blocks"] = height;
                json_resp["headers"] = height;
                json_resp["bestblockhash"] = libbitcoin::encode_hash(block->hash());
                json_resp["difficulty"] = bits_to_difficulty(block->header().bits());
                json_resp["mediantime"] = block->header().timestamp(); //TODO Get medianpasttime
                json_resp["verificationprogress"] = 1;
                std::stringstream ss;
                ss << std::setfill('0')
                   << std::nouppercase
                   << std::hex
                   << block->proof();
                json_resp["chainwork"] = ss.str();
                json_resp["pruned"] = false;
                json_resp["pruneheight"] = 0;
                json_resp["softforks"] = nlohmann::json::array(); //TODO Check softforks
                json_resp["bip9_softforks"] = nlohmann::json::array(); //TODO Check softforks
            
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
    return json_resp;
}

nlohmann::json rpc_getblockheader(const std::string & block_hash, bool verbose, libbitcoin::blockchain::block_chain const& chain) {
    nlohmann::json json_resp;

    libbitcoin::hash_digest hash;
    libbitcoin::decode_hash(hash, block_hash);

    boost::latch latch(2);
    chain.fetch_block(hash, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block, size_t height){
        if (ec == libbitcoin::error::success) {
            if (!verbose) {
                json_resp = libbitcoin::encode_base16(block->header().to_data(0));
            } else {
                json_resp["hash"] = libbitcoin::encode_hash(block->hash());

                size_t top_height;
                chain.get_last_height(top_height);
                json_resp["confirmations"] = top_height - height + 1;

                json_resp["size"] = block->serialized_size(0);
                json_resp["height"] = height;
                json_resp["version"] = block->header().version();
                // TODO: encode the version to base 16
                json_resp["versionHex"] = block->header().version();
                json_resp["merkleroot"] = libbitcoin::encode_hash(block->header().merkle());
                json_resp["time"] = block->header().timestamp();
                // TODO: get real median time
                json_resp["mediantime"] = block->header().timestamp();
                json_resp["nonce"] = block->header().nonce();
                // TODO: encode bits to base 16
                json_resp["bits"] = block->header().bits();
                json_resp["difficulty"] = bits_to_difficulty(block->header().bits());
                // TODO: validate that proof is chainwork
                // Optimizate the encoded to base 16
                std::stringstream ss;
                ss << std::setfill('0')
                   << std::nouppercase
                   << std::hex
                   << block->proof();
                json_resp["chainwork"] = ss.str();
                json_resp["previousblockhash"] = libbitcoin::encode_hash(block->header().previous_block_hash());
                json_resp["nextblockhash"];

                boost::latch latch2(2);
                chain.fetch_block(height + 1, [&] (const libbitcoin::code &ec, libbitcoin::block_const_ptr block_2, size_t) {
                    if (ec == libbitcoin::error::success) {
                        json_resp["nextblockhash"] = libbitcoin::encode_hash(block_2->header().hash());
                    }
                    latch2.count_down();
                });
                latch2.count_down_and_wait();
            }
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
    return json_resp;
}

nlohmann::json getblockcount(libbitcoin::blockchain::block_chain const& chain)
{
    nlohmann::json json_resp;

    size_t top_height;
    chain.get_last_height(top_height);   
    json_resp = top_height;
    return json_resp;
}

nlohmann::json getdifficulty(libbitcoin::blockchain::block_chain const& chain)
{
    nlohmann::json json_resp;
    json_resp = std::get<2>(get_last_block_difficulty(chain));
    return json_resp;
}

nlohmann::json getchaintips(libbitcoin::blockchain::block_chain const& chain)
{
/*
            "getchaintips\n"
            "Return information about all known tips in the block tree,"
            " including the main chain as well as orphaned branches.\n"
            "\nResult:\n"
            "[\n"
            "  {\n"
            "    \"height\": xxxx,         (numeric) height of the chain tip\n"
            "    \"hash\": \"xxxx\",         (string) block hash of the tip\n"
            "    \"branchlen\": 0          (numeric) zero for main chain\n"
            "    \"status\": \"active\"      (string) \"active\" for the main "
            "chain\n"
            "  },\n"
            "  {\n"
            "    \"height\": xxxx,\n"
            "    \"hash\": \"xxxx\",\n"
            "    \"branchlen\": 1          (numeric) length of branch "
            "connecting the tip to the main chain\n"
            "    \"status\": \"xxxx\"        (string) status of the chain "
            "(active, valid-fork, valid-headers, headers-only, invalid)\n"
            "  }\n"
            "]\n"
            "Possible values for status:\n"
            "1.  \"invalid\"               This branch contains at least one "
            "invalid block\n"
            "2.  \"headers-only\"          Not all blocks for this branch are "
            "available, but the headers are valid\n"
            "3.  \"valid-headers\"         All blocks are available for this "
            "branch, but they were never fully validated\n"
            "4.  \"valid-fork\"            This branch is not part of the "
            "active chain, but is fully validated\n"
            "5.  \"active\"                This is the tip of the active main "
            "chain, which is certainly valid\n"
            "\nExamples:\n" +
            HelpExampleCli("getchaintips", "") +
            HelpExampleRpc("getchaintips", ""));
    }
*/

    nlohmann::json json_resp, active;
    size_t top_height;
    libbitcoin::message::header::ptr top;
    chain.get_last_height(top_height);
    getblockheader(top_height, top, chain);
    active["height"] = top_height;  
    active["hash"] = libbitcoin::encode_hash(top->hash());
    active["branchlen"] = 0;
    active["status"] = "active";
    json_resp[0] = active;
    return json_resp;

}

nlohmann::json validateaddress(std::string& raw_address, libbitcoin::blockchain::block_chain const& chain) {
    nlohmann::json json_resp;
    libbitcoin::wallet::payment_address payment_address (raw_address);

    int ver = (int) payment_address.version();
    if (ver == 111 /*testnet*/ || ver == 0 /*mainnet btc*/ || ver == 48 /*mainnet ltc*/ ){
        //PAY TO PUBLIC KEY HASH
        json_resp["isvalid"] = true;
        json_resp["address"] = raw_address;
        json_resp["scriptPubKey"] = "76a914" + libbitcoin::encode_base16(payment_address.hash()) + "88ac";
    } else if (ver == 196 /*testnet*/ || ver == 5 /*mainnet btc*/ || ver == 5 /*mainnet ltc*/ ){
        //PAY TO SCRIPT HASH
        json_resp["isvalid"] = true;
        json_resp["address"] = raw_address;
        json_resp["scriptPubKey"] = "a914" + libbitcoin::encode_base16(payment_address.hash()) + "87";
    } else {
        //TODO: VALIDATE WIF
        json_resp["isvalid"] = false;
        json_resp["address"];
        json_resp["scriptPubKey"] ;
    }

    return json_resp;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////  GET BLOCK TEMPLATE ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool get_last_block_header(size_t& out_height, libbitcoin::message::header& out_header, libbitcoin::blockchain::block_chain const& chain) {
    //size_t out_height;
    if (chain.get_last_height(out_height)) {
        return chain.get_header(out_header, out_height);
    }
    return false;
}

uint32_t get_clock_now() {
    auto const now = std::chrono::high_resolution_clock::now();
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
}

/*constexpr*/
uint64_t get_block_reward(uint32_t height) {
    auto subsidy = libbitcoin::initial_block_subsidy_satoshi();
    subsidy >>= (height / libbitcoin::subsidy_interval);
    return subsidy;
}

nlohmann::json getblocktemplate(bool use_testnet_rules, size_t max_bytes, std::chrono::nanoseconds timeout, libbitcoin::blockchain::block_chain const& chain) {

    nlohmann::json json_object;

    json_object["capabilities"] = std::vector<std::string> {"proposal"};
    json_object["version"] = 536870912;                          //TODO: hardcoded value
    json_object["rules"] = std::vector<std::string> {"csv"}; //, "!segwit"};  //TODO!
    json_object["vbavailable"] = nlohmann::json::object();
    json_object["vbrequired"] = 0;

    static bool first_time = true;
    static size_t old_height = 0;
    static std::vector<libbitcoin::blockchain::block_chain::tx_mempool> tx_cache;
    static std::chrono::time_point<std::chrono::high_resolution_clock> cache_timestamp = std::chrono::high_resolution_clock::now();

    size_t last_height;
    chain.get_last_height(last_height);
    libbitcoin::message::header::ptr header;
    getblockheader(last_height, header, chain);

    auto time_now = get_clock_now();
    json_object["curtime"] = time_now;
    json_object["mintime"] = chain.chain_state()->median_time_past() + 1;
    if (json_object["curtime"] < json_object["mintime"]) {
        json_object["curtime"] = json_object["mintime"];
    }

    auto const bits = chain.chain_state()->get_next_work_required(time_now);
    auto const height = last_height + 1;

    json_object["previousblockhash"] = libbitcoin::encode_hash(header->hash());

    json_object["sigoplimit"] = libbitcoin::get_max_block_sigops(libbitcoin::is_bitcoin_cash()); //OLD max_block_sigops; //TODO: this value is hardcoded using bitcoind pcap
    json_object["sizelimit"] = libbitcoin::get_max_block_size(libbitcoin::is_bitcoin_cash()); //OLD max_block_size;   //TODO: this value is hardcoded using bitcoind pcap
    json_object["weightlimit"] = libbitcoin::get_max_block_size(libbitcoin::is_bitcoin_cash());//                    //TODO: this value is hardcoded using bitcoind pcap

    auto now = std::chrono::high_resolution_clock::now();

    if (first_time || old_height != last_height ||
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - cache_timestamp) >= timeout)
    {
        first_time = false;
        old_height = last_height;
        tx_cache = chain.fetch_mempool_all(max_bytes);
        cache_timestamp = std::chrono::high_resolution_clock::now();
    }

   nlohmann::json transactions_json = nlohmann::json::array();

    uint64_t fees = 0;
    for (size_t i = 0; i < tx_cache.size(); ++i) {
        auto const& tx_mem = tx_cache[i];
        auto const& tx = std::get<0>(tx_mem);
        const auto tx_data = tx.to_data();
        transactions_json[i]["data"] = libbitcoin::encode_base16(tx_data);
        transactions_json[i]["txid"] = libbitcoin::encode_hash(tx.hash());
        transactions_json[i]["hash"] = libbitcoin::encode_hash(tx.hash());
        transactions_json[i]["depends"] = nlohmann::json::array(); //TODO CARGAR DEPS
        transactions_json[i]["fee"] = std::get<1>(tx_mem);
        transactions_json[i]["sigops"] = std::get<2>(tx_mem);
        transactions_json[i]["weight"] = tx_data.size();
        fees += std::get<1>(tx_mem);
    }

    json_object["transactions"] = transactions_json;

    auto coinbase_reward = get_block_reward(height);
    json_object["coinbasevalue"] = coinbase_reward + fees;
    json_object["coinbaseaux"]["flags"] = "";

    const auto header_bits = libbitcoin::chain::compact(bits);
    libbitcoin::uint256_t target(header_bits);

    std::ostringstream target_stream;
    target_stream << std::setfill('0') << std::setw(64) << std::hex << target <<"\0"<< std::dec;
    auto target_str = target_stream.str();
    char final_target[66];
    target_str.copy(final_target, 64);
    final_target[64] = '\0';
    json_object["target"] = final_target;

    json_object["mutable"]  = std::vector<std::string> {"time", "transactions", "prevblock"};
    json_object["noncerange"] = "00000000ffffffff";

    uint8_t rbits[8];
    sprintf((char*)rbits, "%08x", bits);
    json_object["bits"] = std::string((char*)rbits, 8);

    json_object["height"] = height;

    return json_object;
}

void handle_organize(const libbitcoin::code& ec){
    if(ec)
        std::cout<< "Failed to submit block" << std::endl;
    else std::cout<< "Block submited successfully" << std::endl;
}

nlohmann::json submitblock(bool use_testnet_rules, std::string const& incoming_hex, libbitcoin::blockchain::block_chain& chain) {

    nlohmann::json json_resp;
    const auto block = std::make_shared<bc::message::block>();
    libbitcoin::data_chunk out;
    libbitcoin::decode_base16(out,incoming_hex);
    block->from_data(1, out);

    chain.organize(block, [&](const libbitcoin::code & ec){ handle_organize(ec);});
    //TODO: error messages
    return json_resp;
}

nlohmann::json sendrawtransaction(bool use_testnet_rules, std::string const & incoming_hex, bool allowhighfees,libbitcoin::blockchain::block_chain& chain) {

    //TODO: use allowhighfees
    nlohmann::json json_resp;
    const auto tx = std::make_shared<bc::message::transaction>();
    libbitcoin::data_chunk out;
    libbitcoin::decode_base16(out,incoming_hex);
    tx->from_data(1, out);
    //TODO: error TX decode failed if from_data failed

    boost::latch latch(2);
    chain.organize(tx, [&](const libbitcoin::code & ec){
        if (ec){
            //TODO: error when sending the txn
            json_resp;
        }else {
            json_resp=libbitcoin::encode_hash(tx->hash());
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
    return json_resp;
}


nlohmann::json getmininginfo(bool use_testnet_rules, libbitcoin::blockchain::block_chain const& chain) {

    nlohmann::json json_resp;

    auto last_block_data = get_last_block_difficulty(chain);

    if (std::get<0>(last_block_data)) {
        json_resp["blocks"] = std::get<1>(last_block_data);
    }

    //TODO: check size and weight on mainnet, testnet is sending 0s
    json_resp["currentblocksize"] = libbitcoin::get_max_block_size(libbitcoin::is_bitcoin_cash());
    json_resp["currentblockweight"] = libbitcoin::get_max_block_size(libbitcoin::is_bitcoin_cash());
    json_resp["currentblocktx"] = 0;

    json_resp["difficulty"] = std::get<2>(last_block_data);
    //TODO: check errors
    json_resp["errors"] = "";

    //TODO: calculate networkhashps
    json_resp["networkhashps"] = 0.0;
    //TODO: calculate pooledtx
    json_resp["pooledtx"] = 0;

    json_resp["testnet"] = use_testnet_rules;

    //TODO: libbitcoin does not support regtest
    if (use_testnet_rules){
        json_resp["chain"] = "test";
    } else {
        json_resp["chain"] = "main";
    }

    return json_resp;
}

}} // namespace bitprim::rpc

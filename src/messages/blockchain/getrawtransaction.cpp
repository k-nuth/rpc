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

#include <bitprim/rpc/messages/blockchain/getrawtransaction.hpp>
#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/blockchain/getspentinfo.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool json_in_getrawtransaction(nlohmann::json const& json_object, std::string& tx_id, bool& verbose)
{
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



bool getrawtransaction (nlohmann::json& json_object, int& error, std::string& error_code, std::string const& txid, const bool verbose, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    libbitcoin::hash_digest hash;
    if(libbitcoin::decode_hash(hash, txid)){

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
                                                 nlohmann::json spent;
                                                 getspentinfo(spent, error, error_code,libbitcoin::encode_hash(tx_ptr->hash()), i, chain);
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
                                             error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
                                             error_code = "No information available about transaction";
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
                                             error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
                                             error_code = "No information available about transaction";
                                         }
                                         latch.count_down();
                                     });
            latch.count_down_and_wait();
        }
    } else {
        error = bitprim::RPC_INVALID_PARAMETER;
        error_code = "Invalid transaction hash";
    }

    if(error != 0)
        return false;
    return true;

}


nlohmann::json process_getrawtransaction(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string tx_id;
    bool verbose;
    if(!json_in_getrawtransaction(json_in, tx_id, verbose)) //if false return error
    {
        //load error code
        //return
    }

    if(getrawtransaction(result, error, error_code, tx_id, verbose, chain, use_testnet_rules))
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


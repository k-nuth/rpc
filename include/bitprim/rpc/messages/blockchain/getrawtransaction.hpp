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

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETRAWTRANSACTION_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETRAWTRANSACTION_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/blockchain/getspentinfo.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {
    

// RPC CODE
// ----------------------------------------------------------------------------
// TODO: move this code to a better place

inline
std::string get_txn_type(const libbitcoin::chain::script& script) {
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

inline
bool json_in_getrawtransaction(nlohmann::json const& json_object, std::string& tx_id, bool& verbose) {
    if (json_object["params"].size() == 0)
        return false;
        
    verbose = false;
    try {
        tx_id = json_object["params"][0];
        if (json_object["params"].size() == 2) {
            if (json_object["params"][1].is_boolean()) {
                verbose = json_object["params"][1];
            } else if (json_object["params"][1].is_number() && json_object["params"][1].get<uint32_t>() == 1) {
                verbose = true;
            }
        }
    } catch (const std::exception & e) {
        return false;
    }
    return true;
}

template <typename Blockchain>
bool getrawtransaction(nlohmann::json& json_object, int& error, std::string& error_code, std::string const& txid, const bool verbose, Blockchain const& chain, bool use_testnet_rules) {
    libbitcoin::hash_digest hash;
    if (libbitcoin::decode_hash(hash, txid)) {

        if (verbose) {
            boost::latch latch(2);
            chain.fetch_transaction(hash, false,
                [&](const libbitcoin::code &ec, libbitcoin::transaction_const_ptr tx_ptr, size_t index,
                    size_t height) {
                if (ec == libbitcoin::error::success) {
                    json_object["hex"] = libbitcoin::encode_base16(tx_ptr->to_data(/*version is not used*/ 0));
                    json_object["txid"] = txid;
                    json_object["hash"] = txid;
                    json_object["size"] = tx_ptr->serialized_size(/*version is not used*/ 0);
                    json_object["version"] = tx_ptr->version();
                    json_object["locktime"] = tx_ptr->locktime();

                    int vin = 0;
                    for (const auto & in : tx_ptr->inputs()) {
                        if (tx_ptr->is_coinbase()) {
                            json_object["vin"][vin]["coinbase"] = libbitcoin::encode_base16(in.script().to_data(0));
                        }
                        else {
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
                                    json_object["vin"][vin]["value"] = output.value() / (double)100000000;
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
                    for (const auto & out : tx_ptr->outputs()) {
                        json_object["vout"][i]["value"] = out.value() / (double)100000000;
                        json_object["vout"][i]["valueSat"] = out.value();
                        json_object["vout"][i]["n"] = i;
                        json_object["vout"][i]["scriptPubKey"]["asm"] = out.script().to_string(0);
                        json_object["vout"][i]["scriptPubKey"]["hex"] = libbitcoin::encode_base16(out.script().to_data(0));

                        uint8_t reqsig = 1;
                        std::string type = get_txn_type(out.script());
                        if (type == "pay_multisig" || type == "sign_multisig") {
                            // TODO: check if it's working for multisig (see ExtractDestinations in bitcoind)
                            reqsig = static_cast<uint8_t>(out.script().operations()[0].code());
                        }
                        json_object["vout"][i]["scriptPubKey"]["reqSigs"] = (int)reqsig;
                        json_object["vout"][i]["scriptPubKey"]["type"] = type;
                        json_object["vout"][i]["scriptPubKey"]["addresses"][0] = out.address(use_testnet_rules).encoded();

                        // SPENT INFO
                        nlohmann::json spent;
                        boost::latch latch2(2);
                        chain.fetch_spend(libbitcoin::chain::output_point(tx_ptr->hash(), i), [&](const libbitcoin::code &ec, libbitcoin::chain::input_point input) {
                            if (ec == libbitcoin::error::not_found) {
                                // Output not spent
                                json_object["vout"][i]["spentTxId"] = spent["txid"];
                                json_object["vout"][i]["spentIndex"] = spent["index"];
                                json_object["vout"][i]["spentHeight"] = spent["height"];

                            }
                            latch2.count_down();
                        });
                        latch2.count_down_and_wait();
                        ++i;
                    }

                    if (index != libbitcoin::database::transaction_database::unconfirmed) {
                        //confirmed txn
                        boost::latch latch(2);
                        chain.fetch_block_hash_timestamp(height, [&](const libbitcoin::code &ec, const libbitcoin::hash_digest& h, uint32_t time, size_t block_height) {
                            if (ec == libbitcoin::error::success) {
                                json_object["blockhash"] = libbitcoin::encode_hash(h);
                                json_object["height"] = height;
                                json_object["time"] = time;
                                json_object["blocktime"] = time;
                            }
                            latch.count_down();
                        });
                        latch.count_down_and_wait();
                        boost::latch latch3(2);
                        chain.fetch_last_height(
                            [&](std::error_code const &ec, size_t last_height) {
                            json_object["confirmations"] = 1 + last_height - height;
                            latch3.count_down();
                        });
                        latch3.count_down_and_wait();
                    }

                    else {
                        //unconfirmed txn
                        json_object["height"] = -1;
                        json_object["confirmations"] = 0;
                    }
                }
                else {
                    error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
                    error_code = "No information available about transaction";
                }
                latch.count_down();
            });
            latch.count_down_and_wait();
        }
        else {
            // No verbose
            boost::latch latch(2);
            chain.fetch_transaction(hash, false,
                [&](const libbitcoin::code &ec, libbitcoin::transaction_const_ptr tx_ptr, size_t index,
                    size_t height) {
                if (ec == libbitcoin::error::success) {
                    json_object = libbitcoin::encode_base16(tx_ptr->to_data(/*version is not used*/ 0));
                }
                else {
                    error = bitprim::RPC_INVALID_ADDRESS_OR_KEY;
                    error_code = "No information available about transaction";
                }
                latch.count_down();
            });
            latch.count_down_and_wait();
        }
    }
    else {
        error = bitprim::RPC_INVALID_PARAMETER;
        error_code = "Invalid transaction hash";
    }

    if (error != 0)
        return false;
    return true;

}

template <typename Blockchain>
nlohmann::json process_getrawtransaction(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string tx_id;
    bool verbose;
    if (!json_in_getrawtransaction(json_in, tx_id, verbose)) //if false return error
    {
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "getrawtransaction \"txid\" ( verbose )\n"

            "\nNOTE: By default this function only works for mempool "
            "transactions. If the -txindex option is\n"
            "enabled, it also works for blockchain transactions.\n"
            "DEPRECATED: for now, it also works for transactions with unspent "
            "outputs.\n"

            "\nReturn the raw transaction data.\n"
            "\nIf verbose is 'true', returns an Object with information about "
            "'txid'.\n"
            "If verbose is 'false' or omitted, returns a string that is "
            "serialized, hex-encoded data for 'txid'.\n"

            "\nArguments:\n"
            "1. \"txid\"      (string, required) The transaction id\n"
            "2. verbose       (bool, optional, default=false) If false, return "
            "a string, otherwise return a json object\n"

            "\nResult (if verbose is not set or set to false):\n"
            "\"data\"      (string) The serialized, hex-encoded data for "
            "'txid'\n"

            "\nResult (if verbose is set to true):\n"
            "{\n"
            "  \"hex\" : \"data\",       (string) The serialized, hex-encoded "
            "data for 'txid'\n"
            "  \"txid\" : \"id\",        (string) The transaction id (same as "
            "provided)\n"
            "  \"hash\" : \"id\",        (string) The transaction hash "
            "(differs from txid for witness transactions)\n"
            "  \"size\" : n,             (numeric) The serialized transaction "
            "size\n"
            "  \"version\" : n,          (numeric) The version\n"
            "  \"locktime\" : ttt,       (numeric) The lock time\n"
            "  \"vin\" : [               (array of json objects)\n"
            "     {\n"
            "       \"txid\": \"id\",    (string) The transaction id\n"
            "       \"vout\": n,         (numeric) \n"
            "       \"scriptSig\": {     (json object) The script\n"
            "         \"asm\": \"asm\",  (string) asm\n"
            "         \"hex\": \"hex\"   (string) hex\n"
            "       },\n"
            "       \"sequence\": n      (numeric) The script sequence number\n"
            "     }\n"
            "     ,...\n"
            "  ],\n"
            "  \"vout\" : [              (array of json objects)\n"
            "     {\n"
            //TODO use correct currency unit
            "       \"value\" : x.xxx,            (numeric) The value in BCC\n"
            "       \"n\" : n,                    (numeric) index\n"
            "       \"scriptPubKey\" : {          (json object)\n"
            "         \"asm\" : \"asm\",          (string) the asm\n"
            "         \"hex\" : \"hex\",          (string) the hex\n"
            "         \"reqSigs\" : n,            (numeric) The required sigs\n"
            "         \"type\" : \"pubkeyhash\",  (string) The type, eg "
            "'pubkeyhash'\n"
            "         \"addresses\" : [           (json array of string)\n"
            "           \"address\"        (string) bitcoin address\n"
            "           ,...\n"
            "         ]\n"
            "       }\n"
            "     }\n"
            "     ,...\n"
            "  ],\n"
            "  \"blockhash\" : \"hash\",   (string) the block hash\n"
            "  \"confirmations\" : n,      (numeric) The confirmations\n"
            "  \"time\" : ttt,             (numeric) The transaction time in "
            "seconds since epoch (Jan 1 1970 GMT)\n"
            "  \"blocktime\" : ttt         (numeric) The block time in seconds "
            "since epoch (Jan 1 1970 GMT)\n"
            "}\n";
        return container;
    }

    if (getrawtransaction(result, error, error_code, tx_id, verbose, chain, use_testnet_rules)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETRAWTRANSACTION_HPP_

// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_MINING_SUBMITBLOCK_HPP_
#define KTH_RPC_MESSAGES_MINING_SUBMITBLOCK_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

inline
bool json_in_submitblock(nlohmann::json const& json_object, std::string& block_hex_str) {
    if (json_object["params"].size() == 0)
        return false;
    try {
        block_hex_str = json_object["params"][0].get<std::string>();
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;
}

inline
void handle_organize(const kth::code& ec) {
    if (ec)
        std::cout << "Failed to submit block" << std::endl;
    else std::cout << "Block submited successfully" << std::endl;
}

static
void setcoinbasereserved(std::shared_ptr<kd::message::block> block){
    kth::data_chunk stack_s(32);
    for(auto& data : stack_s){
        data = (uint8_t)0;
    }
    kth::data_stack stack{};
    stack.insert(stack.begin(), stack_s);
#ifndef KTH_CURRENCY_BCH
    block->transactions()[0].inputs()[0].set_witness(kth::domain::chain::witness(stack));
#endif
}

template <typename Blockchain>
bool submitblock(nlohmann::json& json_object, int& error, std::string& error_code, std::string const& incoming_hex, bool use_testnet_rules, Blockchain& chain) {
    auto const block = std::make_shared<kd::message::block>();
    kth::data_chunk out;
    kth::decode_base16(out, incoming_hex);
    if (domain::entity_from_data(*block, out, 1)) {
#ifndef KTH_CURRENCY_BCH
        if ( ! block->transactions()[0].is_segregated()) {
            setcoinbasereserved(block);
        }
#endif
       chain.organize(block, [&](const kth::code & ec) {
            if (ec) {
                error = ec.value();
                error_code = "Failed to submit block.";
            }
        });
    }
    else {
        error = kth::RPC_DESERIALIZATION_ERROR;
        error_code = "Block decode failed";
    }

    if (error != 0){
        LOG_WARNING("rpc", "Failed to Submit Block [Error code: ", error, "]");
        return false;
    }

    LOG_INFO("rpc", "Block submitted successfully [", kth::encode_hash(block->hash()), "]");

    return true;
}

template <typename Blockchain>
nlohmann::json process_submitblock(nlohmann::json const& json_in, Blockchain& chain, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string block_str;
    if ( ! json_in_submitblock(json_in, block_str)) //if false return error
    {
        container["result"];
        container["error"]["code"] = kth::RPC_MISC_ERROR;
        container["error"]["message"] = "submitblock \"hexdata\" ( \"jsonparametersobject\" )\n\nAttempts to submit new block to network.\nThe 'jsonparametersobject' parameter is currently ignored.\nSee https://en.bitcoin.it/wiki/BIP_0022 for full specification.\n\nArguments\n1. \"hexdata\"    (string, required) the hex-encoded block data to submit\n2. \"jsonparametersobject\"     (string, optional) object of optional parameters\n    {\n      \"workid\" : \"id\"    (string, optional) if the server provided a workid, it MUST be included with submissions\n    }\n\nResult:\n\nExamples:\n> bitcoin-cli submitblock \"mydata\"\n> curl --user myusername --data-binary '{\"jsonrpc\": \"1.0\", \"id\":\"curltest\", \"method\": \"submitblock\", \"params\": [\"mydata\"] }' -H 'content-type: text/plain;' http://127.0.0.1:8332/\n";
        return container;
    }

    if (submitblock(result, error, error_code, block_str, use_testnet_rules, chain))
    {
        container["result"] = result;
        container["error"];
    }
    else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif // KTH_RPC_MESSAGES_MINING_SUBMITBLOCK_HPP_

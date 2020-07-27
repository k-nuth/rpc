// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_WALLET_CREATETRANSACTION_HPP_
#define KTH_RPC_MESSAGES_WALLET_CREATETRANSACTION_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <kth/domain/wallet/transaction_functions.hpp>

namespace kth {

inline
bool json_in_createtransaction(nlohmann::json const& json_object, 
                              std::vector<kth::domain::chain::input_point>& outputs_to_spend,  
                              std::vector<std::pair<kth::domain::wallet::payment_address, uint64_t>>& outputs,
                              kth::domain::chain::output::list& extra_outputs) {

    auto const& size = json_object["params"].size();
    if (size == 0) {
        return false;
    }

    try {
        for (auto const& o : json_object["params"]["origin"]) {
            int index = o["output_index"];
            std::string hash_str = o["output_hash"];
            kth::hash_digest hash_to_spend;
            kth::decode_hash(hash_to_spend, hash_str);
            outputs_to_spend.push_back({hash_to_spend, uint32_t(index)});
        }

        for (auto const& d : json_object["params"]["dests"]) {
            // Implicit json to string conversion
            std::string addr = d["addr"];
            outputs.push_back({kth::domain::wallet::payment_address(addr), d["amount"]});
        }

        for (auto const& extra : json_object["params"]["extra_outputs"]){
            kth::data_chunk script_string;
            kth::decode_base16(script_string, extra["script"]);
            kth::domain::chain::script script;
            domain::entity_from_data(script, script_string, false);

            extra_outputs.push_back({extra["amount"], script});
        }
    } catch (std::exception const& e) {
        // std::cout << e.what() << std::endl;
        return false;
    }
    return true;

}

template <typename Blockchain>
bool createtransaction(nlohmann::json& json_object, int& error, std::string& error_code,
                       std::vector<kth::domain::chain::input_point>& outputs_to_spend,
                       std::vector<std::pair<kth::domain::wallet::payment_address, uint64_t>>& outputs,
                       kth::domain::chain::output::list& extra_outputs, bool use_testnet_rules, Blockchain& chain)
{
    auto res = kth::domain::wallet::tx_encode(outputs_to_spend, outputs, extra_outputs);
    if (res.first != kth::error::success) {
        return false;
    }

    auto str = kth::encode_base16(res.second.to_data(true));
    json_object = str;
    return true;
}

template <typename Blockchain>
nlohmann::json process_createtransaction(nlohmann::json const& json_in, Blockchain& chain, bool use_testnet_rules) {

    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<kth::domain::chain::input_point> outputs_to_spend;
    std::vector<std::pair<kth::domain::wallet::payment_address, uint64_t>> outputs;
    kth::domain::chain::output::list extra_outputs;

    if ( ! json_in_createtransaction(json_in, outputs_to_spend, outputs, extra_outputs)) { //if false return error
        container["result"];
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (createtransaction(result, error, error_code, outputs_to_spend, outputs, extra_outputs, use_testnet_rules, chain)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_WALLET_CREATETRANSACTION_HPP_

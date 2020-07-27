// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSMEMPOOL_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSMEMPOOL_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>
#include <boost/thread/latch.hpp>
#include <kth/rpc/messages/error_codes.hpp>

namespace kth {

inline
bool json_in_getaddressmempool(nlohmann::json const& json_object, std::vector<std::string>& payment_address) {
    // Example:
    // curl --user bitcoin:local321 --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getaddressmempool", "params": [{"addresses": ["mkR6TH68C5DXgjQDn7eDjTRFwNvo5V5zdA"]}] }' -H 'content-type: text/plain;' http://127.0.0.1:18332/
 
    if (json_object["params"].size() == 0)
        return false;

    // TODO: if the payment_address are invalid return
    //       error = kth::RPC_INVALID_ADDRESS_OR_KEY;
    //       error_code = "No information available for address";
    // instead of calling getaddressmempool and returning an empty json
    try {
        auto temp = json_object["params"][0];
        if (temp.is_object()){
            for (auto const & addr : temp["addresses"]){
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

template <typename Blockchain>
bool getaddressmempool(nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, Blockchain const& chain, bool use_testnet_rules) {
#ifdef KTH_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
#endif

    json_object = nlohmann::json::array();
    auto const res = chain.get_mempool_transactions(payment_addresses, use_testnet_rules, witness);

    size_t i = 0;
    for (auto const& r : res) {
        json_object[i]["address"] = r.address();
        json_object[i]["txid"] = r.hash();
        json_object[i]["index"] = r.index();
        json_object[i]["satoshis"] = r.satoshis();
        json_object[i]["timestamp"] = r.timestamp();
        if (r.previous_output_hash() != ""){
            json_object[i]["prevtxid"] = r.previous_output_hash();
            json_object[i]["prevout"] = r.previous_output_index();
        }
        ++i;
    }

    return true;
}

template <typename Blockchain>
nlohmann::json process_getaddressmempool(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules) {
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<std::string> payment_addresses;
    if ( ! json_in_getaddressmempool(json_in, payment_addresses))
    {
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
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
    }
    else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSMEMPOOL_HPP_

// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_KEOKEN_GETKEOKENADDRESS_HPP_
#define KTH_RPC_MESSAGES_KEOKEN_GETKEOKENADDRESS_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <kth/rpc/messages/keoken/keokenutils.hpp>

namespace kth {

inline
bool json_in_getkeokenaddress(nlohmann::json const& json_object, std::string& payment_address, size_t& index_start, size_t& index_end)
{
    // Example:
    // curl --user bitcoin:local321 --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getkeokenaddress", "params": {"address": "mvBZZ2s7yCQmRN7FoYkHDXdXpWqtFydXUg", "index_start":1, "index_end":10}] }' -H 'content-type: text/plain;' http://127.0.0.1:8332/
    // curl --user bitcoin:local321 --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getkeokenaddress", "params": "mvBZZ2s7yCQmRN7FoYkHDXdXpWqtFydXUg" }' -H 'content-type: text/plain;' http://127.0.0.1:8332/

    if (json_object["params"].size() == 0)
        return false;

    index_start = 0;
    index_end = kth::max_size_t;
    try {
        auto temp = json_object["params"];
        if (temp.is_object()) {
            if ( ! temp["index_start"].is_null()) {
                index_start = temp["index_start"].get<size_t>();
            }
            if ( ! temp["index_end"].is_null()) {
                index_end = temp["index_end"].get<size_t>();
            }
            if ( ! temp["address"].is_null()) {
                payment_address = temp["address"].get<std::string>();
            }
        } else {
            payment_address = json_object["params"].get<std::string>();
        }
    }
    catch (const std::exception & e) {
        LOG_ERROR("rpc", "Malformed RPC message Raised an exception [", e.what(), "]");
        return false;
    }

    return true;
}

template <typename Blockchain>
bool getkeokenaddress(nlohmann::json& json_object, int& error, std::string& error_code, std::string const& payment_address, size_t& index_start, size_t& index_end, size_t keoken_start, Blockchain const& chain, bool use_testnet_rules)
{
#ifdef KTH_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
#endif

    int i = 0;
    kth::domain::wallet::payment_address address(payment_address);
    if (address)
    {
        boost::latch latch(2);
        chain.fetch_keoken_history(address, INT_MAX, keoken_start, [&](const kth::code &ec,
            std::shared_ptr <std::vector<kth::transaction_const_ptr>> keoken_txs) {
                if (ec == kth::error::success) {
                    size_t i = 0;
                    size_t limit = index_end > (*keoken_txs).size() ? (*keoken_txs).size() : index_end;
                    size_t j = 0;
                    for(size_t i = index_start; i < limit; ++i){
                        json_object["transactions"][j] = kth::decode_keoken(chain, (*keoken_txs)[i], use_testnet_rules);
                        ++j;
                    }

                    json_object["count"] = (*keoken_txs).size();
                }
                else {
                    error = kth::RPC_INVALID_ADDRESS_OR_KEY;
                    error_code = "No information available for address " + address;
                }
            latch.count_down();
        });
        latch.count_down_and_wait();
    }
    else {
        error = kth::RPC_INVALID_ADDRESS_OR_KEY;
        error_code = "Invalid address";
    }

    if (error != 0)
        return false;

    return true;
}

template <typename Blockchain>
nlohmann::json process_getkeokenaddress(nlohmann::json const& json_in, Blockchain const& chain, size_t keoken_start,bool use_testnet_rules)
{
    nlohmann::json container, result;

    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string payment_address;
    size_t index_start;
    size_t index_end;
    if ( ! json_in_getkeokenaddress(json_in, payment_address, index_start, index_end))
    {
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (getkeokenaddress(result, error, error_code, payment_address, index_start, index_end, keoken_start, chain, use_testnet_rules))
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

#endif //KTH_RPC_MESSAGES_KEOKEN_GETKEOKENADDRESS_HPP_

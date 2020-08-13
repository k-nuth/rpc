// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSTXIDS_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSTXIDS_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

inline
bool json_in_getaddresstxids(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height) {
    if (json_object["params"].size() == 0)
        return false;

    start_height = 0;
    end_height = kth::max_size_t;
    try {
        auto temp = json_object["params"][0];
        if (temp.is_object()) {
            if ( ! temp["start"].is_null()) {
                start_height = temp["start"];
            }
            if ( ! temp["end"].is_null()) {
                end_height = temp["end"];
            }

            for (auto const & addr : temp["addresses"]) {
                payment_address.push_back(addr);
            }
        }
        else {
            //Only one address:
            payment_address.push_back(json_object["params"][0].get<std::string>());
        }
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;
}

template <typename Blockchain>
bool getaddresstxids(nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, size_t const& start_height, size_t const& end_height, Blockchain const& chain)
{
    int i = 0;
    for (auto const & payment_address : payment_addresses) {
        kth::domain::wallet::payment_address address(payment_address);
        if (address)
        {
            boost::latch latch(2);
            chain.fetch_confirmed_transactions(address, INT_MAX, start_height,
                [&](const kth::code &ec, const std::vector<kth::hash_digest>& history_list) {
                if (ec == kth::error::success) {
                    // TODO: remove this if the new code pass the tests
                    //                                    for (auto const & history : history_list) {
                    //                                        kth::hash_digest temp;
                    //                                        if (history.height < end_height) {
                    //                                            // If a txns spend more than one utxo from the address, just add the first one
                    //                                            temp = history.point.hash();
                    //                                            if (std::find (unique_data.begin(), unique_data.end(), history.point.hash()) == unique_data.end()){
                    //                                                unique_data.push_back(temp);
                    //                                            }
                    //                                        }
                    //                                    }
                    for (auto it = history_list.rbegin(); it != history_list.rend(); ++it) {
                        json_object[i] = kth::encode_hash(*it);
                        ++i;
                    }
                }
                else
                {
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
    }
    if (error != 0)
        return false;

    return true;
}

template <typename Blockchain>
nlohmann::json process_getaddresstxids(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{
    nlohmann::json container;
    nlohmann::json result = nlohmann::json::array();

    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<std::string> payment_address;
    size_t start_height;
    size_t end_height;
    if ( ! json_in_getaddresstxids(json_in, payment_address, start_height, end_height)) {
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "getaddresstxids\n"
            "\nReturns the txids for an address(es) (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ]\n"
            "  \"start\" (number) The start block height\n"
            "  \"end\" (number) The end block height\n"
            "}\n"
            "\nResult:\n"
            "[\n"
            "  \"transactionid\"  (string) The transaction id\n"
            "  ,...\n"
            "]\n";
        return container;
    }

    if (getaddresstxids(result, error, error_code, payment_address, start_height, end_height, chain)) {
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

#endif //KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSTXIDS_HPP_

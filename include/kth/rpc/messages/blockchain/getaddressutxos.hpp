// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSUTXOS_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSUTXOS_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

inline
bool json_in_getaddressutxos(nlohmann::json const& json_object, std::vector<std::string>& payment_address, bool& chain_info) {
    // Example:
    //  curl --user bitcoin:local321 --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getaddressutxos", "params": [{"addresses": ["2N8Cdq7BcCCqb8KiA3HzYaGDLJ6tXmVjcZ1"], "chainInfo":true}] }' -H 'content-type: text/plain;' http://127.0.0.1:8332/

    if (json_object["params"].size() == 0)
        return false;

    chain_info = false;
    try {
        auto temp = json_object["params"][0];
        if (temp.is_object()) {
            if ( ! temp["chainInfo"].is_null()) {
                chain_info = temp["chainInfo"];
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
bool getaddressutxos(nlohmann::json& json_object, int& error, std::string& error_code, std::vector<std::string> const& payment_addresses, const bool chain_info, Blockchain const& chain, bool use_testnet_rules) {
#if defined(KTH_CURRENCY_BCH)
    bool witness = false;
#else
    bool witness = true;
#endif

    boost::latch latch(2);
    nlohmann::json temp_utxos, utxos;
    int i = 0;
    for (auto const & payment_address : payment_addresses) {
        kth::domain::wallet::payment_address address(payment_address);
        if (address)
        {
            chain.fetch_history(address, INT_MAX, 0, [&](const kth::code &ec,
                kth::domain::chain::history_compact::list history_compact_list) {
                if (ec == kth::error::success) {
                    for (auto const & history : history_compact_list) {
                        if (history.kind == kth::domain::chain::point_kind::output) {
                            // It's outpoint
                            boost::latch latch2(2);
                            chain.fetch_spend(history.point, [&](const kth::code &ec, kth::domain::chain::input_point input) {
                                if (ec == kth::error::not_found) {
                                    // Output not spent
                                    temp_utxos[i]["address"] = address.encoded();
                                    temp_utxos[i]["txid"] = kth::encode_hash(history.point.hash());
                                    temp_utxos[i]["outputIndex"] = history.point.index();
                                    temp_utxos[i]["satoshis"] = history.value;
                                    temp_utxos[i]["height"] = history.height;
                                    // We need to fetch the txn to get the script
                                    boost::latch latch3(2);
                                    chain.fetch_transaction(history.point.hash(), false, witness,
                                        [&](const kth::code &ec, kth::transaction_const_ptr tx_ptr, size_t index,
                                            size_t height) {
                                        if (ec == kth::error::success) {
                                            temp_utxos[i]["script"] = kth::encode_base16(tx_ptr->outputs().at(history.point.index()).script().to_data(0));
                                        }
                                        else {
                                            temp_utxos[i]["script"] = "";
                                        }
                                        latch3.count_down();
                                    });
                                    latch3.count_down_and_wait();
                                    ++i;
                                }
                                latch2.count_down();
                            });
                            latch2.count_down_and_wait();
                        }
                    }
                }
                else {
                    error = kth::RPC_INVALID_ADDRESS_OR_KEY;
                    error_code = "No information available for address " + address;
                }
                latch.count_down();
            });
            latch.count_down_and_wait();


            auto unconfirmed = chain.get_mempool_transactions(address.encoded(), use_testnet_rules, witness);
            for (int j = 0; j < i ; ++j ) {
              for(auto const& dependant : unconfirmed) {
                if ( temp_utxos[j]["txid"] == dependant.previous_output_hash()) {
                    temp_utxos[j]["txid"] = "";
                  }
                }
            }

            int k = 0;
            for (int j = 0; j < i ; ++j ) {
              if (temp_utxos[j]["txid"] != "") {
                utxos[k] = temp_utxos[j];
                ++k;
              }
            }

            for (auto const& r : unconfirmed) {
                if (std::stoi (r.satoshis(), nullptr, 10) > 0) {
                  bool used = false;

                  for(auto const& dependant : unconfirmed) {
                      if (dependant.previous_output_hash() == r.hash()) {
                          used = true;
                      }
                  }

                  if ( ! used) {
                      utxos[k]["address"] = r.address();
                      utxos[k]["txid"] = r.hash();
                      utxos[k]["outputIndex"] = r.index();
                      utxos[k]["satoshis"] = r.satoshis();
                      boost::latch latch3(2);
                      kth::hash_digest hash;
                      kth::decode_hash(hash,r.hash());
                      chain.fetch_transaction(hash, false, witness,
                                              [&](const kth::code &ec,
                                                  kth::transaction_const_ptr tx_ptr,
                                                  size_t index,
                                                  size_t height) {
                                                if (ec == kth::error::success) {
                                                    utxos[k]["script"] =
                                                    kth::encode_base16(tx_ptr->outputs().at(r.index()).script().to_data(
                                                    0));
                                                } else {
                                                    utxos[k]["script"] = "";
                                                }
                                                latch3.count_down();
                                              });
                      latch3.count_down_and_wait();
                      ++k;
                  }
                }
            }
        }
        else {
            error = kth::RPC_INVALID_ADDRESS_OR_KEY;
            error_code = "Invalid address";
        }
    }

    if (chain_info) {
        json_object["utxos"] = utxos;

        size_t height;
        boost::latch latch2(2);
        chain.fetch_last_height([&](const kth::code &ec, size_t last_height) {
            if (ec == kth::error::success) {
                height = last_height;
            }
            latch2.count_down();
        });
        latch2.count_down_and_wait();

        boost::latch latch3(2);
        chain.fetch_block(height, witness, [&](const kth::code &ec, kth::block_const_ptr block, size_t) {
            if (ec == kth::error::success) {
                json_object["height"] = height;
                json_object["hash"] = kth::encode_hash(block->hash());
            }
            latch3.count_down();
        });
        latch3.count_down_and_wait();

    }
    else {
        json_object = utxos;
    }

    if (error != 0)
        return false;

    return true;
}

template <typename Blockchain>
nlohmann::json process_getaddressutxos(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{

    nlohmann::json container;
    nlohmann::json result = nlohmann::json::array();

    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::vector<std::string> payment_address;
    bool chain_info;
    if ( ! json_in_getaddressutxos(json_in, payment_address, chain_info)) {
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "getaddressutxos\n"
            "\nReturns all unspent outputs for an address (requires addressindex to be enabled).\n"
            "\nArguments:\n"
            "{\n"
            "  \"addresses\"\n"
            "    [\n"
            "      \"address\"  (string) The base58check encoded address\n"
            "      ,...\n"
            "    ],\n"
            "  \"chainInfo\"  (boolean) Include chain info with results\n"
            "}\n"
            "\nResult\n"
            "[\n"
            "  {\n"
            "    \"address\"  (string) The address base58check encoded\n"
            "    \"txid\"  (string) The output txid\n"
            "    \"height\"  (number) The block height\n"
            "    \"outputIndex\"  (number) The output index\n"
            "    \"script\"  (string) The script hex encoded\n"
            "    \"satoshis\"  (number) The number of satoshis of the output\n"
            "  }\n"
            "]\n";
        return container;
    }

    if (getaddressutxos(result, error, error_code, payment_address, chain_info, chain, use_testnet_rules)) {
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

#endif //KTH_RPC_MESSAGES_BLOCKCHAIN_GETADDRESSUTXOS_HPP_

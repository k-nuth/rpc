// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETKEOKENBLOCK_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETKEOKENBLOCK_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

#include <kth/rpc/messages/keoken/keokenutils.hpp>

namespace kth {

inline
bool json_in_getkeokenblock(nlohmann::json const& json_object, std::string & hash) {
    if (json_object["params"].size() == 0)
        return false;

    try {
        hash = json_object["params"][0];
    }
    catch (const std::exception & e) {
        LOG_ERROR("rpc", "Malformed RPC message Raised an exception [", e.what(), "]";
        return false;
    }
    return true;
}

template <typename Blockchain>
bool getkeokenblock(nlohmann::json& json_object, int& error, std::string& error_code, const std::string & block_hash, Blockchain const& chain, bool use_testnet_rules) {
#ifdef KTH_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
#endif
    nlohmann::json transactions;

    kth::hash_digest hash;
    if (kth::decode_hash(hash, block_hash)) {
        boost::latch latch(2);
        chain.fetch_block_keoken(hash, witness, [&](const kth::code &ec, kth::header_const_ptr header, size_t height, std::shared_ptr <std::vector <kth::transaction_const_ptr>> keoken_txs, uint64_t serialized_size, size_t transactions_total)
        {
            if (ec == kth::error::success) {
                //json_object["hash"] = block_hash;

                json_object["hash"] = block_hash;

                size_t top_height;
                chain.get_last_height(top_height);
                json_object["confirmations"] = top_height - height + 1;

                json_object["size"] = serialized_size;
                json_object["height"] = height;
                json_object["version"] = header->version();
                // TODO: encode the version to base 16
                json_object["versionHex"] = header->version();
                json_object["merkleroot"] = kth::encode_hash(header->merkle());
                json_object["time"] = header->timestamp();
                // TODO: get real median time
                json_object["mediantime"] = header->timestamp();
                json_object["nonce"] = header->nonce();
                // TODO: encode bits to base 16
                json_object["bits"] = header->bits();
                json_object["difficulty"] = bits_to_difficulty(header->bits());
                // TODO: validate that proof is chainwork
                // Optimizate the encoded to base 16
                std::stringstream ss;
                ss << std::setfill('0')
                   << std::nouppercase
                   << std::hex
                   << header->proof();
                json_object["chainwork"] = ss.str();
                json_object["previousblockhash"] = kth::encode_hash(header->previous_block_hash());

                json_object["nextblockhash"] = "";

                kth::hash_digest nexthash;
                if (chain.get_block_hash(nexthash, height+1))
                  json_object["nextblockhash"] = kth::encode_hash(nexthash);

                json_object["totaltransactions"] = transactions_total;
                int i = 0;
                for(auto& tx : (*keoken_txs)){
                    transactions[i] = decode_keoken(chain, tx, use_testnet_rules);
                    ++i;
                }
                json_object["transactions"] = transactions;

            }
            else {
                if (ec == kth::error::not_found)
                {
                    error = kth::RPC_INVALID_ADDRESS_OR_KEY;
                    error_code = "Block not found";
                }
                else {
                    error = kth::RPC_INTERNAL_ERROR;
                    error_code = "Can't read block from disk";
                }
            }
            latch.count_down();
        });
        latch.count_down_and_wait();
    }
    else {
        error = kth::RPC_INVALID_PARAMETER;
        error_code = "Invalid block hash";
    }

    if (error != 0)
        return false;
    return true;

}


template <typename Blockchain>
nlohmann::json process_getkeokenblock(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{

    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    std::string hash;
    if ( ! json_in_getkeokenblock(json_in, hash)) //if false return error
    {
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
        container["error"]["message"] = "";
        return container;
    }

    if (getkeokenblock(result, error, error_code, hash, chain, use_testnet_rules))
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

#endif //KTH_RPC_MESSAGES_BLOCKCHAIN_GETKEOKENBLOCK_HPP_

// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKCHAININFO_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKCHAININFO_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {
    template <typename Blockchain>
    bool getblockchaininfo(nlohmann::json& json_object, int& error, std::string& error_code, Blockchain const& chain) {

#if defined(KTH_CURRENCY_BCH)
    bool witness = false;
#else
    bool witness = true;
#endif
        json_object["chain"] = "main";

        size_t top_height;
        kth::domain::message::header::ptr top;
        chain.get_last_height(top_height);

        boost::latch latch(2);
        
        // chain.fetch_block(top_height, witness, [&](const kth::code &ec, kth::block_const_ptr block, size_t height) {
        chain.fetch_block_header(top_height, [&](const kth::code &ec, kth::header_ptr header, size_t height) {
            if (ec == kth::error::success) {
                json_object["blocks"] = height;
                json_object["headers"] = height;
                json_object["bestblockhash"] = kth::encode_hash(header->hash());
                json_object["difficulty"] = bits_to_difficulty(header->bits());
                json_object["mediantime"] = header->timestamp(); //TODO Get medianpasttime
                json_object["verificationprogress"] = 1;
                std::stringstream ss;
                ss << std::setfill('0')
                    << std::nouppercase
                    << std::hex
                    << header->proof();
                json_object["chainwork"] = ss.str();
                json_object["pruned"] = false;
                json_object["pruneheight"] = 0;
                json_object["softforks"] = nlohmann::json::array(); //TODO Check softforks
                json_object["bip9_softforks"] = nlohmann::json::array(); //TODO Check softforks

            }
            latch.count_down();
        });
        latch.count_down_and_wait();
        return true;
    }

    template <typename Blockchain>
    nlohmann::json process_getblockchaininfo(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules) {
        nlohmann::json container, result;
        container["id"] = json_in["id"];

        int error = 0;
        std::string error_code;

        if (getblockchaininfo(result, error, error_code, chain))
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

}

#endif

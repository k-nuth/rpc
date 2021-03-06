// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKCOUNT_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKCOUNT_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

    template <typename Blockchain>
    bool getblockcount(nlohmann::json& json_object, int& error, std::string& error_code, Blockchain const& chain) {
        size_t top_height;
        chain.get_last_height(top_height);
        json_object = top_height;
        return true;
    }

    template <typename Blockchain>
    nlohmann::json process_getblockcount(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules) {
        nlohmann::json container, result;
        container["id"] = json_in["id"];

        int error = 0;
        std::string error_code;

        if (getblockcount(result, error, error_code, chain))
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

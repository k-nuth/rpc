// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_BLOCKCHAIN_GETCHAINTIPS_HPP_
#define KTH_RPC_MESSAGES_BLOCKCHAIN_GETCHAINTIPS_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

    template <typename Blockchain>
    bool getchaintips(nlohmann::json& json_object, int& error, std::string& error_code, Blockchain const& chain)
    {
        /*
        "getchaintips\n"
        "Return information about all known tips in the block tree,"
        " including the main chain as well as orphaned branches.\n"
        "\nResult:\n"
        "[\n"
        "  {\n"
        "    \"height\": xxxx,         (numeric) height of the chain tip\n"
        "    \"hash\": \"xxxx\",         (string) block hash of the tip\n"
        "    \"branchlen\": 0          (numeric) zero for main chain\n"
        "    \"status\": \"active\"      (string) \"active\" for the main "
        "chain\n"
        "  },\n"
        "  {\n"
        "    \"height\": xxxx,\n"
        "    \"hash\": \"xxxx\",\n"
        "    \"branchlen\": 1          (numeric) length of branch "
        "connecting the tip to the main chain\n"
        "    \"status\": \"xxxx\"        (string) status of the chain "
        "(active, valid-fork, valid-headers, headers-only, invalid)\n"
        "  }\n"
        "]\n"
        "Possible values for status:\n"
        "1.  \"invalid\"               This branch contains at least one "
        "invalid block\n"
        "2.  \"headers-only\"          Not all blocks for this branch are "
        "available, but the headers are valid\n"
        "3.  \"valid-headers\"         All blocks are available for this "
        "branch, but they were never fully validated\n"
        "4.  \"valid-fork\"            This branch is not part of the "
        "active chain, but is fully validated\n"
        "5.  \"active\"                This is the tip of the active main "
        "chain, which is certainly valid\n"
        "\nExamples:\n" +
        HelpExampleCli("getchaintips", "") +
        HelpExampleRpc("getchaintips", ""));
        }
        */

        nlohmann::json active;
        size_t top_height;
        kth::domain::message::header::ptr top;
        chain.get_last_height(top_height);
        if (getblockheader(top_height, top, chain) != kth::error::success) {
            return false;
        }
        active["height"] = top_height;
        active["hash"] = kth::encode_hash(top->hash());
        active["branchlen"] = 0;
        active["status"] = "active";
        json_object[0] = active;
        return true;
    }

    template <typename Blockchain>
    nlohmann::json process_getchaintips(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
    {
        nlohmann::json container, result;
        container["id"] = json_in["id"];

        int error = 0;
        std::string error_code;

        if (getchaintips(result, error, error_code, chain))
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

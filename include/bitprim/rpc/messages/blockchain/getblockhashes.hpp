/**
* Copyright (c) 2017 Bitprim developers (see AUTHORS)
*
* This file is part of bitprim-node.
*
* bitprim-node is free software: you can redistribute it and/or
* modify it under the terms of the GNU Affero General Public License with
* additional permissions to the one published by the Free Software
* Foundation, either version 3 of the License, or (at your option)
* any later version. For more information see LICENSE.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASHES_HPP_
#define BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASHES_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

inline
bool json_in_getblockhashes(nlohmann::json const& json_object, uint32_t& time_high, uint32_t& time_low, bool& no_orphans, bool& logical_times) {
    if (json_object["params"].size() == 0)
        return false;

    no_orphans = true;
    logical_times = false;
    try {
        time_high = json_object["params"][0].get<uint32_t>();
        time_low = json_object["params"][1].get<uint32_t>();

        // TODO: params[2] should work if only one parameter is sent, fix the size() == 2
        if (!json_object["params"][2].is_null() && json_object["params"][2].is_object() && json_object["params"][2].size() == 2) {
            no_orphans = json_object["params"][2]["noOrphans"];
            logical_times = json_object["params"][2]["logicalTimes"];
        }
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;
}

template <typename Blockchain>
void update_mid(size_t top_height, size_t low_height, size_t& mid, libbitcoin::message::header::ptr& header, Blockchain const& chain) {
    mid = (top_height + low_height) / 2;
    getblockheader(mid, header, chain);
}

template <typename Blockchain>
bool getblockhashes(nlohmann::json& json_object, int& error, std::string& error_code, uint32_t time_high, uint32_t time_low, bool no_orphans, bool logical_times, Blockchain const& chain)
{

    if (time_high < time_low)
    {
        error = RPC_INVALID_PARAMETER;
        error_code = "Parameter \"HIGH\" is smaller than \"LOW\"";
        return false;
    }

    libbitcoin::message::header::ptr genesis, top, mid_header;
    getblockheader(0, genesis, chain);
    uint32_t time_genesis = genesis->timestamp();

    if (time_high < time_genesis)
    {
        error = RPC_INVALID_PARAMETER;
        error_code = "Parameter \"HIGH\" is older than genesis timestamp.";
        return false;
    }

    if (time_low < time_genesis) time_low = time_genesis;


    size_t top_height;
    chain.get_last_height(top_height);
    getblockheader(top_height, top, chain);
    uint32_t time_top = top->timestamp();

    if (time_top < time_low)
    {
        error = RPC_INVALID_PARAMETER;
        error_code = "Parameter \"LOW\" is newer than top timestamp.";
        return false;
    }

    if (time_high > time_top) time_high = time_top;

    //TODO: return error when time_high - time_low >= 2*60*60*24 (2 days)

    size_t low_height = 0;
    size_t mid;
    update_mid(top_height, low_height, mid, mid_header, chain);

    while (!((mid_header->timestamp() >= time_low) && (mid_header->timestamp() <= time_high))) {
        if (mid_header->timestamp() > time_high) {
            top_height = mid;
            update_mid(top_height, low_height, mid, mid_header, chain);
        }
        if (mid_header->timestamp() < time_low) {
            low_height = mid;
            update_mid(top_height, low_height, mid, mid_header, chain);
        }
    }

    libbitcoin::message::header::ptr last_header_found = mid_header;
    uint32_t last_time_found = mid_header->timestamp();
    size_t last_height = mid;

    libbitcoin::hash_digest last_hash = last_header_found->hash();

    std::deque<std::pair<libbitcoin::hash_digest, uint32_t>> hashes;

    while (last_time_found <= time_high && last_height <= top_height) {
        hashes.push_back(std::make_pair(last_hash, last_time_found));
        last_height++;
        getblockhash_time(last_height, last_hash, last_time_found, chain);
    }

    last_height = mid - 1;
    if(getblockheader(last_height, last_header_found, chain) == libbitcoin::error::success)
    {
        last_time_found = last_header_found->timestamp();
        while (last_time_found >= time_low && last_height > 0) {
            hashes.push_front(std::make_pair(last_hash, last_time_found));
            last_height--;
            getblockhash_time(last_height, last_hash, last_time_found, chain);
        }
    }

    int i = 0;
    if (!logical_times) {
        for (const auto & h : hashes) {
            json_object[i] = libbitcoin::encode_hash(h.first);
            ++i;
        }
    }
    else {
        for (const auto & h : hashes) {
            json_object[i]["blockhash"] = libbitcoin::encode_hash(h.first);
            json_object[i]["timestamp"] = h.second;
            ++i;
        }
    }
    return true;
}

template <typename Blockchain>
nlohmann::json process_getblockhashes(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    uint32_t time_high;
    uint32_t time_low;
    bool no_orphans;
    bool logical_times;

    if (!json_in_getblockhashes(json_in, time_high, time_low, no_orphans, logical_times))
    {
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "getblockhashes timestamp\n"
            "\nReturns array of hashes of blocks within the timestamp range provided.\n"
            "\nArguments:\n"
            "1. high         (numeric, required) The newer block timestamp\n"
            "2. low          (numeric, required) The older block timestamp\n"
            "3. options      (string, required) A json object\n"
            "    {\n"
            "      \"noOrphans\":true   (boolean) will only include blocks on the main chain\n"
            "      \"logicalTimes\":true   (boolean) will include logical timestamps with hashes\n"
            "    }\n"
            "\nResult:\n"
            "[\n"
            "  \"hash\"         (string) The block hash\n"
            "]\n"
            "[\n"
            "  {\n"
            "    \"blockhash\": (string) The block hash\n"
            "    \"logicalts\": (numeric) The logical timestamp\n"
            "  }\n"
            "]\n";
        return container;
    }

    if (getblockhashes(result, error, error_code, time_high, time_low, no_orphans, logical_times, chain))
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

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_BLOCKCHAIN_GETBLOCKHASHES_HPP_

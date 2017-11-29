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

#include <bitprim/rpc/messages/blockchain/getblockhashes.hpp>
#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

bool json_in_getblockhashes(nlohmann::json const& json_object, uint32_t& time_high, uint32_t& time_low, bool& no_orphans, bool& logical_times){
    if (json_object["params"].size() == 0)
        return false;

    time_high = json_object["params"][0].get<uint32_t>();
    time_low = json_object["params"][1].get<uint32_t>();
    no_orphans = true;
    logical_times = false;
    // TODO: params[2] should work if only one parameter is sent, fix the size() == 2
    if (!json_object["params"][2].is_null() && json_object["params"][2].is_object() && json_object["params"][2].size() ==2){
        no_orphans = json_object["params"][2]["noOrphans"];
        logical_times = json_object["params"][2]["logicalTimes"];
    }
    return true;
}

void update_mid(size_t top_height, size_t low_height, size_t& mid, libbitcoin::message::header::ptr& header, libbitcoin::blockchain::block_chain const& chain){
    mid = (top_height + low_height) / 2;
    getblockheader(mid, header, chain);
}

bool getblockhashes(nlohmann::json& json_object, int& error, std::string& error_code, uint32_t time_high, uint32_t time_low, bool no_orphans, bool logical_times, libbitcoin::blockchain::block_chain const& chain)
{
    if(time_high < time_low)
    {
        error = RPC_INVALID_PARAMETER;
        error_code = "Parameter \"HIGH\" is smaller than \"LOW\"";
        return false;
    }

    libbitcoin::message::header::ptr genesis, top, mid_header;
    getblockheader(0, genesis, chain);
    uint32_t time_genesis = genesis->timestamp();

    if(time_high < time_genesis)
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

    if(time_top < time_low)
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

    while (!((mid_header->timestamp() >= time_low) && (mid_header->timestamp() <= time_high))){
        if (mid_header->timestamp() > time_high){
            top_height = mid;
            update_mid(top_height, low_height, mid, mid_header, chain);
        }
        if (mid_header->timestamp() < time_low){
            low_height = mid;
            update_mid(top_height, low_height, mid, mid_header, chain);
        }
    }

    libbitcoin::message::header::ptr last_header_found = mid_header;
    uint32_t last_time_found = mid_header->timestamp();
    size_t last_height = mid;

    std::deque<std::pair<libbitcoin::hash_digest, uint32_t>> hashes;

    while (last_time_found <= time_high && last_height <= top_height){
        hashes.push_back(std::make_pair(last_header_found->hash(), last_header_found->timestamp()));
        last_height++;
        getblockheader(last_height, last_header_found, chain);
        last_time_found = last_header_found->timestamp();
    }

    last_height = mid - 1;
    getblockheader(last_height, last_header_found, chain);
    last_time_found = last_header_found->timestamp();

    while (last_time_found >= time_low && last_height >=0){
        hashes.push_front(std::make_pair(last_header_found->hash(), last_header_found->timestamp()));
        last_height--;
        getblockheader(last_height, last_header_found, chain);
        last_time_found = last_header_found->timestamp();
    }


    int i = 0;
    if (!logical_times) {
        for (const auto & h : hashes) {
            json_object[i] = libbitcoin::encode_hash(h.first);
            ++i;
        }
    } else {
        for (const auto & h : hashes) {
            json_object[i]["blockhash"] = libbitcoin::encode_hash(h.first);
            json_object[i]["timestamp"] = h.second;
            ++i;
        }
    }
    return true;
}

nlohmann::json process_getblockhashes(nlohmann::json const& json_in, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules)
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
        //load error code
        //return
    }

    if (getblockhashes(result, error, error_code, time_high, time_low, no_orphans, logical_times, chain))
    {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

}

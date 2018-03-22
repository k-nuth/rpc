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

#ifndef BITPRIM_RPC_MESSAGES_MINING_GETBLOCKTEMPLATE_HPP_
#define BITPRIM_RPC_MESSAGES_MINING_GETBLOCKTEMPLATE_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitcoin/bitcoin/multi_crypto_support.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

template <typename Blockchain>
bool get_last_block_header(size_t& out_height, libbitcoin::message::header& out_header, Blockchain const& chain) {
    //size_t out_height;
    if (chain.get_last_height(out_height)) {
        return chain.get_header(out_header, out_height);
    }
    return false;
}

inline
uint32_t get_clock_now() {
    auto const now = std::chrono::high_resolution_clock::now();
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
}

/*constexpr*/
inline
uint64_t get_block_reward(uint32_t height) {
    auto subsidy = libbitcoin::initial_block_subsidy_satoshi();
    subsidy >>= (height / libbitcoin::subsidy_interval);
    return subsidy;
}

template <typename Blockchain>
bool getblocktemplate(nlohmann::json& json_object, int& error, std::string& error_code, size_t max_bytes, std::chrono::nanoseconds timeout, Blockchain const& chain) {

    json_object["capabilities"] = std::vector<std::string>{ "proposal" };
    json_object["version"] = 536870912;                          //TODO: hardcoded value
    json_object["rules"] = std::vector<std::string>{ "csv" }; //, "!segwit"};  //TODO!
    json_object["vbavailable"] = nlohmann::json::object();
    json_object["vbrequired"] = 0;

    static bool first_time = true;
    static size_t old_height = 0;
    static std::vector<libbitcoin::blockchain::block_chain::tx_mempool> tx_cache;
    static std::chrono::time_point<std::chrono::high_resolution_clock> cache_timestamp = std::chrono::high_resolution_clock::now();

    size_t last_height;
    chain.get_last_height(last_height);

    libbitcoin::message::header::ptr header;
    if (getblockheader(last_height, header, chain) != libbitcoin::error::success) {
        return false;
    }

    auto time_now = get_clock_now();
    json_object["curtime"] = time_now;
    json_object["mintime"] = chain.chain_state()->median_time_past() + 1;
    if (json_object["curtime"] < json_object["mintime"]) {
        json_object["curtime"] = json_object["mintime"];
    }

    auto const bits = chain.chain_state()->get_next_work_required(time_now);
    auto const height = last_height + 1;

    json_object["previousblockhash"] = libbitcoin::encode_hash(header->hash());

    json_object["sigoplimit"] = libbitcoin::get_max_block_sigops(); //OLD max_block_sigops; //TODO: this value is hardcoded using bitcoind pcap
    json_object["sizelimit"] = libbitcoin::get_max_block_size(); //OLD max_block_size;   //TODO: this value is hardcoded using bitcoind pcap
    json_object["weightlimit"] = libbitcoin::get_max_block_size();//                    //TODO: this value is hardcoded using bitcoind pcap

    auto now = std::chrono::high_resolution_clock::now();

    if (first_time || old_height != last_height ||
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - cache_timestamp) >= timeout)
    {
        first_time = false;
        old_height = last_height;
        tx_cache = chain.fetch_mempool_all(max_bytes);
        cache_timestamp = std::chrono::high_resolution_clock::now();
    }

    nlohmann::json transactions_json = nlohmann::json::array();

    uint64_t fees = 0;
    for (size_t i = 0; i < tx_cache.size(); ++i) {
        auto const& tx_mem = tx_cache[i];
        auto const& tx = std::get<0>(tx_mem);
        const auto tx_data = tx.to_data();
    
        transactions_json[i]["data"] = libbitcoin::encode_base16(tx_data);
        transactions_json[i]["txid"] = libbitcoin::encode_hash(tx.hash());
        transactions_json[i]["hash"] = libbitcoin::encode_hash(tx.hash());
        transactions_json[i]["depends"] = nlohmann::json::array(); //TODO CARGAR DEPS
        transactions_json[i]["fee"] = std::get<1>(tx_mem);
        transactions_json[i]["sigops"] = std::get<2>(tx_mem);
        transactions_json[i]["weight"] = tx_data.size();
        fees += std::get<1>(tx_mem);
    }

    json_object["transactions"] = transactions_json;

    auto coinbase_reward = get_block_reward(height);
    json_object["coinbasevalue"] = coinbase_reward + fees;
    json_object["coinbaseaux"]["flags"] = "";

    const auto header_bits = libbitcoin::chain::compact(bits);
    libbitcoin::uint256_t target(header_bits);

    std::ostringstream target_stream;
    target_stream << std::setfill('0') << std::setw(64) << std::hex << target << "\0" << std::dec;
    auto target_str = target_stream.str();
    char final_target[66];
    target_str.copy(final_target, 64);
    final_target[64] = '\0';
    json_object["target"] = final_target;
    json_object["mutable"] = std::vector<std::string>{ "time", "transactions", "prevblock" };
    json_object["noncerange"] = "00000000ffffffff";

    uint8_t rbits[9];
    sprintf((char*)rbits, "%08x", bits);
    json_object["bits"] = std::string((char*)rbits, 8);
    json_object["height"] = height;

    return true;
}


template <typename Blockchain>
nlohmann::json process_getblocktemplate(nlohmann::json const& json_in, Blockchain const& chain, bool use_testnet_rules) {

    nlohmann::json container, result;
    container["id"] = json_in["id"];

    int error = 0;
    std::string error_code;

    if (getblocktemplate(result, error, error_code, libbitcoin::get_max_block_size() - 20000, std::chrono::seconds(30), chain)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_MINING_GETBLOCKTEMPLATE_HPP_

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
uint64_t get_block_reward(uint32_t height, bool retarget=true) {
    auto subsidy = libbitcoin::initial_block_subsidy_satoshi();
    subsidy >>= (height / libbitcoin::subsidy_interval(retarget));
    return subsidy;
}

static
libbitcoin::hash_digest generate_merkle_root(std::vector<libbitcoin::hash_digest>& merkle)
{
    if (merkle.empty())
        return libbitcoin::null_hash;

    libbitcoin::hash_list update;
    merkle.insert(merkle.begin(),libbitcoin::hash_digest{});

    // Initial capacity is half of the original list (clear doesn't reset).
    update.reserve((merkle.size() + 1) / 2);

    while (merkle.size() > 1)
    {
        // If number of hashes is odd, duplicate last hash in the list.
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());

        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
            update.push_back(libbitcoin::bitcoin_hash(libbitcoin::build_chunk({ it[0], it[1] })));

        std::swap(merkle, update);
        update.clear();
    }

    // There is now only one item in the list.
    return merkle.front();
}

static
std::vector<uint8_t> create_default_witness_commitment(std::vector<libbitcoin::hash_digest>& merkle){
    libbitcoin::byte_array<6> scriptPubKey;
    scriptPubKey[0] = 0x6a;
    scriptPubKey[1] = 0x24;
    scriptPubKey[2] = 0xaa;
    scriptPubKey[3] = 0x21;
    scriptPubKey[4] = 0xa9;
    scriptPubKey[5] = 0xed;

    std::vector<uint8_t> bytes{};
    bytes.reserve(38);
    auto hash = libbitcoin::bitcoin_hash(libbitcoin::build_chunk({ generate_merkle_root(merkle), libbitcoin::hash_digest{} }));//libbitcoin::encode_base16(generate_merkle_root(witness_gen));
    bytes.insert(bytes.begin(), hash.begin(), hash.end());
    bytes.insert(bytes.begin(), scriptPubKey.begin(), scriptPubKey.end());
    return bytes;
}





template <typename Blockchain>
bool getblocktemplate(nlohmann::json& json_object, int& error, std::string& error_code, size_t max_bytes, std::chrono::nanoseconds timeout, Blockchain const& chain) {

    json_object["capabilities"] = std::vector<std::string>{ "proposal" };
    json_object["version"] = 536870912;                          //TODO: hardcoded value
#ifdef BITPRIM_CURRENCY_BCH
    json_object["rules"] = std::vector<std::string>{ "csv" };
#else
    json_object["rules"] = std::vector<std::string>{ "csv", "segwit" };
#endif
    json_object["vbavailable"] = nlohmann::json::object();
    json_object["vbrequired"] = 0;

    static bool first_time = true;
    static size_t old_height = 0;
    static std::vector<libbitcoin::blockchain::block_chain::tx_benefit> tx_cache;
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

#ifdef BITPRIM_CURRENCY_BCH
    json_object["sigoplimit"] = libbitcoin::get_max_block_sigops();
    //TODO(fernando): check what to do with the 2018-May-15 Hard Fork
    json_object["sizelimit"] = libbitcoin::get_max_block_size();
    json_object["weightlimit"] = libbitcoin::get_max_block_size();
#else
    json_object["sigoplimit"] = libbitcoin::max_fast_sigops;
    //TODO(fernando): check what to do with the 2018-May-15 Hard Fork
    json_object["sizelimit"] = libbitcoin::max_block_weight;
    json_object["weightlimit"] = libbitcoin::max_block_weight;
#endif

    auto now = std::chrono::high_resolution_clock::now();

    if (first_time || old_height != last_height ||
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - cache_timestamp) >= timeout)
    {
        first_time = false;
        old_height = last_height;
        tx_cache = chain.get_gbt_tx_list();
        cache_timestamp = std::chrono::high_resolution_clock::now();
    }

    nlohmann::json transactions_json = nlohmann::json::array();

#ifdef BITPRIM_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
    std::vector<libbitcoin::hash_digest> witness_gen(tx_cache.size());
#endif
    uint64_t fees = 0;
    for (size_t i = 0; i < tx_cache.size(); ++i) {
        auto const& tx_mem = tx_cache[i];
        transactions_json[i]["data"] = libbitcoin::encode_base16(tx_mem.tx_hex);
        transactions_json[i]["txid"] = libbitcoin::encode_hash(tx_mem.tx_id);
#ifdef BITPRIM_CURRENCY_BCH
        transactions_json[i]["hash"] = libbitcoin::encode_hash(tx_mem.tx_id);
#else
        transactions_json[i]["hash"] = libbitcoin::encode_hash(tx_mem.tx_hash);
        if(witness){
            witness_gen.insert(witness_gen.begin(), tx_mem.tx_hash);
        }
#endif
        transactions_json[i]["depends"] = nlohmann::json::array(); //TODO CARGAR DEPS
        transactions_json[i]["fee"] = tx_mem.tx_fees;
        transactions_json[i]["sigops"] = tx_mem.tx_sigops;
        transactions_json[i]["weight"] = tx_mem.tx_size;//tx_data.size();
        fees += tx_mem.tx_fees;

    }
#ifndef BITPRIM_CURRENCY_BCH
    if(witness)
    {
        json_object["default_witness_commitment"] = libbitcoin::encode_base16(create_default_witness_commitment(witness_gen));
    }
#endif


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

    //TODO(fernando): check what to do with the 2018-May-15 Hard Fork
    //TODO(fernando): hardcoded 20000
    if (getblocktemplate(result, error, error_code, libbitcoin::get_max_block_size() - 20000, std::chrono::seconds(5), chain)) {
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

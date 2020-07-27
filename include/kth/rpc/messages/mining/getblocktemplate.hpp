// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_MINING_GETBLOCKTEMPLATE_HPP_
#define KTH_RPC_MESSAGES_MINING_GETBLOCKTEMPLATE_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/domain/multi_crypto_support.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

template <typename Blockchain>
bool get_last_block_header(size_t& out_height, kth::domain::message::header& out_header, Blockchain const& chain) {
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
    auto subsidy = kth::initial_block_subsidy_satoshi();
    subsidy >>= (height / kth::subsidy_interval(retarget));
    return subsidy;
}

static
kth::hash_digest generate_merkle_root(std::vector<kth::hash_digest>& merkle)
{
    if (merkle.empty())
        return kth::null_hash;

    kth::hash_list update;
    merkle.insert(merkle.begin(),kth::hash_digest{});

    // Initial capacity is half of the original list (clear doesn't reset).
    update.reserve((merkle.size() + 1) / 2);

    while (merkle.size() > 1)
    {
        // If number of hashes is odd, duplicate last hash in the list.
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());

        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
            update.push_back(kth::bitcoin_hash(kth::build_chunk({ it[0], it[1] })));

        std::swap(merkle, update);
        update.clear();
    }

    // There is now only one item in the list.
    return merkle.front();
}

static
std::vector<uint8_t> create_default_witness_commitment(std::vector<kth::hash_digest>& merkle){
    kth::byte_array<6> scriptPubKey;
    scriptPubKey[0] = 0x6a;
    scriptPubKey[1] = 0x24;
    scriptPubKey[2] = 0xaa;
    scriptPubKey[3] = 0x21;
    scriptPubKey[4] = 0xa9;
    scriptPubKey[5] = 0xed;

    std::vector<uint8_t> bytes{};
    bytes.reserve(38);
    auto hash = kth::bitcoin_hash(kth::build_chunk({ generate_merkle_root(merkle), kth::hash_digest{} }));//kth::encode_base16(generate_merkle_root(witness_gen));
    bytes.insert(bytes.begin(), hash.begin(), hash.end());
    bytes.insert(bytes.begin(), scriptPubKey.begin(), scriptPubKey.end());
    return bytes;
}

template <typename Blockchain>
bool getblocktemplate(nlohmann::json& json_object, int& error, std::string& error_code, std::chrono::nanoseconds timeout, Blockchain const& chain) {

#ifdef KTH_CURRENCY_BCH
    bool witness = false;
#else
    bool witness = true;
#endif


    json_object["capabilities"] = std::vector<std::string>{ "proposal" };
    json_object["version"] = 536870912;                          //TODO: hardcoded value
#ifdef KTH_CURRENCY_BCH
    json_object["rules"] = std::vector<std::string>{ "csv" };
#else
    json_object["rules"] = std::vector<std::string>{ "csv", "segwit" };
#endif
    json_object["vbavailable"] = nlohmann::json::object();
    json_object["vbrequired"] = 0;

    static bool first_time = true;
    static size_t old_height = 0;
    static std::pair<std::vector<kth::mining::transaction_element>, uint64_t> get_block_template_data;
    static std::chrono::time_point<std::chrono::high_resolution_clock> cache_timestamp = std::chrono::high_resolution_clock::now();

    size_t last_height;
    kth::domain::message::header header;
    if ( ! get_last_block_header(last_height, header, chain)) {
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

    json_object["previousblockhash"] = kth::encode_hash(header.hash());

#ifdef KTH_CURRENCY_BCH
    json_object["sigoplimit"] = kth::get_max_block_sigops();
    //TODO(fernando): check what to do with the 2018-May-15 Hard Fork
    json_object["sizelimit"] = kth::get_max_block_size();
    json_object["weightlimit"] = kth::get_max_block_size();
#else
    json_object["sigoplimit"] = kth::max_fast_sigops;
    //TODO(fernando): check what to do with the 2018-May-15 Hard Fork
    json_object["sizelimit"] = kth::max_block_weight;
    json_object["weightlimit"] = kth::max_block_weight;
#endif

    auto now = std::chrono::high_resolution_clock::now();

    if (first_time || old_height != last_height ||
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - cache_timestamp) >= timeout) {
        first_time = false;
        old_height = last_height;
        get_block_template_data = chain.get_block_template();
        cache_timestamp = std::chrono::high_resolution_clock::now();
    }

    nlohmann::json transactions_json = nlohmann::json::array();

#if ! defined(KTH_CURRENCY_BCH)
    std::vector<kth::hash_digest> witness_gen;
    witness_gen.reserve(get_block_template_data.first.size());
#endif

    uint64_t fees = 0;
    for (size_t i = 0; i < get_block_template_data.first.size(); ++i) {
        auto const& tx_mem = get_block_template_data.first[i];
        transactions_json[i]["data"] = kth::encode_base16(tx_mem.raw());
        transactions_json[i]["txid"] = kth::encode_hash(tx_mem.txid());

#ifdef KTH_CURRENCY_BCH
        transactions_json[i]["hash"] = kth::encode_hash(tx_mem.txid());
#else
        transactions_json[i]["hash"] = kth::encode_hash(tx_mem.hash());
        if (witness) {
            // witness_gen.insert(witness_gen.end(), tx_mem.hash());
            witness_gen.push_back(tx_mem.hash());
        }
#endif
        transactions_json[i]["depends"] = nlohmann::json::array(); //TODO CARGAR DEPS
        transactions_json[i]["fee"] = tx_mem.fee();
        transactions_json[i]["sigops"] = tx_mem.sigops();
        transactions_json[i]["weight"] = tx_mem.size();
//        fees += tx_mem.tx_fees;

    }
#ifndef KTH_CURRENCY_BCH
    if (witness) {
        json_object["default_witness_commitment"] = kth::encode_base16(create_default_witness_commitment(witness_gen));
    }
#endif

    json_object["transactions"] = transactions_json;

    auto coinbase_reward = get_block_reward(height);
    json_object["coinbasevalue"] = coinbase_reward + get_block_template_data.second /* acum fees*/;
    json_object["coinbaseaux"]["flags"] = "";

    auto const header_bits = kth::domain::chain::compact(bits);
    kth::uint256_t target(header_bits);

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
    if (getblocktemplate(result, error, error_code, std::chrono::seconds(5), chain)) {
        container["result"] = result;
        container["error"];
    } else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace kth

#endif //KTH_RPC_MESSAGES_MINING_GETBLOCKTEMPLATE_HPP_

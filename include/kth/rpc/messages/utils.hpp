// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_RPC_MESSAGES_UTILS_HPP_
#define KTH_RPC_MESSAGES_UTILS_HPP_

#include <kth/blockchain/interface/block_chain.hpp>
#include <boost/thread/latch.hpp>

namespace kth {

double bits_to_difficulty (const uint32_t & bits);

    //kth::domain::chain::history::list expand(kth::domain::chain::history_compact::list& compact);


#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW_BLOCKS) || defined(KTH_DB_NEW_FULL)
template <typename Blockchain>
kth::code getblockhash_time(size_t i, kth::hash_digest& out_hash, uint32_t& out_time,Blockchain const& chain) {
    kth::code result;
    boost::latch latch(2);
    chain.fetch_block_hash_timestamp(i, [&](const kth::code &ec, const kth::hash_digest& h, uint32_t time, size_t height) {
        result = ec;
        if (ec == kth::error::success) {
            out_hash = h;
            out_time= time;
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
    return result;
}
#endif

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 

template <typename Blockchain>
kth::code getblockheader(size_t i, kth::domain::message::header::ptr& header, Blockchain const& chain) {
    kth::code result;
    boost::latch latch(2);

    chain.fetch_block_header(i, [&](const kth::code &ec, kth::domain::message::header::ptr h, size_t height) {
        result = ec;
        if (ec == kth::error::success) {
            header = h;
        }
        latch.count_down();
    });
    latch.count_down_and_wait();

    return result;
}

template <typename Blockchain>
std::tuple<bool, size_t, double> get_last_block_difficulty(Blockchain const& chain) {

    double diff = 1.0;
    size_t top_height;
    kth::domain::message::header::ptr top = nullptr;
    bool success = false;
    if (chain.get_last_height(top_height)) {
        auto ec = getblockheader(top_height, top, chain);
        if (ec == kth::error::success && top != nullptr) {
            success = true;
            auto bits = top->bits();
            int shift = (bits >> 24) & 0xff;
            diff = (double)0x0000ffff / (double)(bits & 0x00ffffff);
            while (shift < 29) {
                diff *= 256.0;
                ++shift;
            }
            while (shift > 29) {
                diff /= 256.0;
                --shift;
            }
        }
    }
    return std::make_tuple(success, top_height, diff);
}

#endif // defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 


inline
kth::ec_secret create_secret_from_seed(std::string const& seed_str) {
    kth::data_chunk seed;
    kth::decode_base16(seed, seed_str);
    kth::infrastructure::wallet::hd_private const key(seed);
    // Secret key
    kth::ec_secret secret_key(key.secret());
    return secret_key;
}

inline
kth::domain::wallet::ec_public secret_to_compressed_public(kth::ec_secret const& secret_key) {
    //Public key
    kth::ec_compressed point;
    kth::secret_to_public(point, secret_key);
    kth::domain::wallet::ec_public public_key(point, true /*compress*/);

    return public_key;
}

}

#endif

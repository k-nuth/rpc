/**
* Copyright (c) 2017-2018 Bitprim Inc.
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

#ifndef BITPRIM_RPC_MESSAGES_UTILS_HPP_
#define BITPRIM_RPC_MESSAGES_UTILS_HPP_

#include <bitcoin/blockchain/interface/block_chain.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

double bits_to_difficulty (const uint32_t & bits);

    //libbitcoin::chain::history::list expand(libbitcoin::chain::history_compact::list& compact);


#if defined(BITPRIM_DB_LEGACY) || defined(BITPRIM_DB_NEW_BLOCKS) 
template <typename Blockchain>
libbitcoin::code getblockhash_time(size_t i, libbitcoin::hash_digest& out_hash, uint32_t& out_time,Blockchain const& chain) {
    libbitcoin::code result;
    boost::latch latch(2);
    chain.fetch_block_hash_timestamp(i, [&](const libbitcoin::code &ec, const libbitcoin::hash_digest& h, uint32_t time, size_t height) {
        result = ec;
        if (ec == libbitcoin::error::success) {
            out_hash = h;
            out_time= time;
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
    return result;
}

template <typename Blockchain>
libbitcoin::code getblockheader(size_t i, libbitcoin::message::header::ptr& header, Blockchain const& chain) {
    libbitcoin::code result;
    boost::latch latch(2);

    chain.fetch_block_header(i, [&](const libbitcoin::code &ec, libbitcoin::message::header::ptr h, size_t height) {
        result = ec;
        if (ec == libbitcoin::error::success) {
            header = h;
        }
        latch.count_down();
    });
    latch.count_down_and_wait();

    return result;
}

#endif // defined(BITPRIM_DB_LEGACY) || defined(BITPRIM_DB_NEW_BLOCKS) 

#if defined(BITPRIM_DB_LEGACY) || defined(BITPRIM_DB_NEW) 

template <typename Blockchain>
std::tuple<bool, size_t, double> get_last_block_difficulty(Blockchain const& chain) {

    double diff = 1.0;
    size_t top_height;
    libbitcoin::message::header::ptr top = nullptr;
    bool success = false;
    if (chain.get_last_height(top_height)) {
        auto ec = getblockheader(top_height, top, chain);
        if (ec == libbitcoin::error::success && top != nullptr) {
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

#endif // defined(BITPRIM_DB_LEGACY) || defined(BITPRIM_DB_NEW) 


inline
libbitcoin::ec_secret create_secret_from_seed(std::string const& seed_str) {
    libbitcoin::data_chunk seed;
    libbitcoin::decode_base16(seed, seed_str);
    libbitcoin::wallet::hd_private const key(seed);
    // Secret key
    libbitcoin::ec_secret secret_key(key.secret());
    return secret_key;
}

inline
libbitcoin::wallet::ec_public secret_to_compressed_public(libbitcoin::ec_secret const& secret_key) {
  //Public key
  libbitcoin::ec_compressed point;
  libbitcoin::secret_to_public(point, secret_key);
  libbitcoin::wallet::ec_public public_key(point, true /*compress*/);

  return public_key;
}

}

#endif

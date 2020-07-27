// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <kth/rpc/messages/utils.hpp>

namespace kth {

    double bits_to_difficulty(const uint32_t & bits) {
        double diff = 1.0;
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
        return diff;
    }
}

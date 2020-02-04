/**
* Copyright (c) 2016-2020 Knuth Project developers.
*
* This file is part of kth-node.
*
* kth-node is free software: you can redistribute it and/or
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

#include <knuth/rpc/messages/utils.hpp>

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

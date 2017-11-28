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

#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

namespace bitprim {

double bits_to_difficulty (const uint32_t & bits){
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

void getblockheader(size_t i,libbitcoin::message::header::ptr& header, libbitcoin::blockchain::block_chain const& chain){
    boost::latch latch(2);

    chain.fetch_block_header(i, [&](const libbitcoin::code &ec, libbitcoin::message::header::ptr h, size_t height) {
        if (ec == libbitcoin::error::success) {
            header = h;
        }
        latch.count_down();
    });
    latch.count_down_and_wait();
}

libbitcoin::chain::history::list expand(libbitcoin::chain::history_compact::list& compact) {
    libbitcoin::chain::history::list result;
    result.reserve(compact.size());

    // Process and remove all outputs.
    for (auto output = compact.begin(); output != compact.end();)
    {
        if (output->kind == libbitcoin::chain::point_kind::output)
        {
            libbitcoin::chain::history row;

            // Move the output to the result.
            row.output = std::move(output->point);
            row.output_height = output->height;
            row.value = output->value;

            // Initialize the spend to null.
            row.spend = libbitcoin::chain::input_point{ libbitcoin::null_hash, libbitcoin::chain::point::null_index };
            row.temporary_checksum = row.output.checksum();

            // Store the result and erase the output.
            result.emplace_back(std::move(row));
            output = compact.erase(output);
            continue;
        }

        // Skip the spend.
        ++output;
    }

    // TODO: reduce to output set with distinct checksums, as a fault signal.
    ////std::sort(result.begin(), result.end());
    ////result.erase(std::unique(result.begin(), result.end()), result.end());

    // All outputs have been removed, process the spends.
    for (auto& spend: compact)
    {
        auto found = false;

        // Update outputs with the corresponding spends.
        // This relies on the lucky avoidance of checksum hash collisions :<.
        // Ordering is insufficient since the server may write concurrently.
        for (auto& row: result)
        {
            // The temporary_checksum is a union with spend_height, so we must
            // guard against reading temporary_checksum unless spend is null.
            if (row.spend.is_null() &&
                row.temporary_checksum == spend.previous_checksum)
            {
                // Move the spend to the row of its correlated output.
                row.spend = std::move(spend.point);
                row.spend_height = spend.height;

                found = true;
                break;
            }
        }

        // This will only happen if the history height cutoff comes between an
        // output and its spend. In this case we return just the spend.
        // This is not strictly sufficient because of checksum hash collisions,
        // So this miscorrelation must be discarded as a fault signal.
        if (!found)
        {
            libbitcoin::chain::history row;

            // Initialize the output to null.
            row.output = libbitcoin::chain::output_point{ libbitcoin::null_hash, libbitcoin::chain::point::null_index };
            row.output_height = libbitcoin::max_uint64;
            row.value = libbitcoin::max_uint64;

            // Move the spend to the row.
            row.spend = std::move(spend.point);
            row.spend_height = spend.height;
            result.emplace_back(std::move(row));
        }
    }

    compact.clear();
    result.shrink_to_fit();

    // Clear all remaining checksums from unspent rows.
    for (auto& row: result)
        if (row.spend.is_null())
            row.spend_height = libbitcoin::max_uint64;

    // TODO: sort by height and index of output, spend or both in order.
    return result;
}

std::tuple<bool ,size_t, double> get_last_block_difficulty(libbitcoin::blockchain::block_chain const& chain){

    double diff = 1.0;
    size_t top_height;
    libbitcoin::message::header::ptr top = nullptr;
    bool success = false;
    if(chain.get_last_height(top_height)){
        getblockheader(top_height, top, chain);
        if(top != nullptr){
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

}

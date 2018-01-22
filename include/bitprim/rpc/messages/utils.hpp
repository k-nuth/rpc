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

#ifndef BITPRIM_RPC_MESSAGES_UTILS_HPP_
#define BITPRIM_RPC_MESSAGES_UTILS_HPP_

#include <bitcoin/blockchain/interface/block_chain.hpp>

namespace bitprim {

    double bits_to_difficulty (const uint32_t & bits);

    libbitcoin::code getblockheader(size_t i,libbitcoin::message::header::ptr& header, libbitcoin::blockchain::block_chain const& chain);

    libbitcoin::chain::history::list expand(libbitcoin::chain::history_compact::list& compact);

    std::tuple<bool ,size_t, double> get_last_block_difficulty(libbitcoin::blockchain::block_chain const& chain);

}

#endif

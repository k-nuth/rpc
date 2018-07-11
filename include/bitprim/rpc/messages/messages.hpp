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

#ifndef BITPRIM_RPC_MESSAGES_MESSAGES_HPP_
#define BITPRIM_RPC_MESSAGES_MESSAGES_HPP_

#include <bitprim/rpc/messages/blockchain/getaddressbalance.hpp>
#include <bitprim/rpc/messages/blockchain/getrawtransaction.hpp>
#include <bitprim/rpc/messages/blockchain/getspentinfo.hpp>
#include <bitprim/rpc/messages/blockchain/getaddresstxids.hpp>
#include <bitprim/rpc/messages/blockchain/getaddressdeltas.hpp>
#include <bitprim/rpc/messages/blockchain/getaddressutxos.hpp>
#include <bitprim/rpc/messages/blockchain/getblockhashes.hpp>
#include <bitprim/rpc/messages/blockchain/getbestblockhash.hpp>
#include <bitprim/rpc/messages/blockchain/getblock.hpp>
#include <bitprim/rpc/messages/blockchain/getblockhash.hpp>
#include <bitprim/rpc/messages/blockchain/getblockchaininfo.hpp>
#include <bitprim/rpc/messages/blockchain/getblockheader.hpp>
#include <bitprim/rpc/messages/blockchain/getblockcount.hpp>
#include <bitprim/rpc/messages/blockchain/getdifficulty.hpp>
#include <bitprim/rpc/messages/blockchain/getchaintips.hpp>
#include <bitprim/rpc/messages/blockchain/getaddressmempool.hpp>
#include <bitprim/rpc/messages/mining/getblocktemplate.hpp>
#include <bitprim/rpc/messages/mining/submitblock.hpp>
#include <bitprim/rpc/messages/mining/getmininginfo.hpp>
#include <bitprim/rpc/messages/wallet/sendrawtransaction.hpp>
#include <bitprim/rpc/messages/util/getinfo.hpp>
#include <bitprim/rpc/messages/util/validateaddress.hpp>
#include <bitprim/rpc/messages/util/getnetworkinfo.hpp>

#endif

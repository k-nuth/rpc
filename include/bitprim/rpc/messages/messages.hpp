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

#ifndef BITPRIM_RPC_MESSAGES_MESSAGES_HPP_
#define BITPRIM_RPC_MESSAGES_MESSAGES_HPP_

#if defined(BITPRIM_DB_LEGACY) && defined(BITPRIM_DB_SPENDS) && defined(BITPRIM_DB_HISTORY)
#include <bitprim/rpc/messages/blockchain/getaddressbalance.hpp>
#endif

#if defined(BITPRIM_DB_LEGACY) && defined(BITPRIM_DB_SPENDS)
#include <bitprim/rpc/messages/blockchain/getrawtransaction.hpp>
#include <bitprim/rpc/messages/blockchain/getspentinfo.hpp>
#endif

#if defined(BITPRIM_DB_TRANSACTION_UNCONFIRMED)
#include <bitprim/rpc/messages/blockchain/getaddressmempool.hpp>
#include <bitprim/rpc/messages/blockchain/getaddresstxids.hpp>
#endif

#if defined(BITPRIM_DB_LEGACY) && defined(BITPRIM_DB_SPENDS) && defined(BITPRIM_DB_HISTORY)
#include <bitprim/rpc/messages/blockchain/getaddressdeltas.hpp>
#include <bitprim/rpc/messages/blockchain/getaddressutxos.hpp>
#endif

#include <bitprim/rpc/messages/blockchain/getbestblockhash.hpp>

#include <bitprim/rpc/messages/blockchain/getblockchaininfo.hpp>

#if defined(BITPRIM_DB_LEGACY) || defined(BITPRIM_NEW_DB_BLOCKS) 
#include <bitprim/rpc/messages/blockchain/getblockhashes.hpp>
#include <bitprim/rpc/messages/blockchain/getblock.hpp>
#include <bitprim/rpc/messages/blockchain/getblockheader.hpp>


#include <bitprim/rpc/messages/blockchain/getchaintips.hpp>
#include <bitprim/rpc/messages/blockchain/getdifficulty.hpp>
#include <bitprim/rpc/messages/mining/getmininginfo.hpp>
#include <bitprim/rpc/messages/util/getinfo.hpp>
#endif

#include <bitprim/rpc/messages/blockchain/getblockhash.hpp>
#include <bitprim/rpc/messages/blockchain/getblockcount.hpp>

#ifdef BITPRIM_WITH_MINING
#include <bitprim/rpc/messages/mining/getblocktemplate.hpp>
#include <bitprim/rpc/messages/mining/submitblock.hpp>
#endif // BITPRIM_WITH_MINING

#include <bitprim/rpc/messages/wallet/sendrawtransaction.hpp>

#include <bitprim/rpc/messages/util/validateaddress.hpp>
#include <bitprim/rpc/messages/util/getnetworkinfo.hpp>
#include <bitprim/rpc/messages/wallet/createtransaction.hpp>
#include <bitprim/rpc/messages/wallet/createsignature.hpp>
#include <bitprim/rpc/messages/wallet/setsignature.hpp>
#include <bitprim/rpc/messages/wallet/newwallet.hpp>

#ifdef BITPRIM_WITH_KEOKEN
#include <bitprim/rpc/messages/keoken/createasset.hpp>
#include <bitprim/rpc/messages/keoken/sendtoken.hpp>
#include <bitprim/rpc/messages/keoken/initkeoken.hpp>
#include <bitprim/rpc/messages/keoken/getassets.hpp>
#include <bitprim/rpc/messages/keoken/getassetsbyaddress.hpp>
#include <bitprim/rpc/messages/keoken/getallassets.hpp>
#include <bitprim/rpc/messages/keoken/getkeokenaddress.hpp>
#include <bitprim/rpc/messages/keoken/getkeokenblock.hpp>
#endif //BITPRIM_WITH_KEOKEN

#endif //BITPRIM_RPC_MESSAGES_MESSAGES_HPP_

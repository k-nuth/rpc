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

#ifndef KTH_RPC_MESSAGES_ERROR_CODES_HPP_
#define KTH_RPC_MESSAGES_ERROR_CODES_HPP_

namespace kth {
    //! General application defined errors
    //!< std::exception thrown in command handling
    int const RPC_MISC_ERROR = -1;
    //!< Server is in safe mode, and command is not allowed in safe mode
    int const RPC_FORBIDDEN_BY_SAFE_MODE = -2;
    //!< Unexpected type was passed as parameter
    int const RPC_TYPE_ERROR = -3;
    //!< Invalid address or key
    int const RPC_INVALID_ADDRESS_OR_KEY = -5;
    //!< Ran out of memory during operation
    int const RPC_OUT_OF_MEMORY = -7;
    //!< Invalid, missing or duplicate parameter
    int const RPC_INVALID_PARAMETER = -8;
    //!< Database error
    int const RPC_DATABASE_ERROR = -20;
    //!< Error parsing or validating structure in raw format
    int const RPC_DESERIALIZATION_ERROR = -22;
    //!< General error during transaction or block submission
    int const RPC_VERIFY_ERROR = -25;
    //!< Transaction or block was rejected by network rules
    int const RPC_VERIFY_REJECTED = -26;
    //!< Transaction already in chain
    int const RPC_VERIFY_ALREADY_IN_CHAIN = -27;
    //!< Client still warming up
    int const RPC_IN_WARMUP = -28;

    //! Standard JSON-RPC 2.0 errors
    int const RPC_INVALID_REQUEST = -32600;
    int const RPC_METHOD_NOT_FOUND = -32601;
    int const RPC_INVALID_PARAMS = -32602;
    int const RPC_INTERNAL_ERROR = -32603;
    int const RPC_PARSE_ERROR = -32700;

}
#endif

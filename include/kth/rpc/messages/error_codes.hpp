// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

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

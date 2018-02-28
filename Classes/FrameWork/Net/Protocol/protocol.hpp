#ifndef _PROTOCOL_HPP
#define _PROTOCOL_HPP

#include <limits>

#include "bytebuffer.hpp"
#include "crypto.hpp"

namespace proto {

enum Error {
    kInvalidBuffer = -1,
    kInvalidObject = -2,
    kInvalidParameter = -3,
    kInsufficientBuffer = -4,
    kUnknow = -99
};

template<typename Protocol>
    int Encode(Protocol& p, ByteBuffer& buffer, bool crypto = false) {
        size_t body_size = p.Size();

        if (body_size > std::numeric_limits<uint16_t>::max()) {
            return kInsufficientBuffer;
        }

        if (crypto) {
            crypto::Encrypto(
                    const_cast<uint8_t*>(p.Contents()), body_size, body_size);
        }
        buffer << p;
        return buffer.Size();
    }

template <typename Protocol>
    int Decode(Protocol& p, ByteBuffer& buffer, bool crypto = false) {
        uint16_t total_size = buffer.Size();

        if (total_size < sizeof(uint16_t)) {
            return kInvalidParameter;
        }

        buffer >> p;

        if (crypto) {
            uint16_t body_size = total_size - sizeof(uint16_t);
            crypto::Decrypto(
                    const_cast<uint8_t*>(p.Contents()), body_size, body_size);
        }
        return total_size;
    }

}  // namespace proto

#endif  // _GPROTO_GPROTO_HPP

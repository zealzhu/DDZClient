#ifndef _GPROTO_CRYPTO_HPP
#define _GPROTO_CRYPTO_HPP

namespace crypto {

static uint8_t key[53]={
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
    31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
    73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
    233, 239, 241};

inline uint8_t GetKey(uint16_t salt, int idx) {
    return (key[idx % 53] + salt) & 0xff;
}

inline void Encrypto(uint8_t* buf, uint16_t salt, int len) {
    for ( int i = 0; i < len; i++ )
        buf[i] = buf[i] ^ GetKey(salt, i);
}

inline void Decrypto(uint8_t* buf, uint16_t salt, int len) {
    for ( int i = 0; i < len; i++ )
        buf[i] = buf[i] ^ GetKey(salt, i);
}

}  // namespace crypto

#endif  // GPROTO_CRYPTO_HPP_

#pragma once
#include <fc/crypto/sha512.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/uint128.hpp>
#include <fc/fwd.hpp>
#include <vector>

namespace fc {
    class path;

    class aes_encoder
    {
       public:
         aes_encoder();
         ~aes_encoder();
     
         void init( const fc::sha256& key, const fc::uint128& init_value );
         uint32_t encode( const char* plaintxt, uint32_t len, char* ciphertxt );
 //        uint32_t final_encode( char* ciphertxt );

       private:
         struct      impl;
         fc::fwd<impl,96> my;
    };
    class aes_decoder
    {
       public:
         aes_decoder();
         ~aes_decoder();
     
         void     init( const fc::sha256& key, const fc::uint128& init_value );
         uint32_t decode( const char* ciphertxt, uint32_t len, char* plaintext );
//         uint32_t final_decode( char* plaintext );

       private:
         struct      impl;
         fc::fwd<impl,96> my;
    };

    unsigned aes_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                         unsigned char *iv, unsigned char *ciphertext);
    unsigned aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                         unsigned char *iv, unsigned char *plaintext);

} // namespace fc 

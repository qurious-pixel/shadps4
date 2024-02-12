#pragma once

#include <aes.h>
#include <filters.h>
#include <modes.h>
#include <oaep.h>
#include <osrng.h>
#include <rsa.h>
#include <sha.h>

#include "keys.h"
#include "types.h"

using namespace CryptoPP;

class Crypto {

public:
    PkgDerivedKey3Keyset pkg_derived_key3_keyset;
    FakeKeyset FakeKeyset_keyset;
    DebugRifKeyset DebugRifKeyset_keyset;

    RSA::PrivateKey key_pkg_derived_key3_keyset_init();
    RSA::PrivateKey FakeKeyset_keyset_init();
    RSA::PrivateKey DebugRifKeyset_init();

    void RSA2048Decrypt(CryptoPP::byte*& dk3, unsigned char* ciphertext,
                        bool is_dk3); // RSAES_PKCS1v15_
    void ivKeyHASH256(CryptoPP::byte* cipher_input, CryptoPP::byte*& ivkey_result);
    void aesCbcCfb128Decrypt(CryptoPP::byte* ivkey, CryptoPP::byte* ciphertext,
                             CryptoPP::byte*& decrypted);
    void PfsGenCryptoKey(CryptoPP::byte*& data_tweak_key, CryptoPP::byte*& ekpfs,
                         CryptoPP::byte* seed, CryptoPP::byte*& dataKey, CryptoPP::byte*& tweakKey);
    void decryptPFS(CryptoPP::byte* dataKey, CryptoPP::byte* tweakKey, u8* src_image,
                    CryptoPP::byte*& dst_image, int length, u64 sector);

    void xtsXorBlock(CryptoPP::byte* x, const CryptoPP::byte* a, const CryptoPP::byte* b) {
        for (int i = 0; i < 16; i++) {
            x[i] = a[i] ^ b[i];
        }
    }

    void xtsMult(CryptoPP::byte*& encryptedTweak) {
        int feedback = 0;
        for (int k = 0; k < 16; k++) {
            CryptoPP::byte tmp = (encryptedTweak[k] >> 7) & 1;
            encryptedTweak[k] = ((encryptedTweak[k] << 1) + feedback) & 0xFF;
            feedback = tmp;
        }
        if (feedback != 0)
            encryptedTweak[0] ^= 0x87;
    }
};

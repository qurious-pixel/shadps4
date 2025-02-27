#pragma once

#include <aes.h>
#include <filters.h>
#include <modes.h>
#include <oaep.h>
#include <osrng.h>
#include <rsa.h>
#include <sha.h>
#include <span>

#include "common/keys.h"
#include "common/types.h"

using namespace CryptoPP;

class Crypto {
public:
    PkgDerivedKey3Keyset pkg_derived_key3_keyset;
    FakeKeyset FakeKeyset_keyset;
    DebugRifKeyset DebugRifKeyset_keyset;

    RSA::PrivateKey key_pkg_derived_key3_keyset_init();
    RSA::PrivateKey FakeKeyset_keyset_init();
    RSA::PrivateKey DebugRifKeyset_init();

    void RSA2048Decrypt(std::span<CryptoPP::byte, 32> dk3,
                        std::span<const CryptoPP::byte, 256> ciphertext,
                        bool is_dk3); // RSAES_PKCS1v15_
    void ivKeyHASH256(std::span<const CryptoPP::byte, 64> cipher_input, std::span<CryptoPP::byte, 32> ivkey_result);
    void aesCbcCfb128Decrypt(std::span<const CryptoPP::byte, 32> ivkey,
                             std::span<const CryptoPP::byte, 256> ciphertext,
                             std::span<CryptoPP::byte, 256> decrypted);
    void PfsGenCryptoKey(std::span<const CryptoPP::byte, 32> ekpfs,
                         std::span<const CryptoPP::byte, 16> seed,
                         std::span<CryptoPP::byte, 16> dataKey,
                         std::span<CryptoPP::byte, 16> tweakKey);
    void decryptPFS(std::span<const CryptoPP::byte, 16> dataKey,
                    std::span<const CryptoPP::byte, 16> tweakKey,
                    std::span<const u8> src_image,
                    std::span<CryptoPP::byte> dst_image, u64 sector);

    void xtsXorBlock(CryptoPP::byte* x, const CryptoPP::byte* a, const CryptoPP::byte* b) {
        for (int i = 0; i < 16; i++) {
            x[i] = a[i] ^ b[i];
        }
    }

    void xtsMult(std::span<CryptoPP::byte, 16> encryptedTweak) {
        int feedback = 0;
        for (int k = 0; k < encryptedTweak.size(); k++) {
            const auto tmp = (encryptedTweak[k] >> 7) & 1;
            encryptedTweak[k] = ((encryptedTweak[k] << 1) + feedback) & 0xFF;
            feedback = tmp;
        }
        if (feedback != 0) {
            encryptedTweak[0] ^= 0x87;
        }
    }
};

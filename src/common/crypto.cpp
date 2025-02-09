#include <array>
#include "common/crypto.h"

RSA::PrivateKey Crypto::key_pkg_derived_key3_keyset_init() {
    InvertibleRSAFunction params;
    params.SetPrime1(Integer(pkg_derived_key3_keyset.Prime1, 0x80));
    params.SetPrime2(Integer(pkg_derived_key3_keyset.Prime2, 0x80));

    params.SetPublicExponent(Integer(pkg_derived_key3_keyset.PublicExponent, 4));
    params.SetPrivateExponent(Integer(pkg_derived_key3_keyset.PrivateExponent, 0x100));

    params.SetModPrime1PrivateExponent(Integer(pkg_derived_key3_keyset.Exponent1, 0x80));
    params.SetModPrime2PrivateExponent(Integer(pkg_derived_key3_keyset.Exponent2, 0x80));

    params.SetModulus(Integer(pkg_derived_key3_keyset.Modulus, 0x100));
    params.SetMultiplicativeInverseOfPrime2ModPrime1(
        Integer(pkg_derived_key3_keyset.Coefficient, 0x80));

    RSA::PrivateKey privateKey(params);

    return privateKey;
}

RSA::PrivateKey Crypto::FakeKeyset_keyset_init() {
    InvertibleRSAFunction params;
    params.SetPrime1(Integer(FakeKeyset_keyset.Prime1, 0x80));
    params.SetPrime2(Integer(FakeKeyset_keyset.Prime2, 0x80));

    params.SetPublicExponent(Integer(FakeKeyset_keyset.PublicExponent, 4));
    params.SetPrivateExponent(Integer(FakeKeyset_keyset.PrivateExponent, 0x100));

    params.SetModPrime1PrivateExponent(Integer(FakeKeyset_keyset.Exponent1, 0x80));
    params.SetModPrime2PrivateExponent(Integer(FakeKeyset_keyset.Exponent2, 0x80));

    params.SetModulus(Integer(FakeKeyset_keyset.Modulus, 0x100));
    params.SetMultiplicativeInverseOfPrime2ModPrime1(Integer(FakeKeyset_keyset.Coefficient, 0x80));

    RSA::PrivateKey privateKey(params);

    return privateKey;
}

RSA::PrivateKey Crypto::DebugRifKeyset_init() {
    AutoSeededRandomPool rng;
    InvertibleRSAFunction params;
    params.SetPrime1(Integer(DebugRifKeyset_keyset.Prime1, sizeof(DebugRifKeyset_keyset.Prime1)));
    params.SetPrime2(Integer(DebugRifKeyset_keyset.Prime2, sizeof(DebugRifKeyset_keyset.Prime2)));

    params.SetPublicExponent(Integer(DebugRifKeyset_keyset.PrivateExponent,
                                     sizeof(DebugRifKeyset_keyset.PrivateExponent)));
    params.SetPrivateExponent(Integer(DebugRifKeyset_keyset.PrivateExponent,
                                      sizeof(DebugRifKeyset_keyset.PrivateExponent)));

    params.SetModPrime1PrivateExponent(
        Integer(DebugRifKeyset_keyset.Exponent1, sizeof(DebugRifKeyset_keyset.Exponent1)));
    params.SetModPrime2PrivateExponent(
        Integer(DebugRifKeyset_keyset.Exponent2, sizeof(DebugRifKeyset_keyset.Exponent2)));

    params.SetModulus(
        Integer(DebugRifKeyset_keyset.Modulus, sizeof(DebugRifKeyset_keyset.Modulus)));
    params.SetMultiplicativeInverseOfPrime2ModPrime1(
        Integer(DebugRifKeyset_keyset.Coefficient, sizeof(DebugRifKeyset_keyset.Coefficient)));

    RSA::PrivateKey privateKey(params);

    return privateKey;
}

void Crypto::RSA2048Decrypt(std::span<CryptoPP::byte, 32> dec_key,
                            std::span<const CryptoPP::byte, 256> ciphertext,
                            bool is_dk3) { // RSAES_PKCS1v15_
    // Create an RSA decryptor
    RSA::PrivateKey privateKey;
    if (is_dk3) {
        privateKey = key_pkg_derived_key3_keyset_init();
    } else {
        privateKey = FakeKeyset_keyset_init();
    }

    RSAES_PKCS1v15_Decryptor rsaDecryptor(privateKey);

    // Allocate memory for the decrypted data
    std::array<CryptoPP::byte, 256> decrypted;

    // Perform the decryption
    AutoSeededRandomPool rng;
    DecodingResult result = rsaDecryptor.Decrypt(rng, ciphertext.data(), decrypted.size(), decrypted.data());
    std::copy(decrypted.begin(), decrypted.begin() + dec_key.size(), dec_key.begin());
}

void Crypto::ivKeyHASH256(std::span<const CryptoPP::byte, 64> cipher_input,
                          std::span<CryptoPP::byte, 32> ivkey_result) {
    CryptoPP::SHA256 sha256;
    std::array<CryptoPP::byte, CryptoPP::SHA256::DIGESTSIZE> hashResult;
    auto array_sink = new CryptoPP::ArraySink(hashResult.data(), CryptoPP::SHA256::DIGESTSIZE);
    auto filter = new CryptoPP::HashFilter(sha256, array_sink);
    CryptoPP::ArraySource r(cipher_input.data(), cipher_input.size(), true, filter);
    std::copy(hashResult.begin(), hashResult.begin() + ivkey_result.size(), ivkey_result.begin());
}

void Crypto::aesCbcCfb128Decrypt(std::span<const CryptoPP::byte, 32> ivkey,
                                 std::span<const CryptoPP::byte, 256> ciphertext,
                                 std::span<CryptoPP::byte, 256> decrypted) {
    std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> key;
    std::array<CryptoPP::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> iv;

    std::copy(ivkey.begin() + 16, ivkey.begin() + 16 + key.size(), key.begin());
    std::copy(ivkey.begin(), ivkey.begin() + iv.size(), iv.begin());

    CryptoPP::AES::Decryption aesDecryption(key.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv.data());

    for (size_t i = 0; i < decrypted.size(); i += CryptoPP::AES::BLOCKSIZE) {
        cbcDecryption.ProcessData(decrypted.data() + i, ciphertext.data() + i, CryptoPP::AES::BLOCKSIZE);
    }
}

void Crypto::PfsGenCryptoKey(std::span<const CryptoPP::byte, 32> ekpfs,
                             std::span<const CryptoPP::byte, 16> seed,
                             std::span<CryptoPP::byte, 16> dataKey,
                             std::span<CryptoPP::byte, 16> tweakKey) {
    CryptoPP::HMAC<CryptoPP::SHA256> hmac(ekpfs.data(), ekpfs.size());

    CryptoPP::SecByteBlock d(20); // Use Crypto++ SecByteBlock for better memory management

    // Copy the bytes of 'index' to the 'd' array
    uint32_t index = 1;
    std::memcpy(d, &index, sizeof(uint32_t));

    // Copy the bytes of 'seed' to the 'd' array starting from index 4
    std::memcpy(d + sizeof(uint32_t), seed.data(), seed.size());

    // Allocate memory for 'u64' using new
    std::vector<CryptoPP::byte> data_tweak_key(hmac.DigestSize());

    // Calculate the HMAC
    hmac.CalculateDigest(data_tweak_key.data(), d, d.size());
    std::copy(data_tweak_key.begin(), data_tweak_key.begin() + dataKey.size(), tweakKey.begin());
    std::copy(data_tweak_key.begin() + tweakKey.size(),
              data_tweak_key.begin() + tweakKey.size() + dataKey.size(), dataKey.begin());
}

void Crypto::decryptPFS(std::span<const CryptoPP::byte, 16> dataKey,
                        std::span<const CryptoPP::byte, 16> tweakKey,
                        std::span<const u8> src_image,
                        std::span<CryptoPP::byte> dst_image, u64 sector) {
    // Start at 0x10000 to keep the header when decrypting the whole pfs_image.
    for (int i = 0; i < src_image.size(); i += 0x1000) {
        const u64 current_sector = sector + (i / 0x1000);
        CryptoPP::ECB_Mode<AES>::Encryption encrypt(tweakKey.data(), tweakKey.size());
        CryptoPP::ECB_Mode<AES>::Decryption decrypt(dataKey.data(), dataKey.size());

        std::array<CryptoPP::byte, 16> tweak{};
        std::array<CryptoPP::byte, 16> encryptedTweak;
        std::array<CryptoPP::byte, 16> xorBuffer;
        std::memcpy(tweak.data(), &current_sector, sizeof(u64));

        // Encrypt the tweak for each sector.
        encrypt.ProcessData(encryptedTweak.data(), tweak.data(), 16);

        for (int plaintextOffset = 0; plaintextOffset < 0x1000; plaintextOffset += 16) {
            xtsXorBlock(xorBuffer.data(), src_image.data() + i + plaintextOffset, encryptedTweak.data()); // x, c, t
            decrypt.ProcessData(xorBuffer.data(), xorBuffer.data(), 16);                           // x, x
            xtsXorBlock(dst_image.data() + i + plaintextOffset, xorBuffer.data(), encryptedTweak.data()); //(p)  c, x , t
            xtsMult(encryptedTweak);
        }
    }
}

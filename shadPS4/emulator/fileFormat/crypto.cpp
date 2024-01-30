#include "crypto.h"


using namespace CryptoPP;

RSA::PrivateKey Crypto::key_pkg_derived_key3_keyset_init()
{
    InvertibleRSAFunction params;
    params.SetPrime1(Integer(pkg_derived_key3_keyset.Prime1, 0x80));
    params.SetPrime2(Integer(pkg_derived_key3_keyset.Prime2, 0x80));

    params.SetPublicExponent(Integer(pkg_derived_key3_keyset.PublicExponent, 4));
    params.SetPrivateExponent(Integer(pkg_derived_key3_keyset.PrivateExponent, 0x100));

    params.SetModPrime1PrivateExponent(Integer(pkg_derived_key3_keyset.Exponent1, 0x80));
    params.SetModPrime2PrivateExponent(Integer(pkg_derived_key3_keyset.Exponent2, 0x80));

    params.SetModulus(Integer(pkg_derived_key3_keyset.Modulus, 0x100));
    params.SetMultiplicativeInverseOfPrime2ModPrime1(Integer(pkg_derived_key3_keyset.Coefficient, 0x80));

    RSA::PrivateKey privateKey(params);

    return privateKey;
}


RSA::PrivateKey Crypto::FakeKeyset_keyset_init()
{
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

RSA::PrivateKey Crypto::DebugRifKeyset_init()
{
    AutoSeededRandomPool rng;
    InvertibleRSAFunction params;
    params.SetPrime1(Integer(DebugRifKeyset_keyset.Prime1, sizeof(DebugRifKeyset_keyset.Prime1)));
    params.SetPrime2(Integer(DebugRifKeyset_keyset.Prime2, sizeof(DebugRifKeyset_keyset.Prime2)));

    params.SetPublicExponent(Integer(DebugRifKeyset_keyset.PrivateExponent, sizeof(DebugRifKeyset_keyset.PrivateExponent)));
    params.SetPrivateExponent(Integer(DebugRifKeyset_keyset.PrivateExponent, sizeof(DebugRifKeyset_keyset.PrivateExponent)));

    params.SetModPrime1PrivateExponent(Integer(DebugRifKeyset_keyset.Exponent1, sizeof(DebugRifKeyset_keyset.Exponent1)));
    params.SetModPrime2PrivateExponent(Integer(DebugRifKeyset_keyset.Exponent2, sizeof(DebugRifKeyset_keyset.Exponent2)));

    params.SetModulus(Integer(DebugRifKeyset_keyset.Modulus, sizeof(DebugRifKeyset_keyset.Modulus)));
    params.SetMultiplicativeInverseOfPrime2ModPrime1(Integer(DebugRifKeyset_keyset.Coefficient, sizeof(DebugRifKeyset_keyset.Coefficient)));

    RSA::PrivateKey privateKey(params);

    return privateKey;
}

void Crypto::RSA2048Decrypt(CryptoPP::byte*& dec_key, unsigned char* ciphertext, bool is_dk3) // RSAES_PKCS1v15_
{
    // Create an RSA decryptor
    RSA::PrivateKey privateKey;
    if (is_dk3)
        privateKey = key_pkg_derived_key3_keyset_init();
    else
        privateKey = FakeKeyset_keyset_init();
    size_t ciphertextSize = 256;

    RSAES_PKCS1v15_Decryptor rsaDecryptor(privateKey);

    // Allocate memory for the decrypted data
    CryptoPP::byte* decrypted = new  CryptoPP::byte[ciphertextSize];
    size_t decrypted_key_size = 32;

    // Perform the decryption
    AutoSeededRandomPool rng;

    DecodingResult result = rsaDecryptor.Decrypt(
        rng,
        ciphertext, ciphertextSize,
        decrypted
    );

    dec_key = new  CryptoPP::byte[decrypted_key_size];
    std::copy(decrypted, decrypted + decrypted_key_size, dec_key);
    delete[] decrypted;
}

void Crypto::ivKeyHASH256(CryptoPP::byte* cipher_input, CryptoPP::byte*& ivkey_result)
{
    CryptoPP::SHA256 sha256;
    CryptoPP::byte* hashResult = new  CryptoPP::byte[CryptoPP::SHA256::DIGESTSIZE];
    size_t decrypted_key_size = 32;

    CryptoPP::ArraySource r(cipher_input, 64, true,
        new CryptoPP::HashFilter(sha256, new CryptoPP::ArraySink(hashResult, CryptoPP::SHA256::DIGESTSIZE))
    );

    ivkey_result = new  CryptoPP::byte[decrypted_key_size];
    std::copy(hashResult, hashResult + decrypted_key_size, ivkey_result);
    delete[] hashResult;
}

void Crypto::aesCbcCfb128Decrypt(CryptoPP::byte* ivkey, CryptoPP::byte* ciphertext, CryptoPP::byte*& decrypted)
{
    size_t subarraySize = 16;
    size_t cipher_size = 256;
    CryptoPP::byte* key = new CryptoPP::byte[subarraySize];
    CryptoPP::byte* iv = new CryptoPP::byte[subarraySize];

    std::copy(ivkey + 16, ivkey + 16 + subarraySize, key);
    std::copy(ivkey, ivkey + subarraySize, iv);


    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

    decrypted = new CryptoPP::byte[cipher_size];

    for (size_t i = 0; i < cipher_size; i += CryptoPP::AES::BLOCKSIZE) {
        cbcDecryption.ProcessData(decrypted + i, ciphertext + i, CryptoPP::AES::BLOCKSIZE);
    }
    delete[] key;
    delete[] iv;
}

void Crypto::PfsGenCryptoKey(CryptoPP::byte* &data_tweak_key, CryptoPP::byte* &ekpfs, CryptoPP::byte* seed,
    CryptoPP::byte* &dataKey, CryptoPP::byte* &tweakKey)
{
    size_t ekpfsSize = 32;
    size_t seedSize = 16;
    uint32_t index = 1;

    CryptoPP::HMAC<CryptoPP::SHA256> hmac(ekpfs, ekpfsSize);

    CryptoPP::SecByteBlock d(20); // Use Crypto++ SecByteBlock for better memory management
    
    // Copy the bytes of 'index' to the 'd' array
    std::memcpy(d, &index, sizeof(uint32_t));

    // Copy the bytes of 'seed' to the 'd' array starting from index 4
    std::memcpy(d + sizeof(uint32_t), seed, seedSize);

    // Allocate memory for 'U64' using new
    data_tweak_key = new CryptoPP::byte[hmac.DigestSize()];
    dataKey = new CryptoPP::byte[16];
    tweakKey = new CryptoPP::byte[16];

    // Calculate the HMAC
    hmac.CalculateDigest(data_tweak_key, d, d.size());

    //
    std::copy(data_tweak_key, data_tweak_key + 16, tweakKey);
    std::copy(data_tweak_key + 16, data_tweak_key + 32, dataKey);
}

void Crypto::decryptPFS(CryptoPP::byte* dataKey, CryptoPP::byte* tweakKey,
    U08* src_image, CryptoPP::byte*& dst_image, int length)
{
    for (int i = 0x10000; i < length; i += 0x1000)
    {
        U64 currentSector = i / 0x1000;
        SecByteBlock keydata(AES::DEFAULT_KEYLENGTH); keydata.Assign(dataKey, 16);
        SecByteBlock keytweak(AES::DEFAULT_KEYLENGTH); keytweak.Assign(tweakKey, 16);
        CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption encc; encc.SetKey(keytweak, 16);
        CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption decc; decc.SetKey(keydata, 16);


        CryptoPP::byte* tweak = new CryptoPP::byte[16];
        CryptoPP::byte* encryptedTweak = new CryptoPP::byte[16];
        CryptoPP::byte* xorBuffer = new CryptoPP::byte[16];
        CryptoPP::byte* zeroes = new CryptoPP::byte[8];
        unsigned char sectorBytes[8];

        for (int j = 0; j < 8; j++) {
            zeroes[j] = 0;
            // Shift the bits to the right to isolate each byte
            sectorBytes[j] = static_cast<unsigned char>((currentSector >> (j * 8)) & 0xFF);
        }

        memcpy(tweak, sectorBytes, 8);
        memcpy(tweak + 8, zeroes, 8);

        // Encrypt the tweak for each sector.
        encc.ProcessData(encryptedTweak, tweak, 16);

        for (int plaintextOffset = 0; plaintextOffset < 0x1000; plaintextOffset += 16)
        {

            xtsXorBlock(xorBuffer, src_image + i + plaintextOffset, encryptedTweak); //x, c, t
            decc.ProcessData(xorBuffer, xorBuffer, 16); // x, x
            xtsXorBlock(dst_image + i + plaintextOffset, xorBuffer, encryptedTweak);//(p)  c, x , t

            xtsMult(encryptedTweak);
        }
    }
}
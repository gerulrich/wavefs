#include "cypher.h"
#include <fstream>
#include <openssl/aes.h>
#include <openssl/evp.h>

pair<vector<unsigned char>, vector<unsigned char>> cypher::decrypt_key(const string& cipherKey) {
    string securityToken = base64_decode(cipherKey);
    vector<unsigned char> iv(securityToken.begin(), securityToken.begin() + 16);
    vector<unsigned char> encryptedSt(securityToken.begin() + 16, securityToken.end());

    // Inicializar el descifrador AES
    AES_KEY decryptionKey;
    AES_set_decrypt_key(reinterpret_cast<const unsigned char*>(masterKey.c_str()), 256, &decryptionKey);

    // Descifrar el token encriptado
    vector<unsigned char> decryptedSt(encryptedSt.size());
    AES_cbc_encrypt(encryptedSt.data(), decryptedSt.data(), encryptedSt.size(), &decryptionKey, iv.data(), AES_DECRYPT);

    // Obtener la clave de descifrado y el nonce del token descifrado
    vector<unsigned char> key(decryptedSt.begin(), decryptedSt.begin() + 16);
    vector<unsigned char> nonce(decryptedSt.begin() + 16, decryptedSt.begin() + 24);
    return std::make_pair(key, nonce);
}

void cypher::decrypt_file(pair<vector<unsigned char>, vector<unsigned char>> keyPair, VirtualFile& cypherFile) {
     // Read file to
    ifstream file(cypherFile.filename, ios::binary);
    vector<unsigned char> cypherData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::vector<unsigned char> data(cypherData.size());

    // TODO dynamic name
    std::string outputFilename = cypherFile.filename + ".decrypted";
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open())
    {
        //std::cerr << "Error al abrir el archivo de salida." << std::endl;
        //return -1;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        // std::cerr << "Error al inicializar el contexto de cifrado." << std::endl;
        // return -1;
    }


    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), nullptr, keyPair.first.data(), keyPair.second.data()) != 1)
    {
        //std::cerr << "Error al inicializar la operación de cifrado." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        //return -1;
    }

    // Cifrar los datos
    int len;
    int ciphertextLen = 0;
    vector<unsigned char> ciphertext(cypherData.size() + EVP_MAX_BLOCK_LENGTH);

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, cypherData.data(), cypherData.size()) != 1)
    {
        // std::cerr << "Error al realizar la operación de cifrado." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        // return -1;
    }
    ciphertextLen += len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
    {
        // std::cerr << "Error al finalizar la operación de cifrado." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        // return -1;
    }
    ciphertextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    // Escribir los datos cifrados en el archivo de salida
    outputFile.write(reinterpret_cast<char*>(ciphertext.data()), ciphertextLen);
    outputFile.close();

    if (remove(cypherFile.filename.c_str()) != 0)
    {
        perror("Error al eliminar el archivo temporal");
    }
    cypherFile.filename = outputFilename;


}


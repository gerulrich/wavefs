#ifndef WAVEFS_CYPHER_H
#define WAVEFS_CYPHER_H
#include "../encode/base64.h"
#include "../wavefs/wavefs_types.h"
#include <cstring>
#include <vector>

using namespace std;

class cypher {
public:
    explicit cypher(const string &masterKey) : masterKey(base64_decode(masterKey, false)) {};
    ~cypher() = default;
    std::pair<vector<unsigned char>, vector<unsigned char>> decrypt_key(const string& key);
    void decrypt_file(pair<vector<unsigned char>, vector<unsigned char>> keyPair, VirtualFile& cypherFile);
private:
    string masterKey;

};


#endif //WAVEFS_CYPHER_H

#include <bits/stdc++.h>
#include <fstream>
using namespace std;

ifstream F("BFIce.xm", ios::binary);

// di
// no i said that you would do it
vector<uint8_t> read_bytes(int num_bytes) {
    vector<uint8_t> buffer(num_bytes);
    F.read(reinterpret_cast<char*>(buffer.data()), num_bytes);
    return buffer;
}


int main() {
    auto wuggyvector = read_bytes(100);
    for (auto a : wuggyvector)
        cout << a;
    cout << std::endl;
    cin.get();
    return 0;
}
#include <bits/stdc++.h>
#include <fstream>
using namespace std;

ifstream F("BFIce.xm", ios::binary);

/*
typing typingers typi
why was blud observing me
wow it did Nothing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
did you make the webmook <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
did you make the webmook 
yeah did you push anything
not yet
check deezstaff
*/

char read_byte() {
    char byte;
    F >> byte;
    return byte;
}

vector<char> read_bytes(int b) {
    vector<char> bytes;
    for (int i = 0; i < b; i++) {
        bytes.push_back(read_byte());
    }
    return bytes;
}

int main() {
  /*
    vector<char> bytes = read_bytes(100);
    for (int i = 0; i < 100; i++) {
        cout << bytes[i] << ' ';
    }
    cout << endl;
  */
    cout << read_byte();
    return 0;
}
#include <bits/stdc++.h>
#include "xm_reader.hpp"
#include "xm_converter.hpp"
using namespace std;

void writeToFile(string content) {
    ofstream myfile;
    myfile.open ("output.js");
    myfile << content;
    myfile.close();
}

int main() { // for some reason you have to update main.cpp to make everything RECOMPIELELELE recompiling moment
    XModule mod;
    setFstream("xm/testing.xm"); // it weighs 3 kb so lets test on it are you serious make clean is a bushes
    mod.read_data();
    //cout << readFile("utilities.js") << endl;
    string converted = convertXmToFuncbeat(mod);
    writeToFile(converted);
    return 0;
}
/*
// update this everytime you did not update this file and want to run it
// ########ASDFMOVIE15ISHEREdonotdeletethis#################
UPDATE THE MAFFAKING FILE!!!!!!?!o
reE error happens exactly when it reads patterns COMPRESS MY ASSЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬ� fuc
seggmentation ault uh ohogoggogoggogoggii
fuck
*/
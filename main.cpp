#include "xm_converter.hpp" // xm reader is included there sooooo
#include <bits/stdc++.h>
using namespace std;

void writeToFile(string content, string filename) {
    ofstream myfile;
    myfile.open(filename);
    myfile << content;
    myfile.close();
}
bool hasParam(string look, int argc, char *argv[]){
    for (int i = 0; i < argc; i++){
        if (argv[i] == look){
            return true;
        }
    };
    return false;
};
bool hasInputFile(int argc, char *argv[]){
    for(int i=1;i<argc;i++){
        if(argv[i][0]!='-'){
            return true;
        }
    };
    return false;
};
string getInputFile(int argc, char *argv[]){
    for(int i=1;i<argc;i++){
        if(argv[i][0]!='-'){
            return argv[i];
        }
    };
    return "";
};
bool hasOutputFile(int argc, char *argv[]){
    int passed=0;
    for(int i=1;i<argc;i++){
        if(argv[i][0]!='-'){
            passed++;
            if(passed==2){
                return true;
            };
        }
    };
    return false;
};
string getOutputFile(int argc, char *argv[]){
    int passed=0;
    for(int i=1;i<argc;i++){
        if(argv[i][0]!='-'){
            passed++;
            if(passed==2){
                return argv[i];
            };
        }
    };
    return "";
};
int main(int argc, char *argv[]) {
    bool verbose=false;
    cout << "XM to funcbeat convertor V0. By tema5002, dtpls_ and ponali\n";
    string fp, output = "output.js";
    if (hasParam("-v", argc, argv) || hasParam("--verbose", argc, argv)) {
        verbose=true;
    }
    if(hasInputFile(argc,argv)){
        fp = getInputFile(argc,argv);
        if(hasOutputFile(argc,argv)){
            output = getOutputFile(argc,argv);
        };
    } else {
        cout << "No file name provided.\n";
        cout << "Usage: ./xm_to_funcbeat <xm_file> (-v/--verbose) <output_file> (optional)\n";
        cout << "-v, --verbose: verbose mode\n";
        return 0;
    }
    cout << "Reading file \"" << fp << "\"...\n";
    XModule mod; // now you need to put filepath in .replit file
    setFstream(fp); // it weighs 3 kb so lets test on it are you serious make
    mod.read_data(verbose);
    cout << "Converting...\n";
    string converted = convertXmToFuncbeat(mod,verbose);
    cout << "Writing to file \"" << output << "\"...\n";
    writeToFile(converted, output);
    return 0;
}
/* my balls x5
// update this everytime you did not update this file and want to run it
// ########ASDFMOVIE15ISHEREdonotdeletethis##################
UPDATE THE MAFFAKING FILE!!!!!!?!o
reE error happens exactly when it reads patterns COMPRESS MY
ASSЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬЬ� fuc seggmentation ault uh ohogoggogoggogoggii furqtyrqn
*/
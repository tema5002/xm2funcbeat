#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
typedef uint8_t byte;
typedef int8_t sbyte;
using namespace std;

string convertIntToHex(int in){
    ostringstream ss;
    ss << hex << in;
    return ss.str();
}

string wstring_to_utf8 (const wchar_t& str){
    wstring_convert<codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}

string convertSampleToPCM(Sample sample){
    cout << "converting sample \"" << sample.name << "\" to pcm\n";
    short* data = sample.data;
    string out = "\""; // then what
    for (int i = 0; i < sample.length; i++){ // do you want to use strings here or what are you doing
        int temp = data[i]+32768;
        if(temp<21){
            string hex = convertIntToHex(temp);
            while(hex.size()<2){
                hex = "0"+hex;
            }
            out += "\\x"+hex;
        } else {
            wchar_t temp2 = wchar_t(temp);
            if (temp2==L'\"' || temp2==L'\\'){ // nvm i didnt notice the second // why would it be 8 numbers if there are 2 channels
                out += '\\';
            };
            out += wstring_to_utf8(temp2);
        }
    };
    out += '"';
    return out;
}

string strip(const string &str) {
    string temp = "";
    for (int i = 0; i < str.length(); i++) {
        char x = str[i];
        if (x == '"' || x == '\\') temp += "\\";
        if (0x20 < x && x < 0x7f) temp += x;
    };
    return temp;
}

string writeInfo(string name, string data) {
    return "\"" + name + "\":" + data;
}

string writeStringInfo(string name, string data) {
    return writeInfo(name, "\"" + strip(data) + "\"");
}

string writeInt(string name, int data) {
    return writeInfo(name, to_string(data));
}

string convertInstrumentToJSON(Instrument inst) {
    cout << "converting instrument \"" << inst.name << "\" to sample pcms\n";
    string out = "{";
    out += writeStringInfo("name", strip(inst.name));
    out += ",";
    out += "\"samples\":[";
    for (int i = 0; i < inst.number_of_samples; i++) {
        out += "{";
        Sample sample = inst.samples[i];
        out += writeStringInfo("name", sample.name) + ",";
        string pcm = convertSampleToPCM(sample);
        out += writeInfo("data", pcm) + ",";
        // other sample info
        out += writeInt("length",        sample.length) + ",";
        out += writeInt("loop_start",    sample.loop_start) + ",";
        out += writeInt("loop_length",   sample.loop_length) + ",";
        out += writeInt("volume",        sample.volume) + ",";
        out += writeInt("finetune",      sample.finetune) + ",";
        out += writeInt("type",          sample.type) + ",";
        out += writeInt("panning",       sample.panning) + ",";
        out += writeInt("relative_note", sample.relative_note) + ",";
        out += writeInt("reserved",      sample.reserved);
        out += "}";
        if (i != inst.number_of_samples - 1) out += ",";
    }
    out += "]";

    // other instruments info
    out += ",\"volume_envelope_points\":[";
    for (int i = 0; i < 12; i++) {
        auto a = inst.volume_envelope_points[i];
        out += "[" + to_string(a.first) + "," + to_string(a.second) + "]";
        if (i != 11) out += ",";
    }
    out += "],";

    out += "\"panning_envelope_points\":[";
    for (int i = 0; i < 12; i++) {
        auto a = inst.panning_envelope_points[i];
        out += "[" + to_string(a.first) + "," + to_string(a.second) + "]";
        if (i != 11) out += ",";
    }
    out += "],";

    out += writeInt("volume_envelope_points_amount", inst.volume_envelope_points_amount)+",";
    out += writeInt("panning_envelope_points_amount",inst.panning_envelope_points_amount)+",";
    out += writeInt("volume_sustain_point",          inst.volume_sustain_point)+",";
    out += writeInt("volume_loop_start_point",       inst.volume_loop_start_point)+",";
    out += writeInt("volume_loop_end_point",         inst.volume_loop_end_point)+",";
    out += writeInt("panning_sutain_point",          inst.panning_sutain_point)+",";
    out += writeInt("panning_loop_start_point",      inst.panning_loop_start_point)+",";
    out += writeInt("panning_loop_end_point",        inst.panning_loop_end_point)+",";
    out += writeInt("volume_type",                   inst.volume_type)+",";
    out += writeInt("panning_type",                  inst.panning_type)+",";
    out += writeInt("vibtype",                       inst.vibtype)+",";
    out += writeInt("vibsweep",                      inst.vibsweep)+",";
    out += writeInt("vibdepth",                      inst.vibdepth)+",";
    out += writeInt("vibrate",                       inst.vibrate)+",";
    out += writeInt("volume_fadeout",                inst.volume_fadeout)+",";
    out += writeInt("reserved",                      inst.reserved);
    out += '}';
    return out;
}

string convertInstrumentListToJSON(vector<Instrument> insts, int amount){
    cout << "converting instruments to json\n";
    string out = "[";
    for (int i = 0; i < amount; i++) {
        out += convertInstrumentToJSON(insts[i]);
        if (i != amount - 1) out += ",\n";
    }
    out += "]";
    return out;
}

string compressPattern(byte data[5]) {
    long int out = 0;
    for (int a = 0; a < 5; a++) {
        out += data[a] * pow(2, a * 8);
    }
    return to_string(out);
}

string convertPatternToJSON(Pattern pattern) {
    cout << "converting some pattern with " << pattern.number_of_rows << " rows and " << pattern.channels << " channels" << endl;
    string out = "[";
    for (int i = 0; i < pattern.number_of_rows; i++) {
        out += "[";
        for (int j = 0; j < pattern.channels; j++) { // wdym bro yes it is that where is the last "done compressing shit"
            cout << "converting pattern row " << i << " channel " << j;
            out += compressPattern(pattern.data[i][j]); // nvm WHY DOESNT IT WORK? COck
            if (j != pattern.channels - 1) out += ","; // lets see if it prints converted pattern to jay son
            cout << " done" << endl; // it always prints done idk
        }
        out += "]";
        if (i != pattern.number_of_rows - 1) out += ",";
        cout << "done converting the row " << i << ". out string is now in length " << out.length() << endl;
    } // error is here 💀💀💀💀💀
    cout << "done converting the goddamn pattern" << endl;
    out += "]";
    cout << "converted pattern to json" << endl;
    return out;
}

string convertPatternListToJSON(vector<Pattern> patterns, int patterns_amount) {
    cout << "converting patterns to json" << endl;
    string out = "[";
    for (int i = 0; i < patterns_amount; i++) {
        out += convertPatternToJSON(patterns[i]);
        if (i != patterns_amount - 1) out += ",";
    }
    out += "]";
    return out; 
}

string convertXmToFuncbeat(XModule m) { // you cho puppies for morning 😎😎😎
    string output = "// - epic xm2funcbeat logo goes here - \n";
    output += "// created by tema5002, dtpls_ and ponali\n";
    output += "let name = \"" + m.name + "\";";
    output += "\nlet song_length = " + to_string(m.song_length)+";";
    output += "\nlet song_restart_position = " + to_string(m.song_restart_position)+";";
    output += "\nlet channels = " + to_string(m.channels)+";";
    output += "\nlet patterns_amount = " + to_string(m.patterns_amount)+";";
    output += "\nlet instruments_amount = " + to_string(m.instruments_amount)+";";
    output += "\nlet flags = " + to_string(m.flags)+";";
    output += "\nlet tempo = " + to_string(m.tempo)+";";
    output += "\nlet bpm = " + to_string(m.bpm)+";";
    output += "\nlet pattern_order_table = [";
    for (int i = 0; i < m.song_length; i++) {
        output += to_string(m.pattern_order_table[i]);
        if (i != m.song_length-1) output += ",";
    }
    output += "];";
    output += "\nlet instruments = ";
    output += convertInstrumentListToJSON(m.instruments, m.instruments_amount) + ";";
    output += "\nlet patterns = ";
    output += convertPatternListToJSON(m.patterns, m.patterns_amount) + ";";

    ifstream Fin("utilities.js");
    while (Fin) {
        string s;
        getline(Fin, s);
        output += "\n" + s;
    }
    Fin.close();

    return output;
}

/*
for dtpls
copy this somewhere and remove idk

note: vectors can be used same as arrays
XModule (mod)
.pattern_order_table
    array of ints like [0, 3, 46, 24]
    order of the song

.patterns
    vector of Patterns
    to play the order above you would need to get Patterns like that (please use for loop instead):
    Pattern pattern1 = mod.patterns[0];
    Pattern pattern2 = mod.patterns[3];
    Pattern pattern3 = mod.patterns[46];
    Pattern pattern4 = mod.patterns[24];

.instruments
    vector of Instruments
    works the same as mod.patterns

Pattern
.number_of_rows (int):
    length of pattern
.data (char***)
    3d matrix of chars
    use it like mod.patterns[pattern index].data[row][channel][data]
    for example to get note of third channel in second row in first pattern do:
    int note = mod.patterns[0].data[1][2][0];
    0 - note
    1 - instrument
    2 - volume
    3 - fx
    4 - fx data

Instrument
    all the variables are very self explainating
    idk what sample_number_for_all_notes is
    use volume/panning_envelope_points[index].real()/.imag() to get X and Y btw
.samples
    vector of Samples

Sample
    all variables are also self explaining
*/
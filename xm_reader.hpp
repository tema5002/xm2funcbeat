#include <iostream>
#include <vector>
#include <fstream>
#include "adpcm.hpp"
typedef uint8_t byte;
typedef int8_t sbyte;
using namespace std;

ifstream F;

bool hasFlag(int byte, int flag) {
    return byte & (1 << flag);
}

byte readByte() {
    if (!F) {
        cout << "Error: reached end of file\n";
        cout << "Either your file is invalid or we messed up\n";
        cout << "If you're sure your file is valid post issue on github\n";
        cout << "or something idk\n";
        exit(1);
    }
    byte c = F.get();
    return c;
}

int readInt(int b) {
    int s = 0;
    for (int i = 0; i < b; i++) {
        s += readByte() * (int)pow(256, i);
    }
    return s;
}

sbyte readSignedByte() {
    return (sbyte)readByte();
}

short readSignedShort() {
    short s;
    s = readInt(1);
    s <<= 8;
    s |= readInt(1);
    return s;
}

vector<byte> readBytes(int b) {
    vector<byte> bytes;
    for (int i = 0; i < b; i++) {
        bytes.push_back(readByte());
    }
    return bytes;
}

string readString(int b) {
    string s;
    for (int i = 0; i < b; i++) {
        s.push_back((char)readByte());
    }
    return s;
}

void skipBytes(int b) {
    for (int i = 0; i < b; i++)
        readByte();
}

class Pattern {
    public:
    void read_data(int channels_amount, bool verbose) {
        skipBytes(4); // Pattern header length
        skipBytes(1); // Packing type
        number_of_rows = readInt(2);
        channels = channels_amount;
        if(verbose) cout << "Reading some pattern which length is " << number_of_rows << "\n";
        skipBytes(2); // Packed size (somehow unused)

        data = new byte**[number_of_rows];
        for (int i = 0; i < number_of_rows; i++) {
            data[i] = new byte*[channels_amount];
            for (int j = 0; j < channels_amount; j++) {
                byte* cell = new byte[5];
                byte byte = readByte();
                if (hasFlag(byte, 7)) {
                    if (hasFlag(byte, 0)) cell[0] = readByte();
                    if (hasFlag(byte, 1)) cell[1] = readByte();
                    if (hasFlag(byte, 2)) cell[2] = readByte();
                    if (hasFlag(byte, 3)) cell[3] = readByte();
                    if (hasFlag(byte, 4)) cell[4] = readByte();
                }
                else {
                    cell[0] = byte; // note
                    cell[1] = readByte(); // instrument
                    cell[2] = readByte(); // volume
                    cell[3] = readByte(); // effect
                    cell[4] = readByte(); // effect parameter
                }
                data[i][j] = cell;
            }
        }
    }
    int channels = 2; // by default
    int number_of_rows = 0;
    byte*** data;
};

class Sample {
    public:
    void read_sample_header(bool verbose) {
        if(verbose) cout << "    Reading sample header for ";
        length = readInt(4);
        loop_start = readInt(4);
        loop_length = readInt(4);
        volume = readInt(1);
        finetune = readSignedByte();
        type = readInt(1);
        panning = readInt(1);
        relative_note = readSignedByte();
        int reserved = readInt(1);
        name = readString(22);
        if(verbose) cout << name << "\n";
        if(reserved==0xAD) isADPCM=true;
    }

    void convertFromDPCM(short* &in, short* &out, int length, bool is16Bit){
        if(is16Bit){
            short carry = 0;
            for(int i=0;i<length;i++){
                short delta = in[i];
                carry += delta;
                out[i] = carry;
            };
        } else {
            byte carry = 0;
            for(int i=0;i<length;i++){
                byte delta = in[i];
                carry += delta;
                out[i] = carry;
            };
        }
    };

    void convertFromADPCM(short* &in, short* &out, int length, bool is16Bit){
        // okay i dont know how to go from ADPCM to PCM
        /* WAVEHeader  wave;
        ADPCMHeader adpcm;
        ifstream in(input, ios::binary);
        in.read((char*)&adpcm, sizeof(ADPCMHeader));
        in.read((char*)&wave, sizeof(WAVEHeader));
        char* samples = new char[wave.subchunk2Size];
        char* data = new char[adpcm.dataSize];
        in.read((char*)data, adpcm.dataSize);
        decompress(data, samples, adpcm); */

        _4ADPCM_to_16Linear(in, out, length);
    }

    void read_sample_data(bool verbose) {
        if(verbose) cout << "    Reading sample data for " << name << " (Length: " << length << ")" << "\n";
        if (hasFlag(type, 4)){
            length /= 2;
            has16Bits = true;
            if(verbose) cout << "    Sample is 16-bit\n";
        } else {
            if(verbose) cout << "    Sample is 8-bit\n";
        }
        data = new short[length];
        short* rawData = new short[length];

        if (!hasFlag(type, 4)) {
            for (int i = 0; i < length; i++) {
                rawData[i] = (short)readSignedByte();
            }
        } else {
            for (int i = 0; i < length; i++) {
                rawData[i] = readSignedShort();
            }
        };
        if(verbose) cout << "    Converting sample data from " << name << " to PCM\n";
        if(isADPCM){
            
        } else {
            convertFromDPCM(rawData, data, length, has16Bits);
        }
        
        short old_temp = 0, new_temp = 0;
        for (int i = 0; i < length; i++) {
            new_temp = data[i] + old_temp;
            data[i] = new_temp;
            old_temp = new_temp;
        }
    }
    bool has16Bits = false;
    int length = 0;
    int loop_start = 0;
    int loop_length = 0;
    int volume = 0;
    int finetune = 0;
    int type = 0;
    int panning = 0;
    int relative_note = 0;
    bool isADPCM = false;
    string name = "";
    short* data;
};

class Instrument {
    public:
    void read_data(bool verbose) {
        int header_size = readInt(4);
        name = readString(22);
        skipBytes(1); // Instrument type
        number_of_samples = readInt(2);
        if(verbose) cout << "Reading instrument: " << name << "\n";
        if(verbose) cout << "Samples: " << number_of_samples << "\n";
        if (number_of_samples > 0) {
            sample_header_size = readInt(4);
            for (int i = 0; i < 96; i++) {
                sample_number_for_all_notes[i] = (int)readByte(); // OHHhhhhh
            }
            for (int i = 0; i < 12; i++) {
                volume_envelope_points[i] = {readInt(2), readInt(2)};
            }
            for (int i = 0; i < 12; i++) {
                panning_envelope_points[i] = {readInt(2), readInt(2)};
            }
            volume_envelope_points_amount = readInt(1);
            panning_envelope_points_amount = readInt(1);
            volume_sustain_point = readInt(1);
            volume_loop_start_point = readInt(1);
            volume_loop_end_point = readInt(1);
            panning_sutain_point = readInt(1);
            panning_loop_start_point = readInt(1);
            panning_loop_end_point = readInt(1);
            volume_type = readInt(1);
            panning_type = readInt(1);
            vibtype = readInt(1);
            vibsweep = readInt(1);
            vibdepth = readInt(1);
            vibrate = readInt(1);
            volume_fadeout = readInt(2);
            reserved = readInt(2);
            skipBytes(header_size - 243);
            samples = new Sample[number_of_samples];
            for (int i = 0; i < number_of_samples; i++) {
                samples[i].read_sample_header(verbose);
            }
            for (int i = 0; i < number_of_samples; i++) {
                if (samples[i].length > 0)
                    samples[i].read_sample_data(verbose);
            }
        } else {
            skipBytes(header_size - 29);
        }
    }

    string name = "";
    int number_of_samples = 0;
    int sample_header_size = 0;
    int sample_number_for_all_notes[96];
    pair<int,int> volume_envelope_points[12];
    pair<int,int> panning_envelope_points[12];
    int volume_envelope_points_amount = 0;
    int panning_envelope_points_amount = 0;
    int volume_sustain_point = 0;
    int volume_loop_start_point = 0;
    int volume_loop_end_point = 0;
    int panning_sutain_point = 0;
    int panning_loop_start_point = 0;
    int panning_loop_end_point = 0;
    int volume_type = 0;
    int panning_type = 0;
    int vibtype = 0;
    int vibsweep = 0;
    int vibdepth = 0;
    int vibrate = 0;
    int volume_fadeout = 0;
    int reserved = 0;

    Sample* samples;
};

class XModule {
    public:
    void read_data(bool verbose) {
        skipBytes(17);  // "Extended Module:"
        name = readString(20);
        cout << "Reading module: " << name << "\n";
        skipBytes(1);  // 0x1A
        skipBytes(20);  // Tracker name
        skipBytes(2);  // Tracker version
        int header_size = readInt(4);
        song_length = readInt(2);
        song_restart_position = readInt(2);
        channels = readInt(2);
        patterns_amount = readInt(2);
        instruments_amount = readInt(2);
        if(verbose) cout << "Amount of instruments: " << instruments_amount << "\n";
        flags = readInt(2);
        tempo = readInt(2);
        bpm = readInt(2);
        pattern_order_table = new int[patterns_amount];
        if(verbose) cout << "patterns order:" << endl;
        for (int i = 0; i < header_size - 20; i++) {
            int byte = readByte();
            if (i < patterns_amount) {
                pattern_order_table[i] = byte;
                if(verbose) cout << byte << ' ';
            }
        }
        cout << endl;
        for (int i = 0; i < patterns_amount; i++) {
            Pattern pattern;
            pattern.read_data(channels,verbose);
            patterns.push_back(pattern);
        }
        for (int i = 0; i < instruments_amount; i++) {
            Instrument inst;
            inst.read_data(verbose);
            instruments.push_back(inst);
        }
        F.close();
        cout << "Done reading file" << "\n";
    }
    string name;
    int song_length;
    int song_restart_position;
    int channels;
    int patterns_amount;
    int instruments_amount;
    int flags;
    int tempo;
    int bpm;
    int* pattern_order_table;
    vector<Pattern> patterns;
    vector<Instrument> instruments;
};

void setFstream(string fp) {
    F = ifstream(fp, ios::binary);
}

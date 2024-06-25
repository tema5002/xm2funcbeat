#include <bits/stdc++.h>
#include <fstream>
using namespace std;

ifstream F("tech.xm", ios::binary);

bool hasFlag(int byte, int flag) {
    return byte & (1 << flag);
}

char readByte() {
    if (!F) {
        cout << "not functional bird emoji" << endl;
        exit(1);
    }
    char s = F.get();
    cout << s;
    return s;
}

int readInt(int b) {
    int s = 0;
    for (int i = 0; i < b; i++) {
        s += readByte() * (int)pow(256, i);
    }
    return s;
}

int readSignedByte() {
    int b = readInt(1);
    return (b >= 128) ? b - 256 : b;
}

short readSignedShort() {
    short s;
    s = readInt(1);
    s <<= 8;
    s |= readInt(1);
    return s;
}

vector<char> readBytes(int b) {
    vector<char> bytes;
    for (int i = 0; i < b; i++) {
        bytes.push_back(readByte());
    }
    return bytes;
}

string readString(int b) {
    string s;
    for (int i = 0; i < b; i++) {
        s.push_back(readByte());
    }
    return s;
}

void skipBytes(int b) {
    for (int i = 0; i < b; i++)
        readByte();
}

class Pattern {
    public:
    Pattern(int channels_amount) {
        if (channels_amount != -1) {
            skipBytes(4); // Pattern header length
            skipBytes(1); // Packing type
            number_of_rows = readInt(2);
            packed_size = readInt(2);
            
            data = new char**[number_of_rows];
            for (int i = 0; i < number_of_rows; i++) {
                data[i] = new char*[channels_amount];
                for (int j = 0; j < channels_amount; j++) {
                    char* cell = new char[5];
                    int byte = (int)readByte();
                    if (hasFlag(byte, 7)) {
                        if (hasFlag(byte, 0)) cell[0] = readByte();
                        if (hasFlag(byte, 1)) cell[1] = readByte();
                        if (hasFlag(byte, 2)) cell[2] = readByte();
                        if (hasFlag(byte, 3)) cell[3] = readByte();
                        if (hasFlag(byte, 4)) cell[4] = readByte();
                    }
                    else {
                        cell[0] = (char)byte; // note
                        cell[1] = readByte(); // instrument
                        cell[2] = readByte(); // volume
                        cell[3] = readByte(); // effect
                        cell[4] = readByte(); // effect parameter
                    }
                    data[i][j] = cell;
                }
            }
        }
    }
    int number_of_rows = 0;
    int packed_size = 0;
    char*** data;
};

class Sample {
    public:
    void read_sample_header() {
        length = readInt(4);
        loop_start = readInt(4);
        loop_length = 0;
        volume = readInt(1);
        finetune = readSignedByte();
        type = readInt(1);
        panning = readInt(1);
        relative_note = readSignedByte();
        reserved = readInt(1);
        name = readString(22);
    }
    void read_sample_data() {
        data = new short[length];
        short temp_data[length];
        if (hasFlag(type, 4)) {
            for (int i = 0; i < length; i++) {
                temp_data[i] = readSignedByte();
            }
        }
        else {
            for (int i = 0; i < length; i++) {
                temp_data[i] = readSignedShort();
            }
        }
        short temp = 0;
        for (int i = 0; i < length; i++) {
            temp += temp_data[i];
            data[i] = temp;
        }
    }
    unsigned int length = 0;
    unsigned int loop_start = 0;
    unsigned int loop_length = 0;
    int volume = 0;
    int finetune = 0;
    int type = 0;
    int panning = 0;
    int relative_note = 0;
    int reserved = 0;
    string name = "";
    short* data;
};

class Instrument {
    public:
    Instrument() {
        skipBytes(4); // Instrument header size
        name = readString(22);
        skipBytes(1); // Instrument type
        number_of_samples = readInt(2);
        if (number_of_samples > 0) {
            skipBytes(4); // Sample header size
            for (int i = 0; i < 96; i++) {
                sample_number_for_all_notes[i] = (int)readByte();
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
            samples = new Sample[number_of_samples];
            for (int i = 0; i < number_of_samples; i++) {
                samples[i].read_sample_header();
            }
            for (int i = 0; i < number_of_samples; i++) {
                samples[i].read_sample_data();
            }
        }
    }
    
    string name = "";
    int number_of_samples = 0;
    int sample_number_for_all_notes[96];
    complex<int> volume_envelope_points[12];
    complex<int> panning_envelope_points[12];
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
    XModule() {
        skipBytes(17);  // "Extended Module:"
        name = readString(20);
        skipBytes(1);  // 0x1A
        skipBytes(20);  // Tracker name
        skipBytes(2);  // Tracker version
        skipBytes(4);  // Header size
        song_length = readInt(2);
        song_restart_position = readInt(2);
        channels = readInt(2);
        patterns_amount = readInt(2);
        instruments_amount = readInt(2);
        flags = readInt(2);
        tempo = readInt(2);
        bpm = readInt(2);
        pattern_order_table = new int[patterns_amount];
        for (int i = 0; i < 256; i++) {
            int byte = readByte();
            if (i < patterns_amount) {
                pattern_order_table[i] = byte;
            }
        }
        for (int i = 0; i < patterns_amount; i++) {
            patterns.push_back(Pattern(channels));
        }
        for (int i = 0; i < instruments_amount; i++) {
            instruments.push_back(Instrument());
        }
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

int main() {
    XModule mod;
    int k = 0;
    while (F) {
        readByte();
        k++;
    }
    F.close();
    cout << k << endl;
    for (int i = 0; i < mod.patterns_amount; i++)
        cout << (int)mod.pattern_order_table[i] << ' ';
    return 0;
}
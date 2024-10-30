// hold on lemme copypaste everything WHY CANT I FORMat
// wait 
// stop i have an idea
// oh
// nvm you cant format anymore...

typedef short sample; // :yeh:

struct ADPCMHeader {
    char  chunkID[4] = {'a','d','p',' '};   // Contains the letters "adp " in ASCII form
    long  chunkSize;                        // 16 + (8 + dataSize)

    long channelCount;                      // Number of channels in the audio
    long  sampleCount;                      // Number of samples in the data

    sample ch1KeySample;                    // Key Sample for channel 1
    short  ch1StepIndex;                    // Step Table starting index for channel 1
    sample ch2KeySample;                    // Key Sample for channel 2
    short  ch2StepIndex;                    // Step Table starting index for channel 2

    char  dataID[4] = {'d','a','t','a'};    // Contains the letters "data" in ASCII form
    long  dataSize;                         // This is the number of bytes in the data
};

// Index table that specifies the modification to the index based on
// The 4-bit ADPCM difference in compression or decompression.
const char indexTable[16] = { 
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8 
};

// Quantiziation step size table that specifies how much to change
// The 16-bit PCM sample with based on the 4-bit ADPCM difference.
const int  stepsize[89] = { 
    7,     8,     9,    10,     11,    12,    13,    14,
    16,    17,    19,    21,    23,    25,    28,    31,
    34,    37,    41,    45,    50,    55,	  60,    66,
    73,    80,    88,    97,    107,   118,   130,   143,
    157,   173,   190,   209,   230,   253,   279,   307,
    337,   371,   408,   449,   494,   544,   598,   658,
    724,   796,   876,   963,   1060,  1166,  1282,  1411,
    1552,  1707,  1878,  2066,  2272,  2499,  2749,  3024,
    3327,  3660,  4026,  4428,  4871,  5358,  5894,  6484,
    7132,  7845,  8630,  9493,  10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767 
};

sample _4ADPCM_to_16Linear(const u_char adpcm, sample &prediction, short &index) {
    int difference = 0;
    int newSample = prediction;

    if (adpcm & 4) {difference += stepsize[index]; }
    if (adpcm & 2) {difference += stepsize[index] >> 1;}
    if (adpcm & 1) {difference += stepsize[index] >> 2;}
    difference += stepsize[index] >> 3;

    if (adpcm & 8) {
        difference = -difference;
    }

    newSample += difference;
    if      (newSample >  32767) {newSample =  32767;}
    else if (newSample < -32768) {newSample = -32768;}

    index += indexTable[adpcm];
    if      (index <  0) { index =  0;}
    else if (index > 88) { index = 88;}

    prediction = newSample;
    return newSample;
}

void decompress(const char* src, char* dst, const ADPCMHeader &adp){
    const u_char* dataPtr = (const u_char*)src;
    sample* samplePtr = (sample*)dst; // sample? we have that?

    short  stepIndex[2]  = { 0, 0 };
    sample prediction[2] = { 0, 0 };

    // Extract key values for channel 1
    if(adp.channelCount >= 1){
        prediction[0] = adp.ch1KeySample;
        stepIndex[0]  = adp.ch1StepIndex;
        *samplePtr = prediction[0];
        samplePtr++;
    }

    // Extract key values for channel 2
    if(adp.channelCount >= 2){
        prediction[1] = adp.ch2KeySample;
        stepIndex[1]  = adp.ch2StepIndex;
        *samplePtr = prediction[1];
        samplePtr++;
    }

    // Process all samples of each channel and store the 4-bit data
    for(int i = adp.channelCount, channel = 0, shift = 4; i < adp.sampleCount; i++){
        *samplePtr = _4ADPCM_to_16Linear((*dataPtr >> shift) & 0x0F, prediction[channel], stepIndex[channel]);
        samplePtr++;

        // If processed the last channel, go back to the first
        if(++channel == adp.channelCount){
            channel = 0;
        }

        // If the byte reached the end, start from the next
        if(shift == 0){
            shift = 4;
            dataPtr++;
        }else{
            // Else move the 4-bit shift to the right half
            shift = 0;
        }
    }
}
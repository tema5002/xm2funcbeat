#include <bits/stdc++.h>
#include "xm_reader.hpp"
using namespace std;

int main() {
    XModule mod;
    setFstream("xm/BFIce.xm");
    mod.read_data();

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
    return 0;
}
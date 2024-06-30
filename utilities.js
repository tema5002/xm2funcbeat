let cursor = 0;
let currentPattern = 0;
let timeWhenCursorWillMove = (60/bpm)*48000*tempo/24;
// let C = 4186, CS = 4435, D = 4699, DS = 4978, Ee = 5274, F = 5588, FS = 5920, G = 6272, GS = 6645, A = 7040, AS = 7459, B = 7902;
let noteHz = [4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902]; // on octave 8
let channelCurrentSounds=[...Array(channels)];
let errorBefore="";
let throwOutput="sample text";
let output=0;

for (let i = 0; i < patterns.length; i++) { // pattern
    for (let j = 0; j < patterns[i].length; j++) { // row
        for (let k = 0; k < patterns[i][j].length; k++) { // cell
            const num = patterns[i][j][k];
            const bytes = new Uint8Array(5);
            for (let i = 0; i < 5; i++) {
                bytes[i] = (num >> (i * 8)) & 0xFF;
            }
            patterns[i][j][k] = bytes; // [[cell, cell, cell], [cell, cell, cell]] -> [[[n,i,v,f,d],[n,i,v,f,d],[n,i,v,f,d]],  [[n,i,v,f,d],[n,i,v,f,d],[n,i,v,f,d]]]
        } // normally there should be a debug window from throwing an error
    } // anyway im gonna check entire browser lagged
} // normally that shouldn't happen. ok i just tried recording it and it was too low quality :/

function getSample(note, inst, volume, fx, fx_param, time) {
    let data=instruments[inst-1].samples[0].data;
    if(!data) {
        console.log("no data????? :staring_cat:");
        return 0;
    };
    return (data[(Math.floor(time))]-0x8000)/0xffff;
};

function getNoteFrequency(note,finetune,relativenote){
    return (noteHz[(note-1)%12]/(2**(8-floor((note-1)/12)))/48000)*(getNoteFrequencyWithoutRelativeNote(relativenote)/getNoteFrequencyWithoutRelativeNote(49)); // i think i fixed it :D
}
function getNoteFrequencyWithoutRelativeNote(note){
    return noteHz[(note-1)%12]/(2**(8-ceil((note-1)/12)))/48000;
}

function updateThrowDebug(){
    let out=[];
    out.push("pattern "+currentPattern);
    let patternData=patterns[pattern_order_table[currentPattern]];
    for(let i=cursor-2;i<cursor+3;i++){
        try{
            out.push((i-1).toString(16).padStart(2,"0")+" "+(i==cursor?">":".")+" "+patternData[i-1].map(a=>a.toString().padEnd(12,"\xa0")).join(" "));
        } catch {
            out.push(".. . ...");
        } // check d-stuff to hear the rsult so far
    };
    out.push(channelCurrentSounds.map(a=>JSON.stringify(a)).join("\n"));
    throwOutput=out.join("\n");
}


function addSounds(data){
    for(let ch=0;ch<data.length;ch++){
        let elem=data[ch];
        if(elem[0]!=0){
            if(elem[0]==97){
                channelCurrentSounds[ch]=null;
            }else{
                channelCurrentSounds[ch]={
                    note:elem[0],
                    inst:elem[1],
                    volume:elem[2],
                    fx:elem[3],
                    fx_param:elem[4],
                    time:0,
                    loops_passed:0
                };
            }
        }
    }
}

return function(t, sampleRate) {try{
    if(currentPattern<0) currentPattern=0;
    let time = t*sampleRate;
    if (time < 5) {
        cursor = 0; currentPattern = 0; timeWhenCursorWillMove = 0; // mb
        throw ""
    }
    if (sampleRate != 48000) throw "SAMPLE RATE SHOULD BE 48kHz, IF YOU ARE LAGGING, RECORD THE SONG";
    if (time > timeWhenCursorWillMove) { // update
        timeWhenCursorWillMove += (60/bpm)*48000*(tempo/24);
        cursor+= 1;
        let patternData=patterns[pattern_order_table[currentPattern]];
        let len=patternData.length;
        if (cursor > len) { // new pattern
            cursor=1;
            currentPattern++;
            if (currentPattern == pattern_order_table.length) currentPattern = 0;
        };
          patternData=patterns[pattern_order_table[currentPattern]]
        addSounds(patternData[cursor-1]);
    }

    output=0;

    for(let i=0;i<channelCurrentSounds.length;i++){
        let current=channelCurrentSounds[i];
        if(current){
            if(!current.freq){
                channelCurrentSounds[i].freq=getNoteFrequency(current.note,0, instruments[current.inst].samples[0].relative_note)
            }
            let freq=current.freq;
            channelCurrentSounds[i].time+=freq;
            if(current.time>instruments[current.inst-1].samples[0].data.length){
                channelCurrentSounds[i].loops_passed++;
                if(channelCurrentSounds[i].loops_passed>[inst].loop_length)
                channelCurrentSounds[i]=null;
            }
            if(channelCurrentSounds[i]){
                output+=getSample(current.note,current.inst,current.volume,current.fx,current.fx_param,current.time);
            }
        }
    }

}catch(e){if(e.stack==errorBefore)console.error(e);errorBefore=e.stack;return 0;};if((t*10)%1<0.001){updateThrowDebug();throw throwOutput};return output};
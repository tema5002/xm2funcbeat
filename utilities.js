let cursor = 0;
let currentPattern = 0;
let timeWhenCursorWillMove = (60/bpm)*48000*tempo/8;
let C = 4186, CS = 4435, D = 4699, DS = 4978, Ee = 5274, F = 5588, FS = 5920, G = 6272, GS = 6645, A = 7040, AS = 7459, B = 7902; // on octave 8
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
            patterns[i][j][k] = bytes;
        }
    }
}

function getSample(note, inst, volume, fx, fx_param, time){ // tf is this
    let data=instruments[inst-1].samples[0].data;
    if(!data) {
        console.log("no data????? :staring_cat:");
        return 0;
    }
    return data[Math.floor(time*data.length)]/0xffff*volume;
}

function updateThrowDebug(){
    let out=[];
    out.push("pattern "+currentPattern);
    let patternData=patterns[currentPattern];
    for(let i=cursor+2;i>cursor-3;i--){
        try{
            out.push((i-1==cursor?">":".")+" "+patternData[i].map(a=>a.toString().padStart(3)).join(","));
        } catch {
            out.push(". ...");
        }
    };
    out.push(JSON.stringify(channelCurrentSounds));
    throwOutput=out.join("\n");
}

return function(t, sampleRate) {try{
    if(currentPattern<0) currentPattern=0;
     let output = 0;
     let time = t*sampleRate;
     if (time < 5) {
         cursor = 0; currentPattern = 0; timeWhenCursorWillMove = 0; // mb
         throw ""
     }
     if (sampleRate != 48000) throw "SAMPLE RATE SHOULD BE 48kHz, IF YOU ARE LAGGING, RECORD THE SONG";
     if (time > timeWhenCursorWillMove) { // update
        timeWhenCursorWillMove += (60/bpm)*48000*(tempo/8);
        cursor+= 1;
        let len=patterns[pattern_order_table[currentPattern]].length;
        if (cursor > len) { // new pattern
            cursor-=len;
            currentPattern++;
            if (currentPattern == pattern_order_table.length) currentPattern = 0;
        } 
     }

    for(let i=0;i<channels;i++){
        let current=channelCurrentSounds[i];
        if(current){
            channelCurrentSounds[i].time+=1/48000;
            if(channelCurrentSounds[i].time>1){
                current=null;
            } else {
                output+=getSample(current.note,current.inst,current.volume,current.fx,current.fx_param,current.time);
            }
        }
    }

}catch(e){if(e.stack==errorBefore)console.error(e);errorBefore=e.stack;return 0;};if((t*10)%1<0.001){updateThrowDebug();throw throwOutput};return output};
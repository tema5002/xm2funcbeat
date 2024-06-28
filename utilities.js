let cursor = 0;
let currentPattern = 0;
let timeWhenCursorWillMove = (60/bpm)*48000*tempo/8;
let C = 4186, CS = 4435, D = 4699, DS = 4978, Ee = 5274, F = 5588, FS = 5920, G = 6272, GS = 6645, A = 7040, AS = 7459, B = 7902; // on octave 8
let channelCurrentSounds=[...Array(channels)];
let errorBefore="";
let throwOutput="sample text";
let output=0;

function transpose(arr) {
  return arr[0].map((_, colIndex) => arr.map(row => row[colIndex]));
}

patterns=patterns.map(a=>transpose(a));

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
    return (data.charCodeAt(Math.floor(time*data.length))-0x8000)/0xffff;
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
        }
    };
    out.push(channelCurrentSounds.map(a=>JSON.stringify(a)).join("\n"));
    throwOutput=out.join("\n");
}

function addSounds(data){
    for(let ch=0;ch<data.length;ch++){
        let elem=data[ch];
        if(elem[0]!=0){
            channelCurrentSounds[ch]={
                note:elem[0],
                inst:elem[1],
                volume:elem[2],
                fx:elem[3],
                fx_param:elem[4],
                time:0
            };
            if(elem[1]==0){
                channelCurrentSounds[ch]=null;
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
        timeWhenCursorWillMove += (60/bpm)*48000*(tempo/8);
        cursor+= 1;
          let patternData=patterns[pattern_order_table[currentPattern]];
        let len=patternData.length;
        addSounds(patternData[cursor-1]);
        if (cursor > len) { // new pattern
            cursor-=len;
            currentPattern++;
            if (currentPattern == pattern_order_table.length) currentPattern = 0;
        } 
     }

        output=0;

    for(let i=0;i<channelCurrentSounds.length;i++){
        let current=channelCurrentSounds[i];
        if(current){
            channelCurrentSounds[i].time+=1/48000;
            if(current.time>1){
                channelCurrentSounds[i]=null;
            } else {
                output+=getSample(current.note,current.inst,current.volume,current.fx,current.fx_param,current.time);
            }
        }
    }

}catch(e){if(e.stack==errorBefore)console.error(e);errorBefore=e.stack;return 0;};if((t*10)%1<0.001){updateThrowDebug();throw throwOutput};return output};
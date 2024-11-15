#include <sys/ioctl.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <JuceHeader.h>
#include <nlohmann/json.hpp>
#include <limits>
  


double lerp(double a, double b, double interp, bool isUp){
//    if(isUp){
//        interp = pow(interp, .5);
//    }else{
//        interp = pow(interp, 2);
//    }
    return (((b*2*interp)+(a*-2*interp))/2) + a;
}

int ticks = 48; // AAHhH
int tickStep = 10;

juce::MidiMessageSequence readMidi(std::string filePath, int trackIndex){
    
    juce::MidiMessageSequence seq;
    juce::MidiFile inFile;
    juce::File file = juce::File(filePath);
    juce::FileInputStream stream = juce::FileInputStream(file);
    inFile.readFrom(stream);
    
    seq.addSequence(*inFile.getTrack(trackIndex), 0);
    
    juce::MidiMessageSequence::MidiEventHolder **eventHolder;
    eventHolder = seq.begin();
    
    return seq;
}

juce::MidiMessageSequence loopMidiSequence(juce::MidiMessageSequence seq, int loopNum, int ticksPQN, int loopLenInQN){
    
    juce::MidiMessageSequence::MidiEventHolder **eventHolder;
    eventHolder = seq.begin();
    ticksPQN *= 10;
    
    juce::MidiMessageSequence seqCpy;
    unsigned int loopCnt = 0;
    for (int i = 1; i < (seq.getNumEvents() * loopNum); i++) {
        
        unsigned int timeStmpOn;
        unsigned int timeStmpOff;
        unsigned int endTime = seq.getEndTime();
        
        juce::MidiMessage msg = juce::MidiMessageSequence::MidiEventHolder(**eventHolder).message;
        if (msg.isNoteOn()) {
            timeStmpOn = msg.getTimeStamp();
            msg.setTimeStamp(timeStmpOn+(ticksPQN*loopCnt*loopLenInQN));
            seqCpy.addEvent(msg);
        }else if (msg.isNoteOff()){
            timeStmpOff = msg.getTimeStamp();
            msg.setTimeStamp(timeStmpOff+(ticksPQN*loopCnt*loopLenInQN));
            seqCpy.addEvent(msg);
        }
        seqCpy.updateMatchedPairs();
        
        eventHolder++;
        if (eventHolder == seq.end()) {
            eventHolder = seq.begin();
            loopCnt ++;
        }
    }
    seqCpy.updateMatchedPairs();
    return seqCpy;
}

juce::MidiMessageSequence groupWriteSequence(std::vector<std::vector<double>> group){
    
    std::vector<double> values;
    juce::MidiMessageSequence mainSeq;
    
    for(int k = 0; k < group.size(); k++){
        
        juce::MidiMessage ccMsg;
        juce::MidiMessageSequence seq;
        
        values = group[k];
    
    unsigned long numValues = values.size();
        int midiCC = k;
        
    for (int j = 0; j < numValues; j++) {
        
        double val1 = values[j];
        double val2 = values[(j+1)%numValues];
        bool isUp = val1 < val2;
        
        for (int i = 0; i < (ticks*numValues); i++) {
            ccMsg =
            ccMsg.controllerEvent(1, midiCC,
                                  uint8_t(lerp(val1, val2,  double(i)/(double(ticks)*double(numValues) ),isUp )));
            ccMsg.addToTimeStamp(i + (j*ticks*numValues));
            seq.addEvent(ccMsg);
            
            }
        }
        mainSeq.addSequence(seq, 0);
    }
    juce::MidiMessageSequence fileSeq = readMidi("/Users/syro/WeatherWav/Media/exampleInputMidi.mid",0); // $PATH
    
    juce::MidiMessageSequence loopedSeq;
    loopedSeq.addSequence(loopMidiSequence(fileSeq, 8, ticks, 4), 0);
    
    mainSeq.addSequence(loopedSeq, 0); 
    
    return mainSeq;
}
unsigned int randIncr = 0;

unsigned int variRand = 0;
juce::MidiBuffer groupWriteSequenceAmb(std::vector<std::vector<double>> values, unsigned long bufferOffset, int chan){
    juce::MidiBuffer buf;
    
    unsigned int size = values[0].size();
    
    for(int vari = 0; vari < values[0].size(); vari ++){
        
        double val1 = values[0][vari];
        double val2 = values[1][vari];
        
        for (int event = 0; event < (44100 * 60); event += (44100 / 16)) {
            
            unsigned int eventPos = event;
            
            double intrp = double(event)/(44100.f * 60.f);
            
            int interpVal = lerp(val1, val2, intrp, true);
            
            buf.addEvent(juce::MidiMessage::controllerEvent(chan, vari, uint8_t(interpVal)), eventPos);
            
        }
    }
    return buf;
}


juce::MidiMessageSequence writeSequence(std::vector<double> values, int midiCC){
    
    juce::MidiMessage ccMsg;
    juce::MidiMessageSequence seq; 
    unsigned long numValues = values.size();
    
    for (int j = 0; j < numValues; j++) {
        
        double val1 = values[j];
        double val2 = values[(j+1)%numValues];
        bool isUp = val1 < val2;
         
        for (int i = 0; i < (ticks*numValues); i ++ ) {
            ccMsg =
            ccMsg.controllerEvent(1, midiCC,
                                  uint8_t(lerp(val1, val2, double(i)/(double(ticks)*double(numValues)),isUp ) ) );
            ccMsg.addToTimeStamp(i + (j*ticks*numValues));
            seq.addEvent(ccMsg);
        }
    } 
    return seq;
}


juce::MidiMessageSequence writeSequence(std::vector<unsigned int> &values, int midiCC){
    
    juce::MidiMessage ccMsg;
    juce::MidiMessageSequence seq;
    int numValues = values.size();
    
    for (int j = 0; j < numValues; j++) {
        
        double val1 = values[j];
        double val2 = values[(j+1)%numValues];
        bool isUp = val1 < val2;
         
        for (int i = 0; i < (ticks*numValues); i++) {
            ccMsg =
            ccMsg.controllerEvent(1, midiCC,
                                  uint8_t(lerp(val1, val2, double(i)/(double(ticks)*double(numValues)),isUp ) ) );
            ccMsg.addToTimeStamp(i + (j*ticks*numValues));
            seq.addEvent(ccMsg);
        }
    } 
    return seq;
}


#include <sys/ioctl.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <JuceHeader.h>
#include <nlohmann/json.hpp>
#include <limits>
  


double lerp(double a, double b, double interp, bool isUp){
    if(isUp){
        interp = pow(interp, .5);
    }else{
        interp = pow(interp, 2);
    }
    return (((b*2*interp)+(a*-2*interp))/2) + a;
}


void writeSequence(std::vector<double> values, juce::StringRef name,juce::MidiFile &file){
    
    juce::MidiMessage ccMsg;
    juce::MidiMessageSequence seq; 
    int ticks = 96 ;
    unsigned long numValues = values.size();
    
    for (int j = 0; j < numValues; j++) {
        
        double val1 = values[j];
        double val2 = values[(j+1)%numValues];
        bool isUp = val1 < val2;
         
        for (int i = 0; i < (ticks*numValues); i++) {
            ccMsg =
            ccMsg.controllerEvent(1, 11,
                                  uint8_t(lerp(val1, val2, double(i)/(double(ticks)*double(numValues)),isUp ) ) );
            ccMsg.addToTimeStamp(i + (j*ticks*numValues));
            seq.addEvent(ccMsg);
        }
    } 
    juce::MidiMessage nameEvent = juce::MidiMessage::textMetaEvent(3, name);
    seq.addEvent(nameEvent);
    
    file.addTrack(seq);
}
void writeSequence(std::vector<unsigned int> values, juce::StringRef name,juce::MidiFile &file){
    
    juce::MidiMessage ccMsg;
    juce::MidiMessageSequence seq;
    int ticks = 96 ;
    int numValues = values.size();
    
    for (int j = 0; j < numValues; j++) {
        
        double val1 = values[j];
        double val2 = values[(j+1)%numValues];
        bool isUp = val1 < val2;
         
        for (int i = 0; i < (ticks*numValues); i++) {
            ccMsg =
            ccMsg.controllerEvent(1, 11,
                                  uint8_t(lerp(val1, val2, double(i)/(double(ticks)*double(numValues)),isUp ) ) );
            ccMsg.addToTimeStamp(i + (j*ticks*numValues));
            seq.addEvent(ccMsg);
        }
    }
    juce::MidiMessage nameEvent = juce::MidiMessage::textMetaEvent(3, name);
    seq.addEvent(nameEvent);
    
    file.addTrack(seq);
}



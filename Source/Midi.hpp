#include <sys/ioctl.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <JuceHeader.h>
#include <nlohmann/json.hpp> 
  
double lerp(double a, double b, double interp){
    interp = pow(interp, 2);
    return (((b*2*interp)+(a*-2*interp))/2) + a;
}

std::vector<double> normalizeValues(nlohmann::json values){
    
    unsigned long size = values.size();
    
    double max = 0;
    double min = 99999999999; // any large number
    
    for(int i = 0; i < size; i++){
        double buff = values[i];
        
        if(buff > max){
            max = buff;
        }
        if(buff < min){
            min = buff;
        }
    }
    double delta = max - min;
    
    std::vector<double> normalizedValues(size);
    double normVal = 0;
    for (int i = 0; i < size; i++) {
        
        normVal = double(values[i]) - min;
        normalizedValues[i] = normVal / delta;
        
    }
    return normalizedValues;
    
}

juce::MidiMessageSequence writeSequence(std::vector<double> values, int numOfValues){
    
    juce::MidiMessage ccMsg;
    juce::MidiMessageSequence seq;
    int ticks = 96 ;
    
    for (int j = 0; j < numOfValues; j++) {
        
        double val1 = values[j];
        double val2 = values[(j+1)%numOfValues];
         
        for (int i = 0; i < (ticks*numOfValues); i++) {
            ccMsg =
            ccMsg.controllerEvent(1, 11, uint8_t(lerp(val1, val2, double(i)/(double(ticks)*double(numOfValues)) ) * 127) );
            ccMsg.addToTimeStamp(i + (j*ticks*numOfValues));
            seq.addEvent(ccMsg);
        }
    }
    return seq;
}



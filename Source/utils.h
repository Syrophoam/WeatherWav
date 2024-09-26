//
//  utils.h
//  WeatherWav
//
//  Created by Syro Fullerton on 23/09/2024.
//
//#include "API.h" 



std::vector<double> normalizeValues(nlohmann::json values){
    
    unsigned long size = values.size();
    
    double max = 0;
    double min = std::numeric_limits<double>::max();
    
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
// make option to normalize values based on the max value in array OR max possible value in API


void normalizeResponseInt(std::vector<unsigned int> &values){
        unsigned long size = values.size();
        int max = 0;
        int min = std::numeric_limits<int>::max();
        
        for(int i = 0; i < size; i++){
            int buff = values [i];
            
            if(buff > max)
                max = buff;
                
            if(buff < min)
                min = buff;
        }
    
        double delta = max - min;
        double normVal = 0;
        for (int i = 0; i < size; i++) {
            normVal = values[i] - min;
            values[i] = uint8_t((normVal / delta)*127);
        }
}

void normalizeResponseDouble(std::vector<double> &values){
        unsigned long size = values.size();
        double max = 0;
        double min = std::numeric_limits<double>::max();
        
        for(int i = 0; i < size; i++){
            double buff = values [i];
            
            if(buff > max)
                max = buff;
                
            if(buff < min)
                min = buff;
        }
    
        double delta = max - min;
        double normVal = 0;
        for (int i = 0; i < size; i++) {
            normVal = values[i] - min;
            values[i] = uint8_t((normVal / delta)*127);
        }
}

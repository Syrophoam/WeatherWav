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
        normalizedValues[i] *= 127;
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

void initCoordLUT(std::vector<std::vector<double>> &coord, struct winsize &ws){
    
    double x = 0;
    double y = 0;
    std::vector<double> xy(2);
    double aspectRatio = double(ws.ws_xpixel)/double(ws.ws_ypixel);
//    std::cout << aspectRatio;
    aspectRatio = 1.666;

    
    for (int i = 0; i < 160; i++) {
        x = double(i)/160.f;
//        x /= aspectRatio;
        x *= .5f;
        x *= 2.f;
        x -= .5f;
        x *= 2.f;
        x *= 180.f; // not right ????

        coord[0].at(i) = x;

        
        for (int j = 0; j < 48; j++) {
            y = double(j)/48.f;
//            y /= 2.f;
//            y /= aspectRatio;
            y *= .5f;
            y *= 2.f;
            y -= .5f;
            y *= 2.f;
            y *= 90.f;

            coord[1].at(j) = y;

        }
    }
    
}

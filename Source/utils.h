//
//  utils.h
//  WeatherWav
//
//  Created by Syro Fullerton on 23/09/2024.
//
//#include "API.h" 


std::vector<double> normalizeValues(nlohmann::json values){
    
    unsigned long size = values.size();
    
    double max = -std::numeric_limits<double>::max();
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


void formatResponse(std::vector<std::vector<double>> variableGroup[], nlohmann::json variables){

    std::vector<double> airVisibility, airHumidity, airPressure, airTemperature, airCAPE;
    std::vector<double> cloudCover, cloudBase;
    std::vector<double> windSpeedEast, windSpeedWest, windSpeedGust;
    std::vector<double> fluxPrecipitation, fluxRadiationLongwave, fluxRadiationShortwave;
    std::vector<double> currentSpeedNorth, currentSpeedEast, currentSpeedNorthBarotropic, currentSpeedEastBarotropic;
    std::vector<double> seaDepthSurface, seaDepthSeaLevel, seaTempurature;
    std::vector<double> waveDirectionMean, wavePeriodPeak, waveHeight;
        
    
        airVisibility = normalizeValues(variables["air.visibility"]["data"]);
        airHumidity = normalizeValues(variables["air.humidity.at-2m"]["data"]);
        airPressure =  normalizeValues(variables["air.pressure.at-sea-level"]["data"]);
        airTemperature = normalizeValues(variables["air.temperature.at-2m"]["data"]);
        airCAPE = normalizeValues(variables["atmosphere.convective.potential.energy"]["data"]);
        
//        cloudBase = normalizeValues(variables["cloud.base.height"]["data"]);
        cloudBase = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0};
        cloudCover = normalizeValues(variables["cloud.cover"]["data"]);
        
        windSpeedEast = normalizeValues(variables["wind.speed.eastward.at-10m"]["data"]);
        windSpeedWest = normalizeValues(variables["wind.speed.westward.at-10m"]["data"]);
        windSpeedGust = normalizeValues(variables["wind.speed.gust.at-10m"]["data"]);
        
        fluxPrecipitation = normalizeValues(variables["precipitation.rate"]["data"]);
        fluxRadiationLongwave = normalizeValues(variables["radiation.flux.downward.longwave"]["data"]);
        fluxRadiationShortwave= normalizeValues(variables["radiation.flux.downward.shortwave"]["data"]);
        
        waveDirectionMean = normalizeValues(variables["wave.direction.mean"]["data"]);
        wavePeriodPeak = normalizeValues(variables["wave.period.peak"]["data"]);
        waveHeight = normalizeValues(variables["wave.height"]["data"]);
        
        variableGroup[0] = {airVisibility,airHumidity,airPressure,airTemperature,airCAPE};
        variableGroup[1] = {cloudCover};
        variableGroup[2] = {windSpeedEast,windSpeedWest,windSpeedGust};
        variableGroup[3] = {fluxPrecipitation, fluxRadiationLongwave, fluxRadiationShortwave};
        variableGroup[4] = {waveDirectionMean, wavePeriodPeak, waveHeight};
}

void normalizeResponseInt(std::vector<unsigned int> &values){
        unsigned long size = values.size();
        int max = -std::numeric_limits<double>::max();
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
        double max = -std::numeric_limits<double>::max();
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

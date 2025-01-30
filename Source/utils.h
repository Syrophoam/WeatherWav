//
//  utils.h
//  WeatherWav
//
//  Created by Syro Fullerton on 23/09/2024.
//

#include <iostream>
//#include "API.h"
using namespace nlohmann;

std::vector<u_int8_t> normalizeValues(json values){
    
    unsigned long size = values.size();
    double max, min;
    
    max = *std::max_element(values.begin(), values.end());
    min = *std::min_element(values.begin(), values.end());
    
    float delta = max - min;
    
    std::vector<u_int8_t> normalizedValues(size);
    double normVal;
    
    for (int i = 0; i < size; i++) {
        
        normVal = double(values[i]) - min;
        normalizedValues[i] = normVal / delta;
        normalizedValues[i] *= 127;
    }
    
    return normalizedValues;
}

struct variableMinMax {
    double min, max;
};

std::vector<variableMinMax> minMaxVec;

void initNorm(json data){
    
    json variables = data["variables"];
    json numVari = variables.size();
    
    json::iterator dataIter = variables.begin();
    
    for(int i = 0; i < numVari; i++){
        json vari = dataIter.value();
        variableMinMax variInfo;
        auto values = vari["data"];
        
        variInfo.min = *std::min_element(values.begin(), values.end());
        variInfo.max = *std::max_element(values.begin(), values.end());
        
        minMaxVec.push_back(variInfo);
        dataIter ++;
    }
}

u_int16_t normalizeAmb(json data){
    
    json variables = data["variables"];
    
    json numVari = variables.size();
    json::iterator dataIter = variables.begin();
    
    for(int i = 0; i < numVari; i++){
        json vari1 = dataIter.value()["data"][0];
        json vari2 = dataIter.value()["data"][1];
        
        double buff = 0;
        if(vari1 != nullptr){
            buff = vari1;
        }
        
        
        double min = minMaxVec[i].min;
        double max = minMaxVec[i].max;
        
        double delta = max - min;
        
        double normVal = 0;
        
        
        normVal = buff - min;
        normVal = normVal / delta;
        normVal *= 127;
        
        dataIter ++;
    }
    
    return 0;
}


void formatResponse(std::vector<std::vector<u_int8_t>> variableGroup[], nlohmann::json variables){

    std::vector<u_int8_t> airVisibility, airHumidity, airPressure, airTemperature, airCAPE;
    std::vector<u_int8_t> cloudCover, cloudBase;
    std::vector<u_int8_t> windSpeedEast, windSpeedNorth, windDirection, windSpeed;
    std::vector<u_int8_t> fluxPrecipitation, fluxRadiationLongwave, fluxRadiationShortwave;
    std::vector<u_int8_t> currentSpeedNorth, currentSpeedEast, currentSpeedNorthBarotropic, currentSpeedEastBarotropic;
    std::vector<u_int8_t> seaDepthSurface, seaDepthSeaLevel, seaTempurature;
    std::vector<u_int8_t> waveDirectionMean, wavePeriodPeak, waveHeight;
        
     
    airVisibility =         normalizeValues(variables["air.visibility"]                         ["data"]);
    airHumidity =           normalizeValues(variables["air.humidity.at-2m"]                     ["data"]);
    airPressure =           normalizeValues(variables["air.pressure.at-sea-level"]              ["data"]);
    airTemperature =        normalizeValues(variables["air.temperature.at-2m"]                  ["data"]);
    airCAPE =               normalizeValues(variables["atmosphere.convective.potential.energy"] ["data"]);
    
    cloudBase =             normalizeValues(variables["cloud.base.height"]                      ["data"]);
    cloudCover =            normalizeValues(variables["cloud.cover"]                            ["data"]);
    
    windSpeedEast =         normalizeValues(variables["wind.speed.eastward.at-10m"]             ["data"]);
    windSpeedNorth =        normalizeValues(variables["wind.speed.northward.at-10m"]            ["data"]);
    windDirection =         normalizeValues(variables["wind.direction.at-10m"]                  ["data"]);
    windSpeed =             normalizeValues(variables["wind.speed.at-10m"]                      ["data"]);
    
    fluxPrecipitation =     normalizeValues(variables["precipitation.rate"]                     ["data"]);
    fluxRadiationLongwave = normalizeValues(variables["radiation.flux.downward.longwave"]       ["data"]);
    fluxRadiationShortwave= normalizeValues(variables["radiation.flux.downward.shortwave"]      ["data"]);
    
    waveDirectionMean =     normalizeValues(variables["wave.direction.mean"]                    ["data"]);
    wavePeriodPeak =        normalizeValues(variables["wave.period.peak"]                       ["data"]);
    waveHeight =            normalizeValues(variables["wave.height"]                            ["data"]);
    
    
    variableGroup[0] = {airVisibility,airHumidity,airPressure,airTemperature,airCAPE};
    variableGroup[1] = {cloudCover, cloudBase};
    variableGroup[2] = {windSpeedEast,windSpeedNorth,windDirection, windSpeed};
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

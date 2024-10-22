//
//  API.h
//  WeatherWav
//
//  Created by Syro Fullerton on 23/09/2024.
//


#include <iostream>
#include <string>
#include <array>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <format>
#include <map>

#include <curl/curl.h>

#include <fstream>
#include <nlohmann/json.hpp>

struct openWeather{
    unsigned int count;
    std::vector<unsigned int> dt; //Time of data forecasted, unix, UTC
    
    std::vector<double> temp; // Temperature. Unit Default: Kelvin, Metric: Celsius, Imperial: Fahrenheit
    std::vector<double> feels_like; //This temperature parameter accounts for the human perception of weather. Unit Default: Kelvin, Metric: Celsius, Imperial: Fahrenheit
    std::vector<double> temp_min; // Minimum temperature at the moment of calculation. This is minimal forecasted temperature (within large megalopolises and urban areas), use this parameter optionally. Please find more info here. Unit Default: Kelvin, Metric: Celsius, Imperial: Fahrenheit
    std::vector<double> temp_max; // Maximum temperature at the moment of calculation. This is maximal forecasted temperature (within large megalopolises and urban areas), use this parameter optionally. Please find more info here. Unit Default: Kelvin, Metric: Celsius, Imperial: Fahrenheit
    std::vector<unsigned int> pressure; // Atmospheric pressure on the sea level by default, hPa
    std::vector<unsigned int> sea_level; // Atmospheric pressure on the sea level, hPa
    std::vector<unsigned int> grnd_level; // Atmospheric pressure on the ground level, hPa
    std::vector<unsigned int> humidity; // Humidity, %
    
    std::vector<std::string> main;// Group of weather parameters (Rain, Snow, Clouds etc.)
    std::vector<std::string> description; // Weather condition within the group. Please find more here. You can get the output in your language. Learn more

    std::vector<unsigned int> clouds;

    std::vector<double> windSpeed; // Wind speed. Unit Default: meter/sec, Metric: meter/sec, Imperial: miles/hour
    std::vector<unsigned int> windDeg;// Wind direction, degrees (meteorological)
    std::vector<double> windGust; // Wind gust. Unit Default: meter/sec, Metric: meter/sec, Imperial: miles/hour
    std::vector<unsigned int> visibility; // Average visibility, metres. The maximum value of the visibility is 10km
    std::vector<double> pop; // Probability of precipitation. The values of the parameter vary between 0 and 1, where 0 is equal to 0%, 1 is equal to 100%
    
    std::vector<double> rain3h; // Rain volume for last 3 hours, mm. Please note that only mm as units of measurement are available for this parameter
    std::vector<double> snow3h; // Snow volume for last 3 hours. Please note that only mm as units of measurement are available for this parameter

    std::vector<std::string> sysPod; // Part of the day (n - night, d - day)
    
    std::string  cityName; // City name. Please note that built-in geocoder functionality has been deprecated. Learn more here
    std::string cityCountry; // Country code (GB, JP etc.). Please note that built-in geocoder functionality has been deprecated. Learn more here
    
    unsigned int citySunrise; // Sunrise time, Unix, UTC
    unsigned int citySunset; // Sunset time, Unix, UTC
};
  
using json = nlohmann::json;

struct response {
  char *memory;
  size_t size;
};

static size_t
mem_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct response *mem = (struct response *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

json requestMS(std::map<std::string,float> pos, int repeats, int timeGap){ // metservice
    json jsonReq = {};

    json::array_t positionArray = {pos};
    jsonReq["points"] = positionArray;
    
    json::array_t variables =
    {"wave.height","wave.period.peak","wave.direction.mean",
        "air.visibility","air.humidity.at-2m","air.pressure.at-sea-level","air.temperature.at-2m",
        "atmosphere.convective.potential.energy",
        "cloud.cover","precipitation.rate","radiation.flux.downward.longwave",
        "radiation.flux.downward.shortwave",
        "wind.direction.at-10m", "wind.speed.at-10m","wind.speed.eastward.at-10m", "wind.speed.northward.at-10m" }; 
    jsonReq["variables"] = variables;
    
    const auto now = std::chrono::system_clock::now();
    
    std::string currentTime;
    //currentTime = "2024-09-23T06:15:00Z";
    currentTime = std::format("{:%FT%H:%M:00Z}", now);
    
    std::string timeGapString;
    switch (timeGap) {
        case 0:{
            timeGapString = "1h";
            break;
        }
        case 1:{
            timeGapString = "1m";
        }
            
        default:
            break;
    }
    
    std::map time = std::map<std::string, std::string>{
        {"from",currentTime}, {"interval",timeGapString}
    };
    
    jsonReq["time"] = time;
    
    //int repeats = 4;
    std::cout << "repeats" << std::endl;
    //std::cin >> repeats;
    jsonReq["time"]["repeat"] = repeats; // repeats
    
    return jsonReq;
}

void fillStruct(json data, struct openWeather &OWVec, int size){
        
        
        OWVec.dt.resize(size);
        OWVec.visibility.resize(size);
        OWVec.pop.resize(size);
        OWVec.main.resize(size);
        OWVec.description.resize(size);
        OWVec.temp.resize(size);
        OWVec.temp_min.resize(size);
        OWVec.temp_max.resize(size);
        OWVec.feels_like.resize(size);
        OWVec.pressure.resize(size);
        OWVec.sea_level.resize(size);
        OWVec.grnd_level.resize(size);
        OWVec.humidity.resize(size);
        OWVec.clouds.resize(size);
        OWVec.windDeg.resize(size);
        OWVec.windGust.resize(size);
        OWVec.windSpeed.resize(size);
        OWVec.sysPod.resize(size);
        OWVec.rain3h.resize(size);
        OWVec.snow3h.resize(size);
         
        
        
    for(int i = 0; i < size; i++){
        
        json list =             data["list"][i];
        
        OWVec.dt[i] =           list["dt"];
        OWVec.visibility[i] =   list["visibility"];
        OWVec.pop[i] =          list["pop"];
        
        OWVec.main[i] =         list["weather"][0]["main"];
        OWVec.description[i] =  list["weather"][0]["description"];
        
        json main =             list["main"];
        OWVec.temp[i] =         main["temp"];
        OWVec.temp_min[i] =     main["temp_min"];
        OWVec.temp_max[i] =     main["temp_max"];
        OWVec.feels_like[i] =   main["feels_like"];
        OWVec.pressure[i] =     main["pressure"];
        OWVec.sea_level[i] =    main["sea_level"];
        OWVec.grnd_level[i] =   main["grnd_level"];
        OWVec.humidity[i] =     main["humidity"];
        
        OWVec.clouds[i] =       list["clouds"]["all"];
        OWVec.windDeg[i] =      list["wind"]["deg"];
        OWVec.windGust[i] =     list["wind"]["gust"];
        OWVec.windSpeed[i] =    list["wind"]["speed"];
        
        OWVec.sysPod[i] =       list["sys"]["pod"];
        
        nlohmann::json rain =   list["rain"]; 

        
        if (!rain.is_null()){
            OWVec.rain3h[i] =   list["rain"]["3h"];
        }else{
            OWVec.rain3h[i] = 0.f;
        }
        
        nlohmann::json snow =   list["snow"];
        
        if (!snow.is_null()){
            OWVec.snow3h[i] =   list["snow"]["3h"];
        }else{
            OWVec.snow3h[i] = 0.f;
        }

    }
        
}


std::vector<std::vector<int>> getTime(json data){
    
    
    unsigned int size = data["dimensions"]["time"]["data"].size();
    std::vector<std::vector<int>> timeVec;
    timeVec.resize(size);
     
    char hour[2];
    char min[2];
    for(int i = 0; i < size; i++){
        timeVec[i].resize(2);
        std::string timeStr = data["dimensions"]["time"]["data"][i];
        size_t timePos = timeStr.find("T");
    
        timeStr.erase(timeStr.begin(), timeStr.begin()+timePos+1);
        timeStr.copy(hour,2);
        timeStr.erase(timeStr.begin(),timeStr.begin()+3);
        timeStr.copy(min,2);
          
        timeVec[i][0] = std::stoi(min);
        timeVec[i][1] = std::stoi(hour);
        // hour works but min doesnt cus duuuhhh so obvious
        
    }
    return timeVec;
}

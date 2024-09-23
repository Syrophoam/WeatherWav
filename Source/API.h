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

using json = nlohmann::json;
size_t dataSize = 0;
size_t CURLWriteFunction(void *ptr, size_t size, size_t nmemb, void* userData){
    
    char** stringToWrite = (char**)userData;
    const char* input = (const char*)ptr;
    if(nmemb==0) return 0;
    if (!*stringToWrite) {
        *stringToWrite = static_cast<char*>(malloc(nmemb+1));
    }
    else
        *stringToWrite = static_cast<char*>(realloc(*stringToWrite, size+nmemb+1));
        
    memcpy(*stringToWrite+dataSize, input, nmemb);
    dataSize += nmemb;
    (*stringToWrite)[dataSize] = '\0';
    return nmemb;
}

json request(std::map<std::string,float> pos, int repeats){
    json jsonReq = {};

    
    json::array_t positionArray = {pos};
    jsonReq["points"] = positionArray;
    
    json::array_t variables = {"wave.height","air.visibility"}; // variable list
    jsonReq["variables"] = variables;
    
    const auto now = std::chrono::system_clock::now();
    
    std::string currentTime;// = std::format("{:%FT%H:%M:00Z}", now);
    //currentTime = "2024-09-23T06:15:00Z";
    currentTime = std::format("{:%FT%H:%M:00Z}", now);
    
    
    std::map time = std::map<std::string, std::string>{
        {"from",currentTime}, {"interval","1h"}
    };
    
    jsonReq["time"] = time;
    
    //int repeats = 4;
    std::cout << "repeats" << std::endl;
    //std::cin >> repeats;
    jsonReq["time"]["repeat"] = repeats; // repeats
    
    return jsonReq;
}

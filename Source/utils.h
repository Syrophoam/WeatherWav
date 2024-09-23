//
//  utils.h
//  WeatherWav
//
//  Created by Syro Fullerton on 23/09/2024.
//

#ifndef utils_h
#define utils_h

void waitFrame(){
    struct timespec tim;
    tim.tv_nsec = 50'000'000;
    nanosleep(&tim, NULL);
}


#endif /* utils_h */

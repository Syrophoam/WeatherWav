//
//  audio.h
//  WeatherWav
//
//  Created by syro Fullerton on 29/09/2024.
//

#ifndef audio_h
#define audio_h
//#include "Main.hpp" 

typedef struct mainThread {
    double a;
    double b;
    int c;
} test;



void *mainThreadFunc(void* input){
    
    mainThread *mainT=(mainThread *)input;
    
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        mainT->a += 1;
        mainT->a = fmod(mainT->a,32.f);
//        mainT->a /= 32.f;
    }
    
}


#endif /* audio_h */

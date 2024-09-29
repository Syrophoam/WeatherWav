#include "MainComponent.h"
#include "../main.hpp"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}


int sampleRateG;
pthread_t t; 
mainThread mainT;

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    sampleRateG = sampleRate;
    pthread_create(&t, NULL, mainFunc, (void *)&mainT); 
    mainT.a = 0;
}

int cnt = 0;
double ramp = 0;
double fbEnv = 0;
void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
    auto* leftBuffer  = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    auto* rightBuffer = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);
    
//    fbEnv += double(mainT.b)/2.f;
    
    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            if (mainT.b == 1) {
                mainT.b = 0;
                fbEnv = 1.f;
                cnt = 0;
            }
            
            ramp = double(cnt)/double(sampleRateG);
            ramp = fmod(ramp*440, 1.f);
            
//            fbEnv -= .0001f;
            fbEnv *= 0.9998f;
            
            fbEnv = fmax(fbEnv, 0.f);
            
            double sin = 0.f;
            for (float i = 1; i <= 4 ; i += 1) {
                sin += std::sin(fmod(ramp*i,1.f)*2.f*M_PI);
            }
            sin /= 8.f;
            
            leftBuffer[sample]  = sin  * fbEnv;
            rightBuffer[sample] = sin * fbEnv;
                
            cnt ++;
            cnt %= sampleRateG;
        }

}


void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

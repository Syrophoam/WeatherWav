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
std::unique_ptr<juce::MidiOutput> midiO = nullptr;

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
    
    // look at this https://docs.juce.com/master/tutorial_handling_midi_events.html
    juce::Array<juce::MidiDeviceInfo> devLs = juce::MidiOutput::getAvailableDevices();
    juce::String devId = devLs[0].identifier;
    midiO = juce::MidiOutput::openDevice(devId);
    midiO->startBackgroundThread();
    std::cout<<devLs[0].name<<std::endl;
    
    midiO->clearAllPendingMessages();
 
}

int cnt = 0;
double ramp = 0;
double fbEnv = 0;


void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    
    bufferToFill.clearActiveBufferRegion();
    auto* leftBuffer  = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    auto* rightBuffer = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);
    juce::Time miliSecCounter;
//    fbEnv += double(mainT.b)/2.f;
    
    
    juce::MidiBuffer midiBuf;
    midiBuf.addEvent(juce::MidiMessage::noteOn(1, 30, uint8_t(60)), 0);
    midiBuf.addEvent(juce::MidiMessage::noteOff(1, 30),100);
    
    midiBuf.addEvent(juce::MidiMessage::noteOn(1, 37, uint8_t(60)), 5000);
    midiBuf.addEvent(juce::MidiMessage::noteOff(1, 37),7500);
    
    if(mainT.sendMsg){
        mainT.sendMsg = false;
        midiO->sendBlockOfMessages(mainT.midiBuffer, miliSecCounter.getMillisecondCounter()+1, sampleRateG);
    }
        
    int deltaCC = - 1;
    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            
//            if(mainT.msg.isController()){
//                if(mainT.msg.getControllerValue()!= deltaCC){
//                    midiO->sendMessageNow(mainT.msg);
//                    deltaCC = mainT.msg.getControllerValue();
//                } 
//            }
            
            
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
//    pthread_join(t, NULL); idk where to put this, surley not here
    

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

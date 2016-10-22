// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE sine wave synthesizer plug-in

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Smooth.h"
#include "Sine.h"

//==============================================================================
/**
*/
class BasicAudioPlugInAudioProcessor  : public AudioProcessor
{
public:
    struct FMSound : public SynthesiserSound
    {
        FMSound();
        ~FMSound();
        bool appliesToNote (int /*midiNoteNumber*/) override;
        bool appliesToChannel (int /*midiChannel*/) override;
    };
    
    struct FMVoice : public SynthesiserVoice
    {
        FMVoice();
        ~FMVoice();
        bool canPlaySound (SynthesiserSound* sound) override;
        void startNote (int midiNoteNumber, float velocity,
                        SynthesiserSound*, int /*currentPitchWheelPosition*/) override;
        void stopNote (float /*velocity*/, bool allowTailOff) override;
        void pitchWheelMoved (int /*newValue*/) override;
        void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override;
        void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
        
    private:
        Sine carrier, modulator;
        Smooth smooth[2];
        double carrierFrequency, index, level, envelope;
        bool onOff, tailOff;
    };
    
    struct SynthAudioSource : public AudioSource
    {
        SynthAudioSource (MidiKeyboardState& keyState);
        ~SynthAudioSource ();
        void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override;
        void releaseResources() override;
        void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
        MidiMessageCollector midiCollector;
        MidiKeyboardState& keyboardStateSource;
        Synthesiser synth;
    };
    
    //==============================================================================
    BasicAudioPlugInAudioProcessor();
    ~BasicAudioPlugInAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    Sine sine;
    AudioSourcePlayer audioSourcePlayer;
    SynthAudioSource synthAudioSource;
    MidiKeyboardState keyboardState;
    
    float onOff, gain;
    int samplingRate;
    
private:
    float** audioBuffer;
    int nChans;
    MidiMessageCollector messageCollector;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicAudioPlugInAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED

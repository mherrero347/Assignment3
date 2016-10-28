// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE sine wave synthesizer plug-in

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "FMSynthSource.h"

//==============================================================================
BasicAudioPlugInAudioProcessor::BasicAudioPlugInAudioProcessor() : onOff (0.0), gain (0.0), samplingRate(0.0)
{
    nChans = getTotalNumOutputChannels();
    audioBuffer = new float*[nChans];
    initializeSynth();
}

BasicAudioPlugInAudioProcessor::~BasicAudioPlugInAudioProcessor()
{
    delete [] audioBuffer;
}

void BasicAudioPlugInAudioProcessor::initializeSynth()
{
    // Add some voices to our synth, to play the sounds..
    int nVoices = 4;
    for (int i = nVoices; --i >= 0;)
    {
        synth.addVoice (new FMVoice());
    }
    
    synth.clearSounds();
    synth.addSound (new FMSound());
}

//==============================================================================
const String BasicAudioPlugInAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicAudioPlugInAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicAudioPlugInAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double BasicAudioPlugInAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicAudioPlugInAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicAudioPlugInAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicAudioPlugInAudioProcessor::setCurrentProgram (int index)
{
}

const String BasicAudioPlugInAudioProcessor::getProgramName (int index)
{
    return String();
}

void BasicAudioPlugInAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BasicAudioPlugInAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    samplingRate = sampleRate;
    synth.setCurrentPlaybackSampleRate (sampleRate);
    keyboardState.reset();
}

void BasicAudioPlugInAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicAudioPlugInAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet)
{
    // Reject any bus arrangements that are not compatible with your plugin

    const int numChannels = preferredSet.size();

   #if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
   #elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
   #else
    if (numChannels != 1 && numChannels != 2)
        return false;

    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
   #endif

    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif

void BasicAudioPlugInAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, numSamples);
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples);
}

//==============================================================================
bool BasicAudioPlugInAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BasicAudioPlugInAudioProcessor::createEditor()
{
    return new BasicAudioPlugInAudioProcessorEditor (*this);
}

//==============================================================================
void BasicAudioPlugInAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicAudioPlugInAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicAudioPlugInAudioProcessor();
}

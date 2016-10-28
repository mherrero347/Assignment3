// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE sine wave synthesizer plug-in

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class BasicAudioPlugInAudioProcessorEditor  :
public AudioProcessorEditor,
private Slider::Listener,
private ToggleButton::Listener,
private ComboBox::Listener,
private MidiInputCallback,
private MidiKeyboardStateListener
{
public:
    BasicAudioPlugInAudioProcessorEditor (BasicAudioPlugInAudioProcessor&);
    ~BasicAudioPlugInAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider* slider) override;
    void buttonClicked (Button* button) override;
    void comboBoxChanged (ComboBox* box) override;
    void setMidiInput (int index);
    void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override;
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicAudioPlugInAudioProcessor& processor;
    
    // 0 = gain, 1 = distortion, 2 = attack, 3 = release
    Slider sliderArray[4];
    Label labelArray[4];
    ToggleButton onOffButton;
    
    AudioDeviceManager deviceManager;
    ComboBox midiInputList;
    Label midiInputListLabel;
    
    int lastInputIndex;
    bool isAddingFromMidiInput;
    
    MidiKeyboardComponent keyboardComponent;
    Random random;
    int currentDownColor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicAudioPlugInAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED

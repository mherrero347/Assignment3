// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE sine wave synthesizer plug-in

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <string>


//==============================================================================
BasicAudioPlugInAudioProcessorEditor::BasicAudioPlugInAudioProcessorEditor (BasicAudioPlugInAudioProcessor& p)
        : AudioProcessorEditor (&p), processor (p),
          isAddingFromMidiInput (false),
          keyboardComponent(processor.keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 300);
    
    /*String labelTextArray [4] = {"gain", "distortion", "attack", "release"};
    
    //Configures each of the sliders and pointers in the sliderArray to be a rotary
    for(int i = 0; i < sizeof(sliderArray)/sizeof(Slider); i++) {
        sliderArray[i].setSliderStyle(Slider::Rotary);
        sliderArray[i].setRotaryParameters(1.25*float_Pi, 2.75*float_Pi, true);
        sliderArray[i].setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
        sliderArray[i].setPopupDisplayEnabled(true, this);
        sliderArray[i].addListener(this);
        sliderArray[i].setRange (0.0, 127, 1);
        sliderArray[i].setValue(63);
        addAndMakeVisible (sliderArray[i]);
        
        labelArray[i].setText(labelTextArray[i], dontSendNotification);
        labelArray[i].setJustificationType(Justification::centredBottom);
        labelArray[i].attachToComponent (&sliderArray[i], false);
        addAndMakeVisible(labelArray[i]);
    }*/
    
    //MIDI Keyboard Component
    addAndMakeVisible(keyboardComponent);
    keyboardComponent.setKeyWidth(getWidth()/30.0);
    processor.keyboardState.addListener(this);
    
    //MIDI Input List
    addAndMakeVisible(midiInputList);
    midiInputList.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
    const StringArray midiInputs(MidiInput::getDevices());
    midiInputList.addItemList(midiInputs, 1);
    midiInputList.addListener(this);
    
    for (int i = 0; i < midiInputs.size(); ++i)
    {
        if (deviceManager.isMidiInputEnabled (midiInputs[i]))
        {
            setMidiInput(i);
            break;
        }
    }
    
    // if no enabled devices were found just use the first one in the list
    if (midiInputList.getSelectedId() == 0)
        setMidiInput (0);
}


BasicAudioPlugInAudioProcessorEditor::~BasicAudioPlugInAudioProcessorEditor()
{
}

//==============================================================================
void BasicAudioPlugInAudioProcessorEditor::paint (Graphics& g){
    File background_file ("/Users/matthewherrero/Google Drive/2016 - Fall/Music 256/Assignments/splatter_midi_back.jpg");
    Image background = ImageCache::getFromFile(background_file);
    g.drawImage(background, 0, 0, getWidth(), getHeight(), 0, 0, background.getWidth(), background.getHeight(), false );
}

void BasicAudioPlugInAudioProcessorEditor::resized(){
    /*for(int i = 0; i < sizeof(sliderArray)/sizeof(Slider); i++) {
        float section  = getWidth()/4.0;
        sliderArray[i].setBounds(section*i + (section-80)/2.0, 30, 80, 80);
    }*/
    midiInputList.setBounds(25, getHeight()-20, getWidth()-50, 10);
    keyboardComponent.setBounds(25, getHeight()-160, getWidth()-50, 100);
}

void BasicAudioPlugInAudioProcessorEditor::sliderValueChanged(Slider *slider){
    if (processor.samplingRate > 0.0){
        
    }
}

void BasicAudioPlugInAudioProcessorEditor::buttonClicked(Button *button){
    if(button == &onOffButton && onOffButton.getToggleState()){
        processor.onOff = 1.0;
    }
    else{
        processor.onOff = 0.0;
    }
}

//==============================================================================
void BasicAudioPlugInAudioProcessorEditor::comboBoxChanged (ComboBox* box) {
    if (box == &midiInputList)
        setMidiInput (midiInputList.getSelectedItemIndex());
}

void BasicAudioPlugInAudioProcessorEditor::setMidiInput (int index){
    const StringArray list (MidiInput::getDevices());
    deviceManager.removeMidiInputCallback (list[lastInputIndex], this);
    const String newInput (list[index]);
    if (! deviceManager.isMidiInputEnabled (newInput))
        deviceManager.setMidiInputEnabled (newInput, true);
    deviceManager.addMidiInputCallback (newInput, this);
    midiInputList.setSelectedId (index + 1, dontSendNotification);
    lastInputIndex = index;
}

void BasicAudioPlugInAudioProcessorEditor::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message){
    const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
    processor.keyboardState.noteOn(message.getChannel(), message.getNoteNumber(), message.getVelocity());
    MessageManagerLock mmLock;
    keyboardComponent.setColour (MidiKeyboardComponent::keyDownOverlayColourId, Colour ((uint8)random.nextInt(255),
                                                                                        (uint8)random.nextInt(255),
                                                                                        (uint8)random.nextInt(255)));
}

void BasicAudioPlugInAudioProcessorEditor::handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    if (! isAddingFromMidiInput) {
        MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
        MessageManagerLock mmLock;
        keyboardComponent.setColour (MidiKeyboardComponent::keyDownOverlayColourId, Colour ((uint8)random.nextInt(255),
                                                                                            (uint8)random.nextInt(255),
                                                                                            (uint8)random.nextInt(255)));
    }
}
void BasicAudioPlugInAudioProcessorEditor::handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity){
    if (! isAddingFromMidiInput)
    {
        MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber));
        MessageManagerLock mmLock;
        int blackNotes [5] = {1,3,6,8,10};
        int distFromC = (midiNoteNumber - 60) % 12;
        int* foundNote = std::find(std::begin(blackNotes), std::end(blackNotes), distFromC);
        if(foundNote == std::end(blackNotes) ){
            keyboardComponent.setColour (MidiKeyboardComponent::keyDownOverlayColourId, Colours::white);
        } else {
            keyboardComponent.setColour (MidiKeyboardComponent::keyDownOverlayColourId, Colours::black);
        }
    }
}

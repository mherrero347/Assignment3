/*
  ==============================================================================

    FMSynthSource.h
    Created: 22 Oct 2016 10:19:07am
    Author:  Matthew Herrero

  ==============================================================================
*/

#ifndef FMSYNTHSOURCE_H_INCLUDED
#define FMSYNTHSOURCE_H_INCLUDED

// Not really using this here, but we need it for things to work
struct FMSound : public SynthesiserSound
{
    FMSound() {}
    
    ~FMSound() {}
    bool appliesToNote (int /*midiNoteNumber*/) override        { return true; }
    bool appliesToChannel (int /*midiChannel*/) override        { return true; }
};

// The FM synth voice. The FM synth is hardcoded here but ideally it should be in its own class
// to have a clear hierarchy (Sine -> FMSynth -> FMVoice)
struct FMVoice : public SynthesiserVoice
{
    FMVoice():
    carrierFrequency(440.0),
    index(0.0),
    level(0.0),
    envelope(0.0),
    onOff (false),
    tailOff(true)
    {
        carrier.setSamplingRate(getSampleRate());
        modulator.setSamplingRate(getSampleRate());
    }
    
    ~FMVoice() {
        
    }
    
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<FMSound*> (sound) != nullptr;
    }
    
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        // converting MIDI note number into freq
        carrierFrequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        
        // we don't want an ugly sweep when the note begins...
        smooth[0].setSmooth(0);
        smooth[0].tick(carrierFrequency);
        
        // standard smoothing...
        for(int i=0; i<2; i++){
            smooth[i].setSmooth(0.999);
        }
        
        level = velocity;
        // level = pow(velocity,2); // if we want linear dynamic
        
        // tells the note to begin!
        onOff = true;
        
        // These parameters could be controlled with UI elements and could
        // be assigned to specific MIDI controllers. If you do so,
        // don't forget to smooth them!
        modulator.setFrequency(1000.0);
        index = 150;
    }
    
    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        onOff = false; // end the note
        level = 0; // ramp envelope to 0 if tail off is allowed
        
        tailOff = allowTailOff;
    }
    
    void pitchWheelMoved (int /*newValue*/) override
    {
        // Pitch wheel is an important standard MIDI feature that should be implemented
    }
    
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
        // Thats where you would change the value of the modulator index and frequency
        // if you wanted to control them with MIDI controllers
    }
    
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        // only compute block if note is on!
        if(envelope != 0 || onOff){
            while (--numSamples >= 0){
                envelope = smooth[1].tick(level); // here we use a smoother as an envelope generator
                carrier.setFrequency(smooth[0].tick(carrierFrequency) + modulator.tick()*index);
                const float currentSample = (float) carrier.tick() * envelope;
                for (int i = outputBuffer.getNumChannels(); --i >= 0;){
                    outputBuffer.addSample (i, startSample, currentSample);
                }
                ++startSample;
                
                // if tail off is disabled, we end the note right away, otherwise, we wait for envelope
                // to reach a safe value
                if(!onOff && (envelope < 0.001 || !tailOff)){
                    envelope = 0;
                    clearCurrentNote();
                }
            }
        }
    }
    
private:
    Sine carrier, modulator;
    Smooth smooth[2];
    double carrierFrequency, index, level, envelope;
    bool onOff, tailOff;
};

struct SynthAudioSource : public AudioSource
{
    SynthAudioSource (MidiKeyboardState& keyState) : keyboardStateSource (keyState)
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
    
    ~SynthAudioSource() {
        
    }
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        midiCollector.reset (sampleRate);
        synth.setCurrentPlaybackSampleRate (sampleRate);
    }
    
    void releaseResources() override
    {
    }
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // the synth always adds its output to the audio buffer, so we have to clear it
        // first..
        bufferToFill.clearActiveBufferRegion();
        
        // fill a midi buffer with incoming messages from the midi input.
        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);
        
        // pass these messages to the keyboard state so that it can update the component
        // to show on-screen which keys are being pressed on the physical midi keyboard.
        // This call will also add midi messages to the buffer which were generated by
        // the mouse-clicking on the on-screen keyboard.
        keyboardStateSource.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
        
        // and now get the synth to process the midi events and generate its output.
        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    }
    
    // this collects real-time midi messages from the midi input device, and
    // turns them into blocks that we can process in our audio callback
    MidiMessageCollector midiCollector;
    
    // this represents the state of which keys on our on-screen keyboard are held
    // down. When the mouse is clicked on the keyboard component, this object also
    // generates midi messages for this, which we can pass on to our synth.
    MidiKeyboardState& keyboardStateSource;
    
    // the synth itself!
    Synthesiser synth;
};

#endif  // FMSYNTHSOURCE_H_INCLUDED

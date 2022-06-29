/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 10 Feb 2020 2:28:07pm
    Author:  Acer

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer : public AudioSource,
                      public Timer
{
  public:

    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);

    void start();
    // renamed the stop function from lectures to pause to better reflect its function 
    void pause();

    //as stop() functions more like a pause than a stop, I created the below
    /**function for stopping the track and reset the position to starting point*/
    void reset();

    /** get the relative position of the playhead */
    double getPositionRelative();

    /**get the transport source length in seconds*/
    double getLengthInSeconds();

    //////custom functions
    /////**function for looping section of the track (up to 15secs)*/
    void loop(bool toLoop);

    ////pure virtual function of timer
    ///** customised timer Callback function to help implement looping of the track */
    void timerCallback() override;

    /**helper function detecting if track has reched the end*/
    bool reachedTheEnd();

    //helper variable for implementing changes in the playbutton
    bool isPlaying;


private:

    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource; 
    ResamplingAudioSource resampleSource{&transportSource, false, 2};

    //helper variable for implementing the loop function
    bool isLooping;

};





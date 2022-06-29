/*
  ==============================================================================

    DeckGUI.h
    Created: 12 Feb 2022 7:12:45pm
    Author:  Acer

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

//==============================================================================
/*
*/
class DeckGUI    : public Component,
                   public Button::Listener, 
                   public Slider::Listener,
                   public FileDragAndDropTarget, 
                   public Timer
{
public:
    DeckGUI(DJAudioPlayer* player, 
            AudioFormatManager & 	formatManagerToUse,
            AudioThumbnailCache & 	cacheToUse,
            bool differentSkin);
    ~DeckGUI();

    void paint (Graphics&) override;
    void resized() override;

     /** implement Button::Listener */
    void buttonClicked (Button *) override;

    /** implement Slider::Listener */
    void sliderValueChanged (Slider *slider) override;

    bool isInterestedInFileDrag (const StringArray &files) override;
    void filesDropped (const StringArray &files, int x, int y) override; 

    void timerCallback() override;

    /** function to load track to correspondent DeckGUI */
    void loadTrack(URL audioURL, String trackTitle);

private:

    TextButton playButton{"PLAY"};
    TextButton resetButton{"RESET"};
    TextButton loadButton{"LOAD"};
    TextButton loopButton{"LOOP"};
  
    Slider volSlider; 
    Slider speedSlider;
    Slider posSlider;

    WaveformDisplay waveformDisplay1;
    WaveformDisplay waveformDisplay2;
    WaveformDisplay* waveformDisplay;

    DJAudioPlayer* player;

    //label, displaying track title
    Label trackTitleLabel;

    //variable to help customise colour scheme between left and right DeckGUI
    bool differentLayout;

    //variables to help with play, reset and loop buttons functionality
    bool isLooping;

    //variable to store loaded url and help reload it if loop is turned on
    URL url;

    //lookAndFeel variable used to give buttons a neon feel
    LookAndFeel_V2 lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};

/*
  ==============================================================================

    WaveformDisplay.h
    Created: 14 Feb 2022 1:23:43pm
    Author:  Acer

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class WaveformDisplay    : public Component, 
                           public ChangeListener
{
public:
    WaveformDisplay(AudioFormatManager & 	formatManagerToUse,
                    AudioThumbnailCache & 	cacheToUse,
                    bool differentColour);
    ~WaveformDisplay();

    void paint (Graphics&) override;
    void resized() override;

    void changeListenerCallback (ChangeBroadcaster *source) override;

    void loadURL(URL audioURL);

    /** set the relative position of the playhead*/
    void setPositionRelative(double pos);

private:
    AudioThumbnail audioThumb;
    bool fileLoaded; 
    double position;

    Colour colour1;
    Colour colour2;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};

/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 14 Feb 2022 1:23:43pm
    Author:  Acer

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager & 	formatManagerToUse,
                                 AudioThumbnailCache & 	cacheToUse,
                                 bool differentColour
                                 ) :
                                 audioThumb(1000, formatManagerToUse, cacheToUse), 
                                 fileLoaded(false), 
                                 position(0)
                          
{
    audioThumb.addChangeListener(this);

    if (!differentColour){
        colour1 = Colour(0, 255, 127);
        colour2 = Colour(225, 195, 255);
    }
    else{
        colour1 = Colour(255, 20, 147);
        colour2 = Colour(255, 255, 175);
    }
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (Graphics& g)
{
    g.fillAll (Colour(22, 22, 22));  //background colour

    g.setColour (Colour(0, 245, 245)); //outline colour);
    g.drawRect (getLocalBounds(), 1);  //draw an outline around the component

    //g.setColour (colour);
    //get a gradient colour fill:
    ColourGradient gradientColour = ColourGradient(colour1, 0.0, getHeight()/2, colour2, getWidth(), getHeight()/2, false);
    g.setGradientFill(gradientColour);
    if(fileLoaded){
        // adjusted the area and zoom parameters 
        // two channels drawn on top of each other
        audioThumb.drawChannel(g, 
                               Rectangle<int>::Rectangle(3, 0, getWidth() - 6, getHeight()/2 - 2),
                               0, 
                               audioThumb.getTotalLength(), 
                               0, 
                               0.9f // zoomed out a bit
                               );
        audioThumb.drawChannel(g,
                               Rectangle<int>::Rectangle(3, getHeight() / 2, getWidth() - 6, getHeight() / 2 - 2),
                               0,
                               audioThumb.getTotalLength(),
                               0,
                               0.9f // zoomed out a bit
                               );
      g.setColour(Colours::darkviolet);
      g.drawRect(1.5 + (position * (getWidth() - 4.5)), 1, 2, getHeight() - 1);
    }
    else{
      g.setFont (20.0f);
      g.drawText ("File not loaded...", getLocalBounds(),
                  Justification::centred, true);   // draw some placeholder text

    }
}

void WaveformDisplay::resized()
{

}

void WaveformDisplay::loadURL(URL audioURL)
{
  audioThumb.clear();
  fileLoaded  = audioThumb.setSource(new URLInputSource(audioURL));
  if (fileLoaded)
  {
    repaint();
  }
  else {
    std::cout << "wfd: not loaded! " << std::endl;
  }
}

void WaveformDisplay::changeListenerCallback (ChangeBroadcaster *source)
{
    repaint();
}

void WaveformDisplay::setPositionRelative(double pos)
{
  if (pos != position)
  {
    position = pos;
    repaint();
  }
}





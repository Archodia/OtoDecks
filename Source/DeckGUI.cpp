/*
  ==============================================================================

    DeckGUI.cpp
    Created: 12 Feb 2022 7:12:45pm
    Author:  Acer

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                AudioFormatManager& formatManagerToUse,
                AudioThumbnailCache& cacheToUse,
                bool differentSkin
                ) : 
                player(_player),
                waveformDisplay1(formatManagerToUse, cacheToUse, false),
                waveformDisplay2(formatManagerToUse, cacheToUse, true),
                isLooping(false)
{
    //add various components and make them visible
    //buttons
    addAndMakeVisible(playButton);
    addAndMakeVisible(resetButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(loopButton);
    //sliders  
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    //label for track titles
    addAndMakeVisible(trackTitleLabel);
    //waveform display - differenet version per differentSkin parameter
    //will help customise colour scheme per deckGUI
    if (differentSkin){
        waveformDisplay = &waveformDisplay2;
    }
    else{
        waveformDisplay = &waveformDisplay1;
    }
    addAndMakeVisible(waveformDisplay);
   
    //add listeners for buttons and sliders
    playButton.addListener(this);
    resetButton.addListener(this);
    loadButton.addListener(this);
    loopButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    
    //turn loop button into toggle one - able to change state automatically when clicked on
    loopButton.setClickingTogglesState(true);

    //customisation for trackTitle label
    trackTitleLabel.setFont(18.0f);
    trackTitleLabel.setText(" * Track Title * (not available) ", sendNotification);
    
    //customisation for vol slider
    volSlider.setSliderStyle(Slider::LinearBarVertical);
    volSlider.setTextBoxStyle(Slider::NoTextBox, false, 20, 0);
    volSlider.setPopupDisplayEnabled(true, true, this);
    volSlider.setTextValueSuffix("  Vol");
    volSlider.setRange(0.0, 1.0, 0.01);
    volSlider.setValue(0.5);

    //customisation for speed slider
    speedSlider.setSliderStyle(Slider::Rotary);
    speedSlider.setTextBoxStyle(Slider::NoTextBox, false, 20, 0);
    speedSlider.setPopupDisplayEnabled(true, true, this);
    speedSlider.setTextValueSuffix("  Speed");
    speedSlider.setRange(0.0, 2.0, 0.1);
    speedSlider.setValue(1.0);
    
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 20, 0);
    posSlider.setRange(0.0, 1.0);
    posSlider.setValue(0.0);

    // passing the differentSkin value to differentLayout
    // to use inside resized()/paint() functions; they do not have access to differentSkin
    differentLayout = differentSkin;

    //start timer with interval length of 500 milliseconds
    startTimer(500);

}

DeckGUI::~DeckGUI()
{
    stopTimer(); //stop timer callbacks after deckGUI gets destroyed
}

void DeckGUI::paint(Graphics& g)
{
    g.fillAll(Colour(22, 22, 22));//background colour

    g.setColour(Colour(0, 245, 245)); //outline colour
    g.drawRect(getLocalBounds(), 1); //draw an outline around the component

    //volSlider customisations
    volSlider.setColour(Slider::backgroundColourId, Colour(32, 32, 32));
    // I did not find a way to change this colour with LookAndFeel between the two DeckGUIs
    // so I did it manually by using the differentSkin parameter in DeckGUI object
    if (!differentLayout) {
        volSlider.setColour(Slider::trackColourId, Colour(0, 255, 127));
    }
    else {
        volSlider.setColour(Slider::trackColourId, Colour(255, 20, 147));
    }

    //speedSlider customisations
    // same customisation logic as with vol slider:
    if (!differentLayout) {
        speedSlider.setColour(Slider::rotarySliderFillColourId, Colour(0, 255, 127));
        speedSlider.setColour(Slider::rotarySliderOutlineColourId, Colour(229, 204, 255));
    }
    else {
        speedSlider.setColour(Slider::rotarySliderFillColourId, Colour(255, 20, 147));
    }

    //customisation for pos slider
    if (!differentLayout) {
        posSlider.setColour(Slider::trackColourId, Colour(100, 255, 178));
        posSlider.setColour(Slider::backgroundColourId, Colour(229, 204, 255));
    }

    //customisation for buttons:
 
    playButton.setColour(TextButton::buttonColourId, Colour(27, 255, 255));
    if (player->isPlaying) {
        playButton.setColour(TextButton::buttonColourId, Colour(255, 255, 102));
    }
   
    resetButton.setColour(TextButton::buttonColourId, Colour(226, 107, 255));
    
    loadButton.setColour(TextButton::buttonColourId, Colour(64, 64, 64));
    loadButton.setColour(TextButton::textColourOffId, Colours::white);

    // when loopButton is off:
    loopButton.setColour(TextButton::buttonColourId, Colour(64, 64, 64));
    loopButton.setColour(TextButton::textColourOffId, Colours::white);
    // when loopButton is on:
    if (!differentLayout) {
        loopButton.setColour(TextButton::buttonOnColourId, Colour(0, 255, 127)); //Left DeckDUI
    }
    else {
        loopButton.setColour(TextButton::buttonOnColourId, Colour(255, 20, 147)); //Right DeckGUI
    }
    loopButton.setColour(TextButton::textColourOnId, Colours::black);
    
    //adding lookAndFeel_V2 style on buttons
    playButton.setLookAndFeel(&lookAndFeel);
    resetButton.setLookAndFeel(&lookAndFeel);
    loadButton.setLookAndFeel(&lookAndFeel);
    loopButton.setLookAndFeel(&lookAndFeel);

}

void DeckGUI::resized()
{
    //helper variables
    double rowH = getHeight() / static_cast<double>(10); 
    double widthR = getWidth();

    //setting relevant placement and resizing proportions of the elements
    trackTitleLabel.setBounds(0, 0, widthR, rowH);
    waveformDisplay->setBounds(0, rowH, widthR, rowH*2.5);
    posSlider.setBounds(-3, rowH*3.5, widthR + 6, rowH/2.3);
    playButton.setBounds(widthR/2 - (widthR/8 + 6), rowH * 8.5, widthR/8, rowH);
    resetButton.setBounds(widthR/2 + 6, rowH * 8.5, widthR/8, rowH);

    //below elements to be placed with a mirror effect in the two DeckGUIs
    if (!differentLayout){
        volSlider.setBounds(widthR * 17.5/20, rowH * 4.5, widthR/20, rowH * 5);
        speedSlider.setBounds(widthR * 0.1/4, rowH * 5.5, widthR/5, rowH * 2.8);  
        loopButton.setBounds(widthR * 0.26 / 4, rowH * 4.2, widthR / 8, rowH);
        loadButton.setBounds(widthR * 0.26/4, rowH * 8.5, widthR/8, rowH);
    }
    else{
        volSlider.setBounds(widthR* 1.5/20, rowH * 4.5, widthR/20, rowH * 5);
        speedSlider.setBounds(widthR - (widthR/5 + widthR * 0.2/4), rowH * 5.5, widthR/5, rowH * 2.8);
        loopButton.setBounds(widthR - (widthR / 8 + widthR * 0.37 / 4), rowH * 4.2, widthR / 8, rowH);
        loadButton.setBounds(widthR - (widthR/8 + widthR * 0.37/4), rowH * 8.5, widthR/8, rowH);
    }
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton) {
        if (player->isPlaying) {
            player->pause();
            playButton.setButtonText("PLAY");
        }
        else {
            player->start();
            playButton.setButtonText("PAUSE");
        }
    }
    if (button == &resetButton) {
        player->reset();
        playButton.setButtonText("PLAY");
    }
    if (button == &loopButton) {
        if (loopButton.getToggleState()) {
            player->loop(true);
            isLooping = true;
        }
        else {
            player->loop(false);
            isLooping = false;
        }
    }
    if (button == &loadButton){
        FileChooser chooser{"Select a file..."};
        if (chooser.browseForFileToOpen()){
            String trackTitle = chooser.getResult().getFileNameWithoutExtension();
            url = URL{ chooser.getResult() };
            player->loadURL(url);
            waveformDisplay->loadURL(url);
            trackTitleLabel.setText(trackTitle, sendNotification);// add track title in the label
        }
    }
}

void DeckGUI::sliderValueChanged (Slider *slider)
{
    if (slider == &volSlider){
        player->setGain(slider->getValue());
    }

    if (slider == &speedSlider){
        player->setSpeed(slider->getValue());
    }
    
    if (slider == &posSlider){
         player->setPositionRelative(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
{
  std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
  return true; 
}

void DeckGUI::filesDropped (const StringArray &files, int x, int y)
{
  std::cout << "DeckGUI::filesDropped" << std::endl;
  if (files.size() == 1){
    player->loadURL(URL{File{files[0]}});
  }
}

void DeckGUI::loadTrack(URL audioURL, String trackTitle)
{
    url = audioURL;
    player->loadURL(url);
    waveformDisplay->loadURL(url);
    trackTitleLabel.setText(trackTitle, sendNotification); // also add track title in label 
}

void DeckGUI::timerCallback()
{
    double relativePos = player->getPositionRelative();
    
    waveformDisplay->setPositionRelative(relativePos);

    if (!isnan(relativePos)) { //if track has loaded only
        posSlider.setValue(relativePos);
    }

    if (isLooping && player->reachedTheEnd()) {
        player->loadURL(url);
        player->start();
    }
}

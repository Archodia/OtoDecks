/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }  
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }  

    addAndMakeVisible(deckGUI1); 
    addAndMakeVisible(deckGUI2);  

    addAndMakeVisible(playlistComponent);

    formatManager.registerBasicFormats();

    //setting different colour scheme for deckGUI2 posSlider and rotary slider
    otherLookAndFeel.setColour(Slider::thumbColourId, Colour(153, 50, 153));
    otherLookAndFeel.setColour(Slider::trackColourId, Colour(255, 50, 160));
    otherLookAndFeel.setColour(Slider::backgroundColourId, Colour(255, 255, 153));
    otherLookAndFeel.setColour(Slider::rotarySliderOutlineColourId, Colour(255, 255, 175));
    deckGUI2.setLookAndFeel(&otherLookAndFeel);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);

 }
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
   
}

void MainComponent::resized()
{
    deckGUI1.setBounds(0, 0, getWidth()/2, getHeight()*3/5);
    deckGUI2.setBounds(getWidth()/2, 0, getWidth()/2, getHeight()*3/5);
    playlistComponent.setBounds(0, getHeight()*3/5, getWidth(), getHeight()*2/5);

}


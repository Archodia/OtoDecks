/*
==============================================================================

    DJAudioPlayer.cpp
    Created: 10 Feb 2020 2:28:07pm
    Author:  Acer

==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager) 
                            : formatManager(_formatManager),
                              isLooping(false),
                              isPlaying(false)
{
  
}
DJAudioPlayer::~DJAudioPlayer()
{

}

void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void DJAudioPlayer::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);

}
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {       
        std::unique_ptr<AudioFormatReaderSource> newSource (
                    new AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());          
    }
}
void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 1.0)
    {
        std::cout << "DJAudioPlayer::setGain gain should be between 0 and 1" << std::endl;
    }
    else {
        transportSource.setGain(gain);
    }
   
}
void DJAudioPlayer::setSpeed(double ratio)
{
  if (ratio < 0 || ratio > 2.0)
    {
        std::cout << "DJAudioPlayer::setSpeed ratio should be between 0 and 2" << std::endl;
    }
    else {
        resampleSource.setResamplingRatio(ratio);
    }
}
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
     if (pos < 0 || pos > 1.0)
    {
        std::cout << "DJAudioPlayer::setPositionRelative pos should be between 0 and 1" << std::endl;
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start()
{
    transportSource.start();
    isPlaying = true;
}
void DJAudioPlayer::pause()
{
    transportSource.stop();
    isPlaying = false;
}

void DJAudioPlayer::reset()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
    isPlaying = false;
}

void DJAudioPlayer::loop(bool toLoop)
{
    isLooping = toLoop;
}

void DJAudioPlayer::timerCallback()
{
    if (isLooping && reachedTheEnd()){
       
        transportSource.setPosition(0.0);
    }
}

double DJAudioPlayer::getLengthInSeconds()
{
    return transportSource.getLengthInSeconds();
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

bool DJAudioPlayer::reachedTheEnd()
{
    return transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 1.0;
}
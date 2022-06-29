/*
  ==============================================================================

    PlaylistComponent.h
    Created: 18 Feb 2022 3:38:11pm
    Author:  Acer

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DeckGUI.h"
#include "DJAudioPlayer.h"
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>


//==============================================================================
/*
*/
class PlaylistComponent  : public Component,
                           public TableListBoxModel,
                           public Button::Listener,
                           public TextEditor::Listener,
                           public FileDragAndDropTarget
{
public:
    /**PlayListComponent constructor*/
    PlaylistComponent(DeckGUI* deck1,
                      DeckGUI* deck2,
                      DJAudioPlayer* metadataplayer);
    /**PlayListComponent destructor*/
    ~PlaylistComponent() override;

    /** Drawing function for customising PlayListComponent*/
    void paint (juce::Graphics&) override;
    /** function for rescaling each component when window is resized*/
    void resized() override;

    //TableListBoxModel pure virtual functions:
    /**Returns number of table's rows*/
    int  getNumRows() override;
    /**Paints table's rows background*/
    void paintRowBackground(Graphics&,
                            int rowNumber,
                            int width,
                            int height,
                            bool rowIsSelected) override;
    /**Paints table's cells: used to input info in cells*/
    void paintCell(Graphics&,
                    int rowNumber,
                    int columnId,
                    int width,
                    int height,
                    bool rowIsSelected) override;

    /**Creates and tracks customised components for certain cells; used to add buttons */
    Component* refreshComponentForCell(int 	rowNumber,
                                       int 	columnId,
                                       bool 	isRowSelected,
                                       Component* existingComponentToUpdate) override;

    //Button::Listener pure virtual function:
    /**Called to take certain actions when a PlaylistComponent button is clicked*/
    void buttonClicked(Button* button) override;

    //FileDragAndDropTarget pure virtual functions:
    /**Function that allows files to be dragged and dropped in PlaylistComponent*/
    bool isInterestedInFileDrag(const StringArray& files) override;
    /**Function that indicates that files are dropped in PlaylistCommponent and processes them*/
    void filesDropped(const StringArray& files, int x, int y) override;

private:

    //a juce files vector which will hold all files loaded or dropped in the playlist
    std::vector<juce::File> tracks;
    //a secondary juce files vector holding all files loaded/dropped, but updated when searchBar has an input
    std::vector<juce::File> filteredTracks;
    
    //our playlist component, displayed as a TableListBox
    TableListBox tableComponent;

    //label, displaying a message over the the TableListBox
    Label playlistLabel;

    //text button used for loading tracks in the playlist
    TextButton loadButton{ "ADD TRACKS" };

    //text button used for clearing all tracks from the playlist
    TextButton clearAllButton{ "CLEAR ALL" };

    //search bar implemented as TextEditor
    TextEditor searchBar;

    DeckGUI* deck1;
    DeckGUI* deck2;
    DJAudioPlayer* player;

    LookAndFeel_V2 lookAndFeel;

    /**function that gets seconds (double) and turns it to string of mm:ss format*/
    String formatLength(double seconds);
    /**function that gets the track length in seconds and turns it to string of mm:ss format*/
    String getTrackLength(URL audioURL);
    /** function that filters tracks on playlist by searchBar input */
    void filterPlaylist(String input);
    /**function that loads the Playlist that was saved upon closure of app*/
    void loadPlaylist();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};

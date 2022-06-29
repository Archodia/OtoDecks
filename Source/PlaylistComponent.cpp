/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 18 Feb 2022 3:38:11pm
    Author:  Acer

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(DeckGUI* deck1,
                                     DeckGUI* deck2,
                                     DJAudioPlayer* player
                                     ): deck1(deck1),
                                        deck2(deck2),
                                        player(player)
{
    // Create a table that will act as a Music Library
    // Create columns and set their headers
    tableComponent.getHeader().addColumn("#", 1, 1);
    tableComponent.getHeader().addColumn("Length", 2, 1);
    tableComponent.getHeader().addColumn("Track title", 3, 1);
    tableComponent.getHeader().addColumn("Add to...", 4, 1);
    tableComponent.getHeader().addColumn("Add to...", 5, 1);
    tableComponent.getHeader().addColumn("Delete", 6, 1);
    tableComponent.setModel(this);

    //make table, loadButton, clearAllButton, searchBar and playlistLabel visible
    addAndMakeVisible(tableComponent);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(clearAllButton);
    addAndMakeVisible(searchBar);
    addAndMakeVisible(playlistLabel);

    //add text in search bar (can type in) and playlistLabel (not editable)
    searchBar.setFont(19.0f);
    searchBar.setTextToShowWhenEmpty(" Type and press enter to search...  ", Colours::white);
    //add text in playlistLabel (not editable) - centred
    playlistLabel.setFont(18.0f);
    playlistLabel.setText(" CREATE PLAYLIST ", dontSendNotification);
    playlistLabel.setJustificationType(Justification::centred);

    //Register listeners to loadButton and searchBar
    loadButton.addListener(this);
    clearAllButton.addListener(this);
    searchBar.addListener(this);

    //if there is a playlist saved, load it
    loadPlaylist();

    searchBar.onTextChange = [this] {filterPlaylist(searchBar.getText());};

}

PlaylistComponent::~PlaylistComponent()
{
    //when the app is closed, the playlist is automatically saved
    std::ofstream savedPlaylist("playlist.txt");

    // playlist is saved to file, by saving each tracks fullpath names
    for (File& t : tracks){
        savedPlaylist << t.getFullPathName() << "\n";
    }
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    //background colour (only visible behind loadButton and clearAllButton)
    //matching label and searchbar background colours
    g.fillAll(Colour(32, 32, 32));

    //customise table's colours:
    //table background - matching added rows colours (when not selected)
    tableComponent.setColour(TableListBox::backgroundColourId, Colour(40,40,40));
    //customise headers' colours:
    tableComponent.getHeader().setColour(TableHeaderComponent::backgroundColourId, Colour(32,32,32));
    tableComponent.getHeader().setColour(TableHeaderComponent::textColourId, Colours::white);

    //customise playlist label's background and font:
    playlistLabel.setColour(Label::backgroundColourId, Colour(12,12,12));
    
    //customise searchbar text editor's background and font:
    searchBar.setColour(TextEditor::backgroundColourId, Colour(32, 32, 32));
    searchBar.setFont(18.0f);

    //customise loadButton's background:
    loadButton.setColour(TextButton::buttonColourId, Colour(12, 12, 12));
    //customise loadButton's text colour:
    loadButton.setColour(TextButton::textColourOffId, Colours::white);

    //customise clearAllButton's background:
    clearAllButton.setColour(TextButton::buttonColourId, Colour(12, 12, 12));
    //customise clearAllButton's text colour:    
    clearAllButton.setColour(TextButton::textColourOffId, Colour(255, 64, 64));

    //adding lookAndFeel_V2 style on buttons
    loadButton.setLookAndFeel(&lookAndFeel);
    clearAllButton.setLookAndFeel(&lookAndFeel);
}

void PlaylistComponent::resized()
{
    //Divide the PlaylistComponent area to 8 "columns" and 10 "rows" (variables created)
    //and place the table, loadButton and searchBar in relative positions

    double rowH = getHeight() / static_cast<double>(10);
    float columnW = getWidth() / static_cast <float>(8);
    float tableColW = getWidth() / static_cast <float>(24);

    playlistLabel.setBounds(0, 0, columnW * 8, rowH);
    tableComponent.setBounds(0, rowH, columnW * 8, rowH * 8);
    searchBar.setBounds(0, rowH * 9, columnW * 6, rowH);
    loadButton.setBounds(columnW * 6, rowH * 9, columnW, rowH);
    clearAllButton.setBounds(columnW * 7, rowH * 9, columnW, rowH);

    tableComponent.getHeader().setColumnWidth(1, tableColW);
    tableComponent.getHeader().setColumnWidth(2, tableColW*3);
    tableComponent.getHeader().setColumnWidth(3, tableColW*14);
    tableComponent.getHeader().setColumnWidth(4, tableColW*2);
    tableComponent.getHeader().setColumnWidth(5, tableColW*2);
    tableComponent.getHeader().setColumnWidth(6, tableColW*2);

    tableComponent.setRowHeight(getHeight() / 10);

}

int  PlaylistComponent::getNumRows()
{
    if (searchBar.isEmpty()){
        return tracks.size();
    }
    else{
        return filteredTracks.size();
    }
}

void PlaylistComponent::paintRowBackground(Graphics& g,
                                           int rowNumber,
                                           int width,
                                           int height,
                                           bool rowIsSelected)
{
    if (rowIsSelected){
        g.fillAll(Colour(65, 65, 65));
    }
    else{
        g.fillAll(Colour(45, 45, 45));
    }
}

void PlaylistComponent::paintCell(Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    g.setFont(16.0f);
    if (rowNumber < getNumRows()) // added to fix the bug when resizing the window
    {
        g.setColour(Colours::white);
        if (columnId == 1) {
            g.drawText(std::to_string(rowNumber + 1),
                2, 0,
                width - 4, height,
                Justification::centred,
                true);
            return;
        }
        if (columnId == 2) {
            URL trackURL;
            if (searchBar.isEmpty()) {
                trackURL = URL{ tracks[rowNumber] };
            }
            else {
                trackURL = URL{ filteredTracks[rowNumber] };
            }
            String trackLength = getTrackLength(trackURL);
            g.drawText(trackLength.toStdString(),
                2, 0,
                width - 4, height,
                Justification::centredLeft,
                true);
            return;
        }
        if (columnId == 3) {
            String trackTitle;
            if (searchBar.isEmpty()) {
                trackTitle = tracks[rowNumber].getFileNameWithoutExtension();
            }
            else {
                trackTitle = filteredTracks[rowNumber].getFileNameWithoutExtension();
            }
            g.drawText(trackTitle.toStdString(),
                2, 0,
                width - 4, height,
                Justification::centredLeft,
                true);
            return;
        }
        g.setColour(Colour(60, 60, 60));
        g.drawRect(getLocalBounds(), 1);
    }
}

Component* PlaylistComponent::refreshComponentForCell(int 	rowNumber,
                                                      int 	columnId,
                                                      bool 	isRowSelected,
                                                      Component* existingComponentToUpdate)
{
    //Inspired by the last topic's lectures' code, create buttons in columns 6,7 and 8 
    //for loading tracks to the two decks and deleting tracks from the list  
    //unique ids are also created for each button for the ButtonListener torecognise which button is clicked
    
    if (columnId == 4){
        if (existingComponentToUpdate == nullptr){

            TextButton* btn = new TextButton{"L Deck"};
            String id{std::to_string(rowNumber)};
            btn->setComponentID(id);

            btn->addListener(this);
            existingComponentToUpdate = btn;

            btn->setColour(TextButton::buttonColourId, Colours::springgreen);
            btn->setColour(TextButton::textColourOffId, Colours::black);
            btn->setLookAndFeel(&lookAndFeel);
            
        }
    }
    if (columnId == 5){
        if (existingComponentToUpdate == nullptr){

            TextButton* btn = new TextButton{"R Deck"};
            String id{std::to_string(rowNumber + 1000)};
            btn->setComponentID(id);

            btn->addListener(this);
            existingComponentToUpdate = btn;

            btn->setColour(TextButton::buttonColourId, Colours::deeppink);
            btn->setColour(TextButton::textColourOffId, Colours::black);
            btn->setLookAndFeel(&lookAndFeel);
        }
    }
    if (columnId == 6){
        if (existingComponentToUpdate == nullptr){

            TextButton* btn = new TextButton{"X"};
            String id{std::to_string(rowNumber + 1000000)};
            btn->setComponentID(id);

            btn->addListener(this);
            existingComponentToUpdate = btn;

            btn->setColour(TextButton::buttonColourId, Colour(229, 204, 255));
            btn->setColour(TextButton::textColourOffId, Colour(220, 0, 0));
            btn->setLookAndFeel(&lookAndFeel);
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(Button* button)
{
    if (button == &loadButton){

        FileChooser chooser{"Select a file..."};
        if (chooser.browseForMultipleFilesToOpen()){
            for (const File& file : chooser.getResults()){

                File newTrack{file};
                String newTrackTitle = newTrack.getFileNameWithoutExtension();
                // if not already loaded in Playlist:
                if (!(std::find(tracks.begin(), tracks.end(), file) != tracks.end())){
                    
                    tracks.push_back(newTrack);
                }
                else{ // display error message
                    juce::AlertWindow::showMessageBox(AlertWindow::AlertIconType::InfoIcon,
                                                      "Action Not Completed - Reason:",
                                                      newTrackTitle + " is already loaded!");
                }
            }
        }
        tableComponent.updateContent();
    }
    else if (button == &clearAllButton){
        if (tracks.size() > 0){
            tracks.clear();
        }
        tableComponent.updateContent();
    }
    else{
        int id = std::stoi(button->getComponentID().toStdString());
        if (id < 1000){ //Add to L Deck button
            if (searchBar.isEmpty()){
                URL trackURL = URL{ tracks[id] };
                String trackTitle = tracks[id].getFileNameWithoutExtension();
                deck1->loadTrack(trackURL, trackTitle);
            }
            else{
                URL trackURL = URL{ filteredTracks[id] };
                String trackTitle = filteredTracks[id].getFileNameWithoutExtension();
                deck1->loadTrack(trackURL, trackTitle);
            }
            
        }
        else if (id >= 1000 && id < 1000000){ //Add to R Deck button
            if (searchBar.isEmpty()){
                URL trackURL = URL{ tracks[id - 1000]};
                String trackTitle = tracks[id - 1000].getFileNameWithoutExtension();
                deck2->loadTrack(trackURL, trackTitle);
            }
            else{
                URL trackURL = URL{ filteredTracks[id - 1000] };
                String trackTitle = filteredTracks[id - 1000].getFileNameWithoutExtension();
                deck2->loadTrack(trackURL, trackTitle);
            }
        }        
        else{ //Delete track button
            if (searchBar.isEmpty()){
                tracks.erase(tracks.begin() + (id - 1000000));
            }
            else{ // if playlist is filtered by search function
                int index = (id - 1000000);
                for (int i = 0; i < tracks.size(); ++i)//loops through tracks vector
                {
                    if (tracks[i] == filteredTracks[index]) //locates selected track for deletion
                    {
                        tracks.erase(tracks.begin() + i);// deletes track in (original) tracks vector
                    }
                }
                filteredTracks.erase(filteredTracks.begin() + index); // deletes selected track in filteredTracks vector
            }
            tableComponent.updateContent();
        }
    }
    
}

bool PlaylistComponent::isInterestedInFileDrag(const StringArray& files)
{
    return true; //to be able to drag and drop files
}

void PlaylistComponent::filesDropped(const StringArray& files, int x, int y)
{
    if (files.size() >= 1){
        for (int i = 0; i < files.size(); ++i){

            File newTrack{ files[i] };
            String newTrackTitle = newTrack.getFileNameWithoutExtension();

            // if not already loaded in Playlist, add it 
            if (!(std::find(tracks.begin(), tracks.end(), files[i]) != tracks.end())){
                
                tracks.push_back(newTrack);
            }
            else{   // display error message
                juce::AlertWindow::showMessageBox(AlertWindow::AlertIconType::InfoIcon,
                                                  "Action Not Completed - Reason:",
                                                  newTrackTitle + " is already loaded!");
            }
        }
    }
    tableComponent.updateContent();
}

String PlaylistComponent::formatLength(double _secs)
{
    //get track length in seconds and turn it to string of mm:ss format
    int seconds {int(std::round(_secs))};
    String mins{std::to_string(seconds / 60)};
    String secs{std::to_string(seconds % 60)};

    //if seconds is zero or one-digit long, turn to double-digit format
    // e.g. 0 -> 00 , 6 ->06
    while (secs.length() < 2){
        secs = '0'+ secs;
    }
    //if minutes is zero or one-digit long, turn to double-digit format
    // e.g. 0 -> 00 , 6 ->06
    while (mins.length() < 2){
        mins = '0' + mins;
    }

    return String{ mins + ":" + secs };
}

String PlaylistComponent::getTrackLength(juce::URL audioURL)
{
    player -> loadURL(audioURL);

    //get track length in seconds(double) using DJAudioPlayer object "player"
    double secs {player -> getLengthInSeconds()};

    //turn length to string format of 
    String mins {formatLength(secs)};

    return mins;
}

void PlaylistComponent::loadPlaylist()
{
    // create input stream from the saved playlist
    std::ifstream playlist("playlist.txt");
    std::string filePath;

    // read saved file, line by line
    if (playlist.is_open()){
        while (getline(playlist, filePath)) {

            File newTrack{filePath};
            tracks.push_back(newTrack);
        }
    }
    playlist.close();
}

void PlaylistComponent::filterPlaylist(String input)
{
    filteredTracks.clear();

    for (File& t : tracks){
        if (t.getFileNameWithoutExtension().containsIgnoreCase(input)){
            if (!(std::find(filteredTracks.begin(), filteredTracks.end(), input) != filteredTracks.end())){
                
                filteredTracks.push_back(t);
            }
        }
    }
    tableComponent.updateContent();
    tableComponent.repaint();
}
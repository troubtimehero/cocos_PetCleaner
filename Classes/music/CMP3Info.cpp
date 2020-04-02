#include <fstream>
#include <stdio.h>
using namespace std;

#include "CMP3Info.h"

/* ----------------------------------------------------------
   CMP3Info class is your complete guide to the 
   MP3 file format in the C++ language. It's a large class
   with three different, quite large "sub-classes" so it's
   a fair amount of code to look into.

   This code will be well commented, so that everyone can
   understand, as it's made for the public and not for
   private use, although private use is allowed. :)

   all functions specified both in the header and .cpp file
   will have explanations in both locations.

   everything here by: Gustav "Grim Reaper" Munkby
                       http://home.swipnet.se/grd/
                       grd@swipnet.se
   ---------------------------------------------------------- */

#define ERR_FILEOPEN    0x0001
#define ERR_NOSUCHFILE  0x0002
#define ERR_NOMP3FILE   0x0004
#define ERR_ID3TAG      0x0008

int CMP3Info::loadInfo( const char* srcMP3 ) {
    
    // open input-file stream to the specified file, name
    ifstream* ifile = new ifstream(srcMP3, ios::in | ios::binary | ios::_Nocreate);
    
    if (ifile) { // if the file was opened correctly

        // get file size, by setting the pointer in the end and tell the position
        ifile->seekg(0,ios::end);
        fileSize = ifile->tellg();

        // get srcMP3 into fileName variable
        strcpy(fileName,srcMP3);

        int pos = 0; // current position in file...


        /******************************************************/
        /* search and load the first frame-header in the file */
        /******************************************************/
        
        char headerchars[4]; // char variable used for header-loading

        do {
            // if no header has been found after 200kB
            // or the end of the file has been reached
            // then there's probably no mp3-file
            if ( pos>(1024*200) || ifile->eof() ) {
                showErrorMessage(ERR_NOMP3FILE, srcMP3);
                ifile->close();
                delete ifile;
                return ERR_NOMP3FILE;
            }

            // read in four characters
            ifile->seekg(pos);
            ifile->read (headerchars, 4);

            // move file-position forward
            pos++;
            
            // convert four chars to CFrameHeader structure
            header.loadHeader(headerchars);

        }
        while ( !header.isValidHeader() );  // test for correct header

        // to correct the position to be right after the frame-header
        // we'll need to add another 3 to the current position
        pos += 3;


        /******************************************************/
        /* check for an vbr-header, to ensure the info from a */
        /* vbr-mp3 is correct                                 */
        /******************************************************/

        char vbrchars[12];
        
        // determine offset from first frame-header
        // it depends on two things, the mpeg-version
        // and the mode(stereo/mono)

        if( header.getVersionIndex()==3 ) {  // mpeg version 1

            if( header.getModeIndex()==3 ) pos += 17; // Single Channel
            else                           pos += 32;

        } else {                             // mpeg version 2 or 2.5

            if( header.getModeIndex()==3 ) pos +=  9; // Single Channel
            else                           pos += 17;

        }

        // read next twelve bits in
        ifile->seekg(pos);
        ifile->read (vbrchars, 12);

        // turn 12 chars into a CVBitRate class structure
        VBitRate = vbr.loadHeader(vbrchars);        


        /******************************************************/
        /* get tag from the last 128 bytes in an .mp3-file    */
        /******************************************************/
        
        char tagchars[128]; // load tag as string

        // get last 128 bytes
        ifile->seekg ( -128, ios::end );
        ifile->read ( tagchars, 128 );

        // turn 128 chars into a CId3Tag structure
        Tagged = tag.loadTag(tagchars);

    }
    else {
        showErrorMessage(ERR_NOSUCHFILE, srcMP3);
        ifile->close();
        delete ifile;
        return ERR_NOSUCHFILE;
    }

    ifile->close();
    delete ifile;
    return 0;

}

int CMP3Info::saveTag( char* title, char* artist, char* album, char* year, char* comment, char* tracknumber, char* genre) {

    int error = 0;

    if (strcmp(title,"n/a"))       tag.setTitle(title);
    if (strcmp(artist,"n/a"))      tag.setArtist(artist);
    if (strcmp(album,"n/a"))       tag.setAlbum(album);
    if (strcmp(year,"n/a"))        tag.setYear(atoi(year));
    if (strcmp(comment,"n/a"))     tag.setComment(comment);
    if (strcmp(tracknumber,"n/a")) tag.setTrackNumber(atoi(tracknumber));
    if (strcmp(genre,"n/a"))       tag.genreString(genre, false);

    char id3tagstring[128] ={""};

    if ( tag.saveTag(id3tagstring) ) {

        ofstream* ofile = new ofstream(fileName, ios::in | ios::out | ios::binary);

        if (ofile) {

            // variable to ensure correct, file, position
            int posalter = (Tagged)?-128:0;
            
            // seek up the correct location
            ofile->seekp(posalter, ios::end);
        
            // write the tag to the file
            ofile->write(id3tagstring, 128);

        } else error = ERR_FILEOPEN;

        ofile->close();

        delete ofile;

    } else error = ERR_ID3TAG;

    if (!error) showErrorMessage(error);
    return error;

}




int CMP3Info::getBitrate() {

    if (VBitRate) {

        // get average frame size by deviding fileSize by the number of frames
        float medFrameSize = (float)fileSize / (float)getNumberOfFrames();
        
        /* Now using the formula for FrameSizes which looks different,
           depending on which mpeg version we're using, for mpeg v1:
        
           FrameSize = 12 * BitRate / SampleRate + Padding (if there is padding)

           for mpeg v2 the same thing is:

           FrameSize = 144 * BitRate / SampleRate + Padding (if there is padding)

           remember that bitrate is in kbps and sample rate in Hz, so we need to
           multiply our BitRate with 1000.

           For our purpose, just getting the average frame size, will make the
           padding obsolete, so our formula looks like:

           FrameSize = (mpeg1?12:144) * 1000 * BitRate / SampleRate;
        */

        return (int)( 
                     ( medFrameSize * (float)header.getFrequency() ) / 
                     ( 1000.0 * ( (header.getLayerIndex()==3) ? 12.0 : 144.0))
                    );

    }
    else return header.getBitrate();

}

int CMP3Info::getLengthInSeconds() {

    // kiloBitFileSize to match kiloBitPerSecond in bitrate...
    int kiloBitFileSize = (8 * fileSize) / 1000;
    
    return (int)(kiloBitFileSize/getBitrate());

}

void CMP3Info::getFormattedLength(char* input) {

    //  s  = complete number of seconds
    int s  = getLengthInSeconds();

    //  ss = seconds to display
    int ss = s%60;

    //  m  = complete number of minutes
    int m  = (s-ss)/60;

    //  mm = minutes to display
    int mm = m%60;

    //  h  = complete number of hours
    int h = (m-mm)/60;

    char szTime[16]; // temporary string

    // make a "hh:mm:ss" if there is any hours, otherwise
    // make it   "mm:ss"
    if (h>0) sprintf(szTime,"%02d:%02d:%02d", h,mm,ss);
    else     sprintf(szTime,     "%02d:%02d",   mm,ss);

    // copy to the inputstring
    strcpy(input, szTime);

}

int CMP3Info::getNumberOfFrames() {

    if (!VBitRate) {

        /* Now using the formula for FrameSizes which looks different,
           depending on which mpeg version we're using, for layer 1:
        
           FrameSize = 12 * BitRate / SampleRate + Padding (if there is padding)

           for layer 2 & 3 the same thing is:

           FrameSize = 144 * BitRate / SampleRate + Padding (if there is padding)

           remember that bitrate is in kbps and sample rate in Hz, so we need to
           multiply our BitRate with 1000.

           For our purpose, just getting the average frame size, will make the
           padding obsolete, so our formula looks like:

           FrameSize = (layer1?12:144) * 1000 * BitRate / SampleRate;
        */
           
        float medFrameSize = (float)( 
                                     ( (header.getLayerIndex()==3) ? 12 : 144 ) *
                                     (
                                      (1000.0 * (float)header.getBitrate() ) /
                                      (float)header.getFrequency()
                                     )
                                    );
        
        return (int)(fileSize/medFrameSize);

    }
    else return vbr.getNumberOfFrames();

}

void CMP3Info::getVersion(char* input) {

    char versionchar[32]; // temporary string
    char tempchar2[4]; // layer

    // call CFrameHeader member function
    float ver = header.getVersion();

    // create the layer information with the amounts of I
	int i=0;
    for( ; i<header.getLayer(); i++ ) tempchar2[i] = 'I';
    tempchar2[i] = '\0';

    // combine strings
    sprintf(versionchar,"MPEG %g Layer %s", (double)ver, tempchar2);

    // copy result into inputstring
    strcpy(input, versionchar);

}

void CMP3Info::getMode(char* input) {

    char modechar[32]; // temporary string

    // call CFrameHeader member function
    header.getMode(modechar);

    // copy result into inputstring
    strcpy(input, modechar);

}

void CMP3Info::getTitle(char* input) {

    char titlechar[32]; // temporary string

    // call CFrameHeader member function
    tag.getTitle(titlechar);

    // copy result into inputstring
    strcpy(input, titlechar);

}

void CMP3Info::getArtist(char* input) {

    char artistchar[32]; // temporary string

    // call CFrameHeader member function
    tag.getArtist(artistchar);

    // copy result into inputstring
    strcpy(input, artistchar);

}

void CMP3Info::getAlbum(char* input) {

    char albumchar[32]; // temporary string

    // call CFrameHeader member function
    tag.getAlbum(albumchar);

    // copy result into inputstring
    strcpy(input, albumchar);

}

void CMP3Info::getComment(char* input) {

    char commentchar[32]; // temporary string

    // call CFrameHeader member function
    tag.getComment(commentchar);

    // copy result into inputstring
    strcpy(input, commentchar);

}

void CMP3Info::getGenre(char* input) {

    char genrechar[32]; // temporary string

    // call CFrameHeader member function
    tag.genreString(genrechar, true);

    // copy result into inputstring
    strcpy(input, genrechar);

}

void CMP3Info::getFileName(char* input) {

    strcpy(input, fileName);

}

void CMP3Info::showErrorMessage(UINT msg, const char* szMsgPlus) {

    switch(msg) {

        case ERR_NOSUCHFILE:
            {
                MessageBox(0,"You tried to open a file\nthat didn't work", szMsgPlus, MB_OK|MB_SYSTEMMODAL);
                return;
            }
        case ERR_NOMP3FILE:
            {
                MessageBox(0,"The file that was opened\nwas no correct .mp3-file", szMsgPlus, MB_OK|MB_SYSTEMMODAL);
                return;
            }
        case ERR_FILEOPEN:
            {
                MessageBox(0,"You tried to open a file\nthat didn't work", szMsgPlus, MB_OK|MB_SYSTEMMODAL);
                return;
            }
        case ERR_ID3TAG:
            {
                MessageBox(0,"No valid ID3-tag could be\ncreated", szMsgPlus, MB_OK|MB_SYSTEMMODAL);
                return;
            }

    }

}
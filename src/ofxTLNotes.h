/**
 * ofxTLNotes created by  Leo Zimmerman [http://www.leozimmerman.com.ar]
 *
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLKeyframes.h"
#include "ofxTextInputField.h"

class ofxTLNote : public ofxTLKeyframe {
public:
    ofxTLNote(int p);
    int pitch;
    string getPitchDisplay();

};

struct GridRow {
    float yPos;
    int pitch;
};

class ofxTLNotes : public ofxTLKeyframes {
public:
    ofxTLNotes();
    virtual ~ofxTLNotes();
    
    virtual void draw();
    
    virtual bool mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseMoved(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
    virtual void mouseReleased(ofMouseEventArgs& args, long millis);

    virtual void keyPressed(ofKeyEventArgs& args);
    
    void setRange(ofRange range);
    void setOctavesNum(int oct);
    void didEnterText();
    
    int getNote();
    int getNoteAtMillis(long millis);
    int getNoteAtPercent(float percent);
    
    
    ofxTextInputField textField;
    ofRectangle display;
    
    //time range contains MIN and MAX time in milliseconds
    //valueRange is 0 at the bottom of the track, and 1 at the top
    //if you have anything other than small dots keyframes you'll want to override
    //the default behavior
    virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);

    virtual string getTrackType();

    // Note-Specific
    void drawBackgroundGrid();
    
    
protected:
    vector<GridRow> gridRows;
    //void quantizeAllNotesByPos();
    void quantizeNoteByPos(ofxTLNote* note);
    void quantizeAllNotesByPitch();
    void quantizeNoteByPitch(ofxTLNote* note);
    float rowNormHeight;
    
    //always return the type for your track, in our case ofxTLEmptyKeyframe;
    //this will enusre that all keyframe objects passed to this class are of this type
    virtual ofxTLKeyframe* newKeyframe();
    //load this keyframe out of xml, which is alraedy pushed to the right level
    //only need to save custom properties that our subclass adds
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    //save custom properties into the xml
    virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    
    //return keyframe at this mouse point if you have non circular keyframes
    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p);
    
    //responde to right clicks on keyframes
    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args);
    
    //you can responde to a keyframe about to be killed with this function right before the parent class deletes it
    virtual void willDeleteKeyframe(ofxTLKeyframe* keyframe);
};

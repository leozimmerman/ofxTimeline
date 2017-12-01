//
//  ofxTLSequencer.cpp
//  example-emptyTrackTemplate
//
//  Created by Leo on 12/1/17.
//

#include "ofxTLNotes.h"
#include "ofxTimeline.h"
#include "ofxHotKeys.h"



ofxTLNote::ofxTLNote(){
    pitch = 60;
}

string ofxTLNote::getPitchDisplay() {
    
    string noteStr = "";
    int mod = pitch % 12;
    
    switch (mod) {
        case 0:
            noteStr = "C";
            break;
        case 1:
            noteStr = "C#";
            break;
        case 2:
            noteStr = "D";
            break;
        case 3:
            noteStr = "Eb";
            break;
        case 4:
            noteStr = "E";
            break;
        case 5:
            noteStr = "F";
            break;
        case 6:
            noteStr = "F#";
            break;
        case 7:
            noteStr = "G";
            break;
        case 8:
            noteStr = "Ab";
            break;
        case 9:
            noteStr = "A";
            break;
        case 10:
            noteStr = "Bb";
            break;
        case 11:
            noteStr = "B";
            break;
        default:
            break;
    }
    
    string display = ofToString(pitch) + "-" + noteStr;
    
    return ofToString(display);
}

//----------------------------

ofxTLNotes::ofxTLNotes(){

    setRange(ofRange(60, 71));
}

ofxTLNotes::~ofxTLNotes(){
    
}

void ofxTLNotes::setRange(ofRange range) {
    valueRange = range;
    gridRows.clear();
    
    for (int i = 0; i <= valueRange.span(); i++) {
        rowNormHeight = 1 / (valueRange.span()+1);
        float rowNormOffset = rowNormHeight * 0.5;
        float y = i * rowNormHeight + rowNormOffset;
        int p = valueRange.min + i;
        
        GridRow row = GridRow();
        row.yPos = y;
        row.pitch = p;
        gridRows.push_back(row);
    }
    
}

void ofxTLNotes::draw(){
    
    if(bounds.width == 0 || bounds.height < 2){
        return;
    }

    if(shouldRecomputePreviews || viewIsDirty){
        recomputePreviews();
    }
    
    drawBackgroundGrid();
    
    
    ofPushStyle();
    
    ofFill();
    for(int i = 0; i < keyframes.size(); i++){
        //make sure it's on screen
        if(isKeyframeIsInBounds(keyframes[i])){
            //we know the type because we created it in newKeyframe()
            //so we can safely cast
            ofxTLNote* note = (ofxTLNote*)keyframes[i];
            ofVec2f screenPoint = screenPositionForKeyframe(note);
            
            if(hoverKeyframe == note){
                ofSetColor(timeline->getColors().highlightColor, 100);
                ofCircle(screenPoint, 5);
                ofSetColor(timeline->getColors().textColor);
                timeline->getFont().drawString(note->getPitchDisplay(), screenPoint.x, screenPoint.y - 5);
            }
            else if(isKeyframeSelected(note)){
                ofSetColor(timeline->getColors().textColor);
                ofCircle(screenPoint, 5);
                ofSetColor(timeline->getColors().textColor);
                timeline->getFont().drawString(note->getPitchDisplay(), screenPoint.x, screenPoint.y - 5);
            }

            //ofSetColor(emptyKeyframe->color);
            ofSetColor(timeline->getColors().keyColor);
            ofCircle(screenPoint, 4);
        }
    }
    
    ofPopStyle();
}

void ofxTLNotes::drawBackgroundGrid() {
  
    ofPushStyle();

    int white = 50;
    int black = 20;
    
    for (int i = 0; i <= gridRows.size(); i++) {
        
        int mod = i%12;
        
        switch (mod) {
            case 0:
                ofSetColor(255, 255, 255, white);
                break;
            case 1:
                
                ofSetColor(255, 255, 255, black);
                break;
            case 2:
                ofSetColor(255, 255, 255, white);
                break;
            case 3:
                ofSetColor(255, 255, 255, black);
                break;
            case 4:
                ofSetColor(255, 255, 255, white);
                break;
            case 5:
                ofSetColor(255, 255, 255, white);
                break;
            case 6:
                ofSetColor(255, 255, 255, black);
                break;
            case 7:
                ofSetColor(255, 255, 255, white);
                break;
            case 8:
                ofSetColor(255, 255, 255, black);
                break;
            case 9:
                ofSetColor(255, 255, 255, white);
                break;
            case 10:
                ofSetColor(255, 255, 255, black);
                break;
            case 11:
                ofSetColor(255, 255, 255, white);
                break;
            default:
                break;
        }

        float y = (bounds.y + bounds.height) - (gridRows[i].yPos * bounds.height);
        ofDrawLine(bounds.x, y, bounds.x + bounds.width, y);
    }
    ofPopStyle();
    
}
int ofxTLNotes::getNote() {
    return getNoteAtMillis(currentTrackTime());
}

int ofxTLNotes::getNoteAtPercent(float percent){
    unsigned long long millis = percent*timeline->getDurationInMilliseconds();
    return getNoteAtMillis(millis);
}

///TODO: Testear esta función
int ofxTLNotes::getNoteAtMillis(long millis){
    
    if(keyframes.size() == 0){
        return 0;
    }
    
    vector<int> notes;
    
    //just one, or sampling before the first we can just return the first
    if(keyframes.size() == 1 || keyframes[0]->time >= millis){
        return ((ofxTLNote*)keyframes[0])->pitch;
        
    }
    //sampling after the last we return the last
    if(keyframes[keyframes.size()-1]->time <= millis){
        return ((ofxTLNote*)keyframes[keyframes.size()-1])->pitch;
    }
    
    //now we are somewhere in between, search
    //keyframes will always be sorted
    for(int i = 1; i < keyframes.size(); i++){
        ///FIXME: Siempre van a coincidir??
        if(keyframes[i]->time >= millis) {
            ofxTLNote* note  = (ofxTLNote*)keyframes[i];
            return note->pitch;
            //ofxTLNote* prevKey  = (ofxTLNote*)keyframes[i-1];
            //interpolate
//            float alpha = ofMap(sampleTime, prevKey->time, nextKey->time, 0, 1.0);
//            return prevKey->pitch.getLerped(nextKey->pitch, alpha);
        }
    }
    return 0;
}

string ofxTLNotes::getTrackType() {
    return "Notes";
}

bool ofxTLNotes::mousePressed(ofMouseEventArgs& args, long millis){
    //for the general behavior call the super class
    //or you can do your own thing. Return true if the click caused an item to
    //become selectd
    return ofxTLKeyframes::mousePressed(args, millis);
}

void ofxTLNotes::mouseMoved(ofMouseEventArgs& args, long millis){
    ofxTLKeyframes::mouseMoved(args, millis);
}

void ofxTLNotes::mouseDragged(ofMouseEventArgs& args, long millis){
    ofxTLKeyframes::mouseDragged(args, millis);
}

void ofxTLNotes::mouseReleased(ofMouseEventArgs& args, long millis){
    ofxTLKeyframes::mouseReleased(args, millis);
    for(int k = 0; k < selectedKeyframes.size(); k++) {
        ofxTLNote* note = (ofxTLNote*)selectedKeyframes[k];
        quantizeNote(note);
    }

}

void ofxTLNotes::quantizeNote(ofxTLNote* note){
    
    for (int i = 0; i <= gridRows.size(); i++) {
        float diff = abs(note->value - gridRows[i].yPos);
        if ( diff <= (rowNormHeight * 0.5) ) {
            note->value = gridRows[i].yPos;
            note->pitch = gridRows[i].pitch;
            return;
        }
    }
}

//keys pressed events, and nuding from arrow keys with normalized nudge amount 0 - 1.0
void ofxTLNotes::keyPressed(ofKeyEventArgs& args){
    ofxTLKeyframes::keyPressed(args);
}

void ofxTLNotes::regionSelected(ofLongRange timeRange, ofRange valueRange){
    //you can override the default to select other things than just dots
    ofxTLKeyframes::regionSelected(timeRange, valueRange);
}

ofxTLKeyframe* ofxTLNotes::newKeyframe(){

    ofxTLNote* newNote = new ofxTLNote();
    return newNote;
}


void ofxTLNotes::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLNote* note = (ofxTLNote*)key;
    note->pitch = xmlStore.getValue("pitch", 60);
    note->value = xmlStore.getValue("value", 0.5);

}

void ofxTLNotes::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLNote* note = (ofxTLNote*)key;
    xmlStore.addValue("pitch", note->pitch);
    xmlStore.addValue("value", note->value);
    
}

ofxTLKeyframe* ofxTLNotes::keyframeAtScreenpoint(ofVec2f p){
    return ofxTLKeyframes::keyframeAtScreenpoint(p);
}

void ofxTLNotes::selectedKeySecondaryClick(ofMouseEventArgs& args){
    //you can make a popup window start here
    //    timeline->presentedModalContent(this);
    //and then when you want to get rid of it call somewhere else
    //    timeline->dismissedModalContent();
    //this will lock all other timeline interaction and feed all things into your track
    //so feel free to draw out of bounds, but don't go off the screen or out of the timeline
}


void ofxTLNotes::willDeleteKeyframe(ofxTLKeyframe* keyframe){
    //do any cleanup before this keyframe gets hosed
}


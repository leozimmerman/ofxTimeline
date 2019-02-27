//
//  ofxTLSequencer.cpp
//  example-emptyTrackTemplate
//
//  Created by Leo on 12/1/17.
//

#include "ofxTLNotes.h"
#include "ofxTimeline.h"
#include "ofxHotKeys.h"



ofxTLNote::ofxTLNote(int p){
    pitch = p;
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
    setRange(ofRange(60,72));
    lastTimelinePoint = 0;
}

ofxTLNotes::~ofxTLNotes(){
    
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
                ofDrawCircle(screenPoint, 5);
                ofSetColor(timeline->getColors().textColor);
                timeline->getFont().drawString(note->getPitchDisplay(), screenPoint.x, screenPoint.y - 5);
            }
            else if(isKeyframeSelected(note)){
                ofSetColor(timeline->getColors().textColor);
                ofDrawCircle(screenPoint, 5);
                ofSetColor(timeline->getColors().textColor);
                timeline->getFont().drawString(note->getPitchDisplay(), screenPoint.x, screenPoint.y - 5);
            }

            //ofSetColor(emptyKeyframe->color);
            ofSetColor(timeline->getColors().keyColor);
            ofDrawCircle(screenPoint, 4);
        }
        
    }
    
   
    
    //int textHeight = bounds.y + 10;
    display = ofRectangle(bounds.x , bounds.y , 50, 20);
    
    textField.bounds.x = display.x;
    textField.bounds.y = display.y;
    
     ofSetColor(0);
    ofDrawRectangle(display);
    
    ofSetColor(255);
    textField.draw();
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

int ofxTLNotes::getNoteAtMillis(long millis){
    if(keyframes.size() == 0){ return 0; }
    
    long thisTimelinePoint = millis;
    if (thisTimelinePoint == lastTimelinePoint){ return 0; }
    
    auto range = timeline->getInOutRangeMillis();
    
    for(auto keyframe : keyframes){
        auto time = keyframe->time;
        auto timeInRange = range.contains(time);
        if(timeInRange && lastTimelinePoint <= time && thisTimelinePoint >= time) {
            lastTimelinePoint = thisTimelinePoint;
            ofxTLNote* note  = (ofxTLNote*)keyframe;
            return note->pitch;
        }
    }
    lastTimelinePoint = thisTimelinePoint;
    return 0;
}

string ofxTLNotes::getTrackType() {
    return "Notes";
}

void ofxTLNotes::setOctavesNum(int oct) {
    int min = 0;
    int max = 12 * oct;
    setRange(ofRange(min, max));
}

void ofxTLNotes::setRange(ofRange range){
    
    valueRange = range;
    textField.text = ofToString(valueRange.min) + "-" + ofToString(valueRange.max);
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
    
    quantizeAllNotesByPitch();
}

void ofxTLNotes::didEnterText(){
    
    string s = textField.text;
    string delimiter = "-";

    int min;
    int max;
    
    size_t pos = 0;
    string token;
    vector<string> v;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        //std::cout << token << std::endl;
        v.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    v.push_back(s);
    
    min = std::stoi(v[0]);
    max = std::stoi(v[1]);
    
    
    if (min < 0) {min = 0;}
    if (max < 0) {max = 0;}
    if (min > 96) {min = 96;}
    if (max > 96) {max = 96;}
    if (max <= min) {max = min+1;}
    
    setRange(ofRange(min, max));
}

bool enteringText = false;
bool ofxTLNotes::mousePressed(ofMouseEventArgs& args, long millis){
    //for the general behavior call the super class
    //or you can do your own thing. Return true if the click caused an item to
    //become selectd
    
    if (display.inside(args.x, args.y)) {
        if (textField.isEditing()) {
            textField.endEditing();
            enteringText = false;
            timeline->dismissedModalContent();
        } else {
            textField.beginEditing();
            enteringText = true;
            timeline->presentedModalContent(this);
        }
        return false;
    } else {
        return ofxTLKeyframes::mousePressed(args, millis);
    }
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
        quantizeNoteByPos(note);
    }

}

void ofxTLNotes::quantizeNoteByPos(ofxTLNote* note){
    
    for (int i = 0; i <= gridRows.size(); i++) {
        float diff = abs(note->value - gridRows[i].yPos);
        if ( diff <= (rowNormHeight * 0.5) ) {
            note->value = gridRows[i].yPos;
            note->pitch = gridRows[i].pitch;
            return;
        }
    }
}
void ofxTLNotes::quantizeAllNotesByPitch(){
    for(int k = 0; k < keyframes.size(); k++) {
        ofxTLNote* note = (ofxTLNote*)keyframes[k];
        quantizeNoteByPitch(note);
    }
}
void ofxTLNotes::quantizeNoteByPitch(ofxTLNote* note){
    
    if (note->pitch < valueRange.min) {note->pitch = valueRange.min;}
    else if (note->pitch > valueRange.max) {note->pitch = valueRange.max;}
    
    for (int i = 0; i <= gridRows.size(); i++) {
        if (note->pitch == gridRows[i].pitch) {
            note->value = gridRows[i].yPos;
            return;
        }
    }
    
}

//keys pressed events, and nuding from arrow keys with normalized nudge amount 0 - 1.0
void ofxTLNotes::keyPressed(ofKeyEventArgs& args){
    
    if (enteringText) {
        if(args.key == OF_KEY_RETURN){
            textField.endEditing();
            enteringText = false;
            timeline->dismissedModalContent();
            //timeline->flagTrackModified(this);
            didEnterText();
            
            
        } else {
            textField.keyPressed(args);
        }
        
    } else {
        ofxTLKeyframes::keyPressed(args);
    }
}

void ofxTLNotes::regionSelected(ofLongRange timeRange, ofRange valueRange){
    //you can override the default to select other things than just dots
    ofxTLKeyframes::regionSelected(timeRange, valueRange);
}

ofxTLKeyframe* ofxTLNotes::newKeyframe(){

    ofxTLNote* newNote = new ofxTLNote(valueRange.min);
    quantizeNoteByPos(newNote);
    return newNote;
}


void ofxTLNotes::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLNote* note = (ofxTLNote*)key;
    note->pitch = xmlStore.getValue("pitch", valueRange.min);
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

void ofxTLNotes::createKeyframesFromXML(ofxXmlSettings xmlStore, vector<ofxTLKeyframe*>& keyContainer){
    ofxTLKeyframes::createKeyframesFromXML(xmlStore, keyContainer);
    restoreRange(xmlStore);
}

string ofxTLNotes::getXMLStringForKeyframes(vector<ofxTLKeyframe*>& keys){
    string keysStr = ofxTLKeyframes::getXMLStringForKeyframes(keys);
    string rangeStr = storeRange(valueRange);
    return keysStr + rangeStr;
}

string ofxTLNotes::storeRange(ofRange range){
    ofxXmlSettings savedRange;
    savedRange.addTag("range");
    savedRange.pushTag("range");
    savedRange.addValue("min", range.min);
    savedRange.addValue("max", range.max);
    savedRange.popTag();
    string str;
    savedRange.copyXmlToString(str);
    return str;
}

void ofxTLNotes::restoreRange(ofxXmlSettings& xmlStore){
    xmlStore.pushTag("range");
    int min = xmlStore.getValue("min", 0);
    int max = xmlStore.getValue("max", 0);
    xmlStore.popTag();
    setRange(ofRange(min,max));
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

void ofxTLNotes::playbackLooped(ofxTLPlaybackEventArgs& args){
    lastTimelinePoint = 0;
}

void ofxTLNotes::playbackStarted(ofxTLPlaybackEventArgs& args){
    ofxTLTrack::playbackStarted(args);
    lastTimelinePoint = currentTrackTime();
}

void ofxTLNotes::prepareForRenderingData(){
    lastTimelinePoint = 0;
}

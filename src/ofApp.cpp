#include "ofApp.h"

#include "ofUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //ofSetFrameRate(0);

    gui.setup();
    gui.add(bpmSlider.setup("BPM", 135, 60, 180));
    gui.add(stepButton.setup("Simulate a step", false));
    gui.add(blackOutToggle.setup("Black out", false));
    
    artnet.setup("169.254.181.144", 0, 1);
    
    dmxDevice = ofxGenericDmx::openFirstDevice(false);
    
    oscSender.setup("localhost", 4321);
}

//--------------------------------------------------------------
void ofApp::update() {
    bool beat = false;
    if (lastBeatTime + 60.0f / bpmSlider < ofGetElapsedTimef()) {
        beat = true;
        lastBeatTime = ofGetElapsedTimef();
    }
    
    bool step = false;
    step = stepButton;
    
    beatHistory.push_back(beat);
    stepHistory.push_back(step);
    if (beatHistory.size() > ofGetFrameRate() * 3) {
        beatHistory.erase(beatHistory.begin());
        stepHistory.erase(stepHistory.begin());
    }
    
    unsigned char data[3];
    data[0] = 0;
    data[1] = 1;
    data[2] = blackOutToggle ? 0 : (ofGetElapsedTimeMillis() % (60 * 1000 / bpmSlider) < 100 ? 255 : 0);

    artnet.sendDmx("169.254.183.100", data + 1, 2);
    dmxDevice->writeDmx(data, 3);

    if (step) {
        ofxOscMessage oscMessage;
        oscMessage.setAddress("/BOTH/triggerLightWithRGBColor");
        oscMessage.addIntArg(0);
        oscMessage.addIntArg(255);
        oscMessage.addIntArg(255);
        oscMessage.addIntArg(255);
        
        oscSender.sendMessage(oscMessage);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0, 0, 0);
    
    // Display FPS
    ofColor(255, 255, 255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()) + "fps", ofGetWidth() - 150, 20);

    // Display beats & steps
    int framesFor3Seconds = ofGetFrameRate() * 3;
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofFill();
    for (int i = 0; i < beatHistory.size(); i++) {
        if (beatHistory[i]) {
            ofSetColor(255, 0, 0);
            ofDrawRectangle(ofGetWidth() - ofGetWidth() * (beatHistory.size() - i) / framesFor3Seconds, 0, ofGetWidth() / framesFor3Seconds, ofGetHeight());
        }

        if (stepHistory[i]) {
            ofSetColor(0, 255, 0);
            ofDrawRectangle(ofGetWidth() - ofGetWidth() * (stepHistory.size() - i) / framesFor3Seconds, 0, ofGetWidth() / framesFor3Seconds, ofGetHeight());
        }
    }
    
    gui.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

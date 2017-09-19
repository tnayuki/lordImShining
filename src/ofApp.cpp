#include "ofApp.h"

#include "ofUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //ofSetFrameRate(0);

    ofxSoundFile soundFile("Revolution of Life.m4a");
    soundFile.readTo(musicSoundBuffer);

    ofSoundStreamSetup(2, 0);
    
    gui.setup();
    gui.add(stepButton.setup("Simulate a step", false));
    gui.add(blackOutToggle.setup("Black out", false));
    
    artnet.setup("169.254.181.144", 0, 1);
    
    oscReceiver.setup(1234);
    oscSender.setup("localhost", 4321);

    ofxOscMessage oscMessage;
    oscMessage.setAddress("/BOTH/setLightOff");
    oscMessage.addIntArg(0);
    oscSender.sendMessage(oscMessage);
}

//--------------------------------------------------------------
void ofApp::update() {
    bool beat = false;
    if (lastBeatTime + 60.0f / 135 < ofGetElapsedTimef()) {
        beat = true;
        lastBeatTime = ofGetElapsedTimef();
    }
    
    bool step = stepButton;

    ofxOscMessage oscMessage;
    while (oscReceiver.getNextMessage(oscMessage)) {
        if (oscMessage.getAddress() == "/LEFT/gesture" || oscMessage.getAddress() == "/RIGHT/gesture") {
            if (oscMessage.getArgAsString(0) == "STEP") {
                step = true;
            }
        }
    }

    beatHistory.push_back(beat);
    stepHistory.push_back(step);
    if (beatHistory.size() > ofGetFrameRate() * 3) {
        beatHistory.erase(beatHistory.begin());
        stepHistory.erase(stepHistory.begin());
    }
    
    unsigned char data[3];
    data[0] = 0;
    data[1] = 1;
    data[2] = blackOutToggle ? 0 : (ofGetElapsedTimeMillis() % (60 * 1000 / 135) < 100 ? 255 : 0);

    artnet.sendDmx("169.254.183.100", data + 1, 2);

    if (beat) {
        ofxOscMessage oscMessage;
        oscMessage.setAddress("/LEFT/triggerLightWithRGBColor");
        oscMessage.addIntArg(1);
        if (beatNumber == 0) {
            oscMessage.addIntArg(255);
            oscMessage.addIntArg(0);
            oscMessage.addIntArg(0);
        } else if (beatNumber == 1) {
            oscMessage.addIntArg(0);
            oscMessage.addIntArg(255);
            oscMessage.addIntArg(0);
        } else if (beatNumber == 2) {
            oscMessage.addIntArg(0);
            oscMessage.addIntArg(0);
            oscMessage.addIntArg(255);
        } else if (beatNumber == 3) {
            oscMessage.addIntArg(255);
            oscMessage.addIntArg(255);
            oscMessage.addIntArg(255);
        }
        
        oscSender.sendMessage(oscMessage);
        
        beatNumber = (beatNumber + 1) % 4;
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

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    for(int i = 0; i < outBuffer.size(); i += 2) {
        outBuffer[i] = musicSoundBuffer[musicPosition];
        outBuffer[i + 1] = musicSoundBuffer[musicPosition + 1];
        
        musicPosition += 2;
        
        if (musicPosition >= musicSoundBuffer.size()) {
            musicPosition = 0;
        }
    }
}

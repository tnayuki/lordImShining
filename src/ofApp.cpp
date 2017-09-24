#include "ofApp.h"

#include "ofUtils.h"

extern void InitializePlugin();
extern void VibrateInternal(int index, unsigned char strength);

//--------------------------------------------------------------
void ofApp::setup(){
    InitializePlugin();

    ofSetFrameRate(60);

    videoPlayer.load("TGS 2017.mov");
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    videoPlayer.play();
    
    font.loadFont("Verdana.ttf", 20);

    ofxSoundFile soundFile("Revolution of Life.m4a");
    soundFile.readTo(musicSoundBuffer);

    musicPosition = 7.111111111 * 44100 * 2 * 4;
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
    videoPlayer.update();

    if (currentQuarterBeat == 4) {
        VibrateInternal(0, 255);
        VibrateInternal(1, 255);
    } if (currentQuarterBeat == 1) {
        VibrateInternal(0, 0);
        VibrateInternal(1, 0);
    }

    bool beat = false;
    if (lastBeatTime + 60.0f / 135 < ofGetElapsedTimef()) {
        beat = true;
        lastBeatTime = ofGetElapsedTimef();
    }
    
    if (stepButton) {
        step = true;
    }

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
    
    unsigned char data[2];
    data[0] = data[1] = 0;

    if (step) {
        flashFrameCount = 1;
        flashSpeed = 1;

        if (ofGetElapsedTimef() - lastBeatTime < 0.2f || lastBeatTime + 60.0f / 135.0f - ofGetElapsedTimef() < 0.2f) {
            flashFrameCount = 10;
            flashSpeed = 255;
        }

        if (currentBar >= 25 && currentBar < 33) {
            flashFrameCount = 23;
            flashSpeed = 255;
        } else if (currentBar >= 65 && currentBar < 81) {
            flashFrameCount = 23;
            flashSpeed = 255;
        }

    } else if (!isPlaying && currentBar == 1 && currentBeat == 1 && currentQuarterBeat == 1) {
        flashFrameCount = 10;
        flashSpeed = 255;
    }
    
    if (flashFrameCount > 0) {
        data[0] = flashSpeed;
        data[1] = blackOutToggle ? 0 : 255;
        
        flashFrameCount--;
    } else {
        data[0] = 0;
        data[1] = 0;
    }

    artnet.sendDmx("169.254.183.100", data, 2);

    if (beat) {
        ofxOscMessage oscMessage;
        oscMessage.setAddress("/BOTH/triggerLightWithRGBColor");
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
    
    step = false;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    ofSetColor(255, 255, 255);
    ofClear(0, 0, 0);

    if (!isPlaying) {
        videoPlayer.draw(0, 0, ofGetWidth(), ofGetHeight());
        
        font.drawString("Press the space key", 320, 640);
    }
    
    // Display FPS
    ofDrawBitmapString(ofToString(ofGetFrameRate()) + "fps", ofGetWidth() - 150, 20);

    // Display current bar/beat
    ofDrawBitmapString(ofToString(currentBar) + "." + ofToString(currentBeat) + "." + ofToString(currentQuarterBeat), ofGetWidth() - 150, 40);

    // Display frame count to flash;
    ofDrawBitmapString(ofToString(flashFrameCount), ofGetWidth() - 150, 60);
    
    // Display beats & steps
    int framesFor3Seconds = ofGetTargetFrameRate() * 3;

    ofBlendMode(OF_BLENDMODE_ADD);
    for (int i = 0; i < beatHistory.size(); i++) {
        if (beatHistory[i]) {
            ofSetColor(255, 0, 0);
            ofDrawRectangle(ofGetWidth() - ofGetWidth() * (beatHistory.size() - i) / framesFor3Seconds, ofGetHeight() - ofGetHeight() / 32, ofGetWidth() / framesFor3Seconds, ofGetHeight());
        }

        if (stepHistory[i]) {
            ofSetColor(0, 255, 0);
            ofDrawRectangle(ofGetWidth() - ofGetWidth() * (stepHistory.size() - i) / framesFor3Seconds, ofGetHeight() - ofGetHeight() / 32, ofGetWidth() / framesFor3Seconds, ofGetHeight());
        }
    }
    
    gui.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
        isPlaying = !isPlaying;
        musicPosition = 7.111111111 * 44100 * 2 * 4;
    } else {
        if ((key == 'L' || key == 'R' || key == 'M' || key == 'S') && (!lastStepTime || lastStepTime - ofGetElapsedTimeMillis() > 200)) {
            step = true;
            lastStepTime = ofGetElapsedTimeMillis();
        }
    }
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
    currentBar = musicPosition / 156800 + 1;
    currentBeat = (musicPosition % 156800) / (156800 / 4) + 1;
    currentQuarterBeat = (musicPosition % (156800 / 4)) / (156800 / 4 / 4) + 1;

    if (musicPosition >= musicSoundBuffer.size()) {
        return;
    }

    for(int i = 0; i < outBuffer.size(); i += 2) {
        outBuffer[i] = musicSoundBuffer[musicPosition];
        outBuffer[i + 1] = musicSoundBuffer[musicPosition + 1];
        
        musicPosition += 2;
        
        if (!isPlaying) {
            if (musicPosition >= 7.111111111 * 44100 * 2 * 5) {
                musicPosition = 7.111111111 * 44100 * 2 * 4;
            }
        } else {
            if (musicPosition >= musicSoundBuffer.size()) {
                break;
            }
        }
    }
}

#include "ofApp.h"

#include "ofUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //ofSetFrameRate(0);
    
    gui.setup();
    gui.add(bpmSlider.setup("BPM", 135, 60, 180));
    gui.add(blackOutToggle.setup("Black out", false));
    
    artnet.setup("169.254.181.144", 0, 1);
    
    dmxDevice = ofxGenericDmx::openFirstDevice(false);
}

//--------------------------------------------------------------
void ofApp::update() {
    unsigned char data[3];
    data[0] = 0;
    data[1] = 1;
    data[2] = blackOutToggle ? 0 : (ofGetElapsedTimeMillis() % (60 * 1000 / bpmSlider) < 100 ? 255 : 0);

    artnet.sendDmx("169.254.183.100", data + 1, 2);
    dmxDevice->writeDmx(data, 3);
}

//--------------------------------------------------------------
void ofApp::draw(){
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

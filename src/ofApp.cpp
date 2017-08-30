#include "ofApp.h"

#include "ofUtils.h"

bool flash = false;

//--------------------------------------------------------------
void ofApp::setup(){
    //ofSetFrameRate(0);
    
    gui.setup();
    gui.add(learnBackgroundButton.setup("Learn background"));
    gui.add(bpmSlider.setup("BPM", 135, 60, 180));
    gui.add(blackOutToggle.setup("Black out", false));
    gui.add(thresholdSlider.setup("Threshold", 100, 0, 255));
    
    detectionAreaPanel.setup();
    detectionAreaPanel.add(detectionAreaX.setup("X", 0, 0, 512));
    detectionAreaPanel.add(detectionAreaY.setup("Y", 0, 0, 424));
    detectionAreaPanel.add(detectionAreaWidth.setup("Width", 512, 0, 512));
    detectionAreaPanel.add(detectionAreaHeight.setup("Height", 424, 0, 424));
    
    artnet.setup("169.254.181.144", 0, 1);
    kinect.open();
}

//--------------------------------------------------------------
void ofApp::update() {
    kinect.update();
    if (kinect.isFrameNew()) {
        //ofLog() << kinect.getDepthPixels().getWidth() << "," << kinect.getDepthPixels().getHeight();
        greyscaleImage.setFromPixels(kinect.getDepthPixels());

        if (learnBackgroundButton) {
            backgroundGreyscaleImage = greyscaleImage;
        }
        
        greyscaleImage.threshold(thresholdSlider);

        int mean = 0;
        for (int i = detectionAreaY; i < detectionAreaY + detectionAreaHeight; i++) {
            for (int j = detectionAreaX; j < detectionAreaX + detectionAreaWidth; j++) {
                mean += greyscaleImage.getPixels().getColor(j, i).r;
            }
        }

        mean /= detectionAreaWidth * detectionAreaHeight;
        ofLog() << mean;

        meanVector.push_back(mean);
        int mean2 = std::accumulate(meanVector.begin(), meanVector.end(), 0) / meanVector.size();
        
        if (meanVector.size() > 5) {
            meanVector.erase(meanVector.begin());
        }

        chartData.push_back(mean - lastMean);
        lastMean = mean;
        if (chartData.size() >= 256) {
            chartData.erase(chartData.begin());
        }

        //ofLog() << mean2;
        
        //greyscaleImage.absDiff(backgroundGreyscaleImage, greyscaleImage);
        //greyscaleImage.threshold(180);

        //contourFinder.findContours(greyscaleImage, 5, (340*240)/4, 4, false, true);
        
        //texDepth.loadData(kinect.getDepthPixels());

        unsigned char data[2];
        data[0] = 50;
        //data[1] = blackOutToggle ? 0 : (ofGetElapsedTimeMillis() % (60 * 1000 / bpmSlider) < 100/* flash*/ ? 255 : 0);
        data[1] = chartData.back() > 10 ? 255 : 0;
        artnet.sendDmx("169.254.183.100", data, 2);
        
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofScale((float)ofGetWidth() / kinect.getDepthPixels().getWidth(), (float)ofGetHeight() / (float)kinect.getDepthPixels().getHeight());
    
    ofSetColor(255);
    greyscaleImage.draw(0, 0);
    contourFinder.draw(0, 0);
    
    ofSetColor(255, 0, 0);
    ofSetLineWidth(3);
    ofNoFill();
    ofDrawRectangle(detectionAreaX, detectionAreaY, detectionAreaWidth, detectionAreaHeight);
    
    ofPopMatrix();
    
    if (chartData.size() > 2) {
        ofPushMatrix();
        
        ofSetColor(0, 255, 0);
        ofSetLineWidth(2);

        ofScale((float)ofGetWidth() / 256, (float)ofGetHeight() / (float)256);
        for (int i = 0; i < 255; i++) {
            if (i >= chartData.size()) break;
            
            ofDrawLine(i, 255 - chartData[i], i + 1, 255 - chartData[i + 1]);
        }

        ofPopMatrix();
    }

    gui.draw();
    detectionAreaPanel.draw();
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
    int i = x * kinect.getDepthPixels().getWidth() / ofGetWidth() - detectionAreaX;
    if (i > 0) {
        detectionAreaWidth = i;
    }
    
    i = y * kinect.getDepthPixels().getHeight() / ofGetHeight() - detectionAreaY;
    if (i > 0) {
        detectionAreaHeight = i;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//    flash = true;
    detectionAreaX = x * kinect.getDepthPixels().getWidth() / ofGetWidth();
    detectionAreaY = y * kinect.getDepthPixels().getHeight() / ofGetHeight();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
//    flash = false;
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

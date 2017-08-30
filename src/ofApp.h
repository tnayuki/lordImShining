#pragma once

#include "ofMain.h"

#include <numeric>
#include <vector>

#include "ofxArtnet.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        ofxArtnet artnet;
        ofxKinectV2 kinect;
        ofTexture texDepth;
        ofxCvContourFinder contourFinder;

        ofxPanel gui;
        ofxButton learnBackgroundButton;
        ofxToggle blackOutToggle;
        ofxSlider<int> bpmSlider;
        ofxSlider<int> thresholdSlider;

        ofxPanel detectionAreaPanel;
        ofxSlider<int> detectionAreaX;
        ofxSlider<int> detectionAreaY;
        ofxSlider<int> detectionAreaWidth;
        ofxSlider<int> detectionAreaHeight;
    
        ofxCvGrayscaleImage backgroundGreyscaleImage;
        ofxCvGrayscaleImage greyscaleImage;

        int lastMean;
        std::vector<int> meanVector;
        std::vector<int> chartData;
};

#pragma once

#include "ofMain.h"

#include <numeric>
#include <vector>

#include "ofxArtnet.h"
#include "ofxGui.h"
#include "ofxGenericDmx.h"
#include "ofxOsc.h"

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
    
        float fps;

        ofxArtnet artnet;

        ofxPanel gui;
        ofxButton stepButton;
        ofxToggle blackOutToggle;
        ofxSlider<int> bpmSlider;

        DmxDevice *dmxDevice;
        ofxOscSender oscSender;

        float lastBeatTime;
        vector<bool> beatHistory;
    
        vector<bool> stepHistory;
};

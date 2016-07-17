#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxKinect.h"
#include "ofxKinectBlobFinder.h"
#include "ofxKinectHitBoxes.h"

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
    
    private:
    
        ofxKinect kinect;
        ofxKinectBlobFinder tracker;
    
        ofImage depthImg;
        ofEasyCam camera;
    
        ofxPanel gui;
        ofParameter<int> thresh2D, minPoints, maxBlobs, trackedBlobs, thresholdNear, thresholdFar;
        ofParameter<float> minVol, maxVol;
        ofParameter<ofVec3f> boxMin, boxMax, thresh3D;
    
        bool visible, drawPointCloud, save;
    
        ofxKinectHitBoxesManager hitBoxesManager;
        void onHitBoxStateChanged(int & boxID);

};

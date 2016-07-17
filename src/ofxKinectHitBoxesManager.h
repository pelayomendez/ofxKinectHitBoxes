/****************************************************************************
*
* ofxKinectHitBoxesManager.h
* openFrameworks
*
* Pelayo Méndez, Febrero 2016
*
******************************************************************************/
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxPointInMesh.h"
#include "ofxKinectBlob.h"
#include "ofxKinectHitBox.h"

class ofxKinectHitBoxesManager {

	public:
	
		void setup();

		void drawBoxes();
		void drawHitBoxesGui();

		void addHitBoxToWorld(ofVec3f position, ofPoint dimensions);
		
		void checkBlobs(vector<ofxKinectBlob> & blobs);
		bool checkPointsInsideBox(int boxIndex, vector<ofVec3f> & points);

		void loadHitboxesSavedData();
		void saveHitBoxesData();

		ofEvent<int> onHitBoxBecomesActive;
		ofEvent<int> onHitBoxBecomesInactive;

		bool getIsHitBoxActive(int hitBoxIndex);

	private:
		
		ofxPanel gui;
		ofParameter<int> selectedBoxId;
		ofParameter<ofVec3f> boxPosition, boxDimensions;
		ofxButton addButton, removeButton;

		vector<ofxKinectHitBox> hitBoxes;

		void selectedBoxIdBoxIdChanged(int & boxId);

		void addHitBox();
		void removeHitBox();

		

};

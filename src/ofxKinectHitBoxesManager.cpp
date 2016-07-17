#include "ofxKinectHitBoxesManager.h"


//--------------------------------------------------------------
void ofxKinectHitBoxesManager::setup() {

	selectedBoxId.set("selectedBoxId", -1, -1, -1);
	selectedBoxId.addListener(this,&ofxKinectHitBoxesManager::selectedBoxIdBoxIdChanged);

	boxPosition.set("boxPosition", ofVec3f(0), ofVec3f(-10), ofVec3f(10));
	boxDimensions.set("boxDimensions", ofVec3f(0), ofVec3f(0), ofVec3f(10));

	gui.setup("hitBoxes", "guiHitboxes.xml");
	gui.add(selectedBoxId);
	gui.add(boxPosition);
	gui.add(boxDimensions);
	gui.add(addButton.setup("+"));
	gui.add(removeButton.setup("-"));
	gui.loadFromFile("guiHitboxes.xml");
	gui.setPosition(ofGetWidth() - 240, 240);

	addButton.addListener(this, &ofxKinectHitBoxesManager::addHitBox);
	removeButton.addListener(this, &ofxKinectHitBoxesManager::removeHitBox);

	selectedBoxId.set(-1);

}


//--------------------------------------------------------------
void ofxKinectHitBoxesManager::loadHitboxesSavedData() {

	hitBoxes.clear();

	ofxXmlSettings hitBoxesData;

	if (hitBoxesData.loadFile("hitboxes.xml")) {

		hitBoxesData.pushTag("hitboxes");
		int numberOfSavedHitboxes = hitBoxesData.getNumTags("hitbox");
		for (int i = 0; i < numberOfSavedHitboxes; i++) {

			hitBoxesData.pushTag("hitbox", i);

			hitBoxesData.pushTag("position");

			ofVec3f p;
			p.x = hitBoxesData.getValue("X", 0.0f);
			p.y = hitBoxesData.getValue("Y", 0.0f);
			p.z = hitBoxesData.getValue("Z", 0.0f);
			hitBoxesData.popTag();

			hitBoxesData.pushTag("dimension");

			ofVec3f d;
			d.x = hitBoxesData.getValue("X", 0.0f);
			d.y = hitBoxesData.getValue("Y", 0.0f);
			d.z = hitBoxesData.getValue("Z", 0.0f);
			hitBoxesData.popTag();

			hitBoxesData.popTag();

			addHitBoxToWorld(p, d);

		}

	}

};


//--------------------------------------------------------------
void ofxKinectHitBoxesManager::saveHitBoxesData() {

	ofxXmlSettings hitBoxesData;

	hitBoxesData.addTag("hitboxes");
	hitBoxesData.pushTag("hitboxes");

	for (unsigned int i = 0; i < hitBoxes.size(); i++) {

		hitBoxesData.addTag("hitbox");
		hitBoxesData.pushTag("hitbox",i);

		hitBoxesData.addTag("position");
		hitBoxesData.pushTag("position");
		hitBoxesData.setValue("X", hitBoxes[i].centroid.x);
		hitBoxesData.setValue("Y", hitBoxes[i].centroid.y);
		hitBoxesData.setValue("Z", hitBoxes[i].centroid.z);
		hitBoxesData.popTag();

		hitBoxesData.addTag("dimension");
		hitBoxesData.pushTag("dimension");
		hitBoxesData.setValue("X", hitBoxes[i].dimensions.x);
		hitBoxesData.setValue("Y", hitBoxes[i].dimensions.y);
		hitBoxesData.setValue("Z", hitBoxes[i].dimensions.z);
		hitBoxesData.popTag();

		hitBoxesData.popTag();

	}

	hitBoxesData.popTag();

	hitBoxesData.saveFile("hitboxes.xml");

};


//--------------------------------------------------------------
void ofxKinectHitBoxesManager::drawHitBoxesGui() {
	
	gui.draw();

};

//--------------------------------------------------------------
void ofxKinectHitBoxesManager::addHitBox() {

	ofVec3f boxCenter = ofVec3f(0.0f, 0.0f, 0.0f);
	ofPoint boxDimensions = ofPoint(1.5, 1.5, 1.5);
	addHitBoxToWorld(boxCenter, boxDimensions);

}

//--------------------------------------------------------------
void ofxKinectHitBoxesManager::removeHitBox() {


}

//--------------------------------------------------------------
void  ofxKinectHitBoxesManager::selectedBoxIdBoxIdChanged(int & boxId) {

	if(boxId > -1) {
		boxPosition.set(hitBoxes[boxId].centroid);
		boxDimensions.set(hitBoxes[boxId].dimensions);
	}
	else {
		boxPosition.set(ofVec3f(0,0,0));
		boxDimensions.set(ofVec3f(0, 0, 0));
	}

}



//--------------------------------------------------------------
void ofxKinectHitBoxesManager::addHitBoxToWorld(ofVec3f position, ofPoint dimensions) {

	ofxKinectHitBox newBox;
	newBox.centroid = position;
	newBox.dimensions = dimensions;
	newBox.generateMesh();

	hitBoxes.push_back(newBox);

	selectedBoxId.set("boxId", -1, -1, hitBoxes.size()-1);

}

//--------------------------------------------------------------
void ofxKinectHitBoxesManager::drawBoxes() {

	int selectedId = selectedBoxId.get();

	if (selectedId > -1) {

		hitBoxes[selectedId].centroid = boxPosition.get();
		hitBoxes[selectedId].dimensions = boxDimensions.get();
		hitBoxes[selectedId].generateMesh();

	}

	for (unsigned int i = 0; i < hitBoxes.size(); i++) {

		bool isEdited = false;

		if (selectedBoxId.get() == i)
			isEdited = true;

		hitBoxes[i].customDraw(isEdited);

	}

}

//--------------------------------------------------------------
void  ofxKinectHitBoxesManager::checkBlobs(vector<ofxKinectBlob> & blobs) {

	for (int i = 0; i < hitBoxes.size(); i++) {

		bool currentBoxState = hitBoxes[i].isActive;

		bool hasPointIside = false;
		for (int a = 0; a < blobs.size(); a++) {
			if (checkPointsInsideBox(i, blobs[a].mesh.getVertices())) {
				hasPointIside = true;
				break;
			}
		}

		hitBoxes[i].isActive = hasPointIside;

		if (currentBoxState != hasPointIside) {

			if (hasPointIside) {
				ofNotifyEvent(onHitBoxBecomesActive, i);
			}
			else {
				ofNotifyEvent(onHitBoxBecomesInactive, i);
			}

		}

	}

};

//--------------------------------------------------------------
bool ofxKinectHitBoxesManager::checkPointsInsideBox(int boxIndex, vector<ofVec3f> & points) {

	for (unsigned int a = 0; a < points.size(); a++) {

		bool isInside = ofxPointInMesh::isInside(points[a], hitBoxes[boxIndex].mesh);
		if (isInside)
			return true;

	}

	return false;

};

//--------------------------------------------------------------
bool ofxKinectHitBoxesManager::getIsHitBoxActive(int hitBoxIndex) {

	return hitBoxes[hitBoxIndex].isActive;

};
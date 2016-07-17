/****************************************************************************
*
* ofxKinectHitBox.h
* openFrameworks
*
* Pelayo Méndez, Febrero 2016
* 
******************************************************************************/
#pragma once

#include "ofMain.h"

class ofxKinectHitBox :public ofNode {

	public:
	
		virtual void customDraw(bool isBeingEdited) {

			ofDrawAxis(0.5f);
			//ofScale(maxX.x - minX.x, maxY.y - minY.y, maxZ.z - minZ.z);
			//ofPushStyle();
			if(isActive) {
				ofSetColor(ofColor::hotPink);
			} else {
				ofSetColor(ofColor::lightPink,125.0f);
			}
			if(isBeingEdited)
				ofSetColor(ofColor::yellow);
			//ofNoFill();
			ofPushMatrix();
			//ofTranslate(centroid);
			mesh.drawWireframe();
			ofPopMatrix();
			//ofDrawBox(centroid,dimensions.x,dimensions.y,dimensions.z);
			//ofPopStyle();
			// if (bDrawAxis)
			// if (bDrawBox) drawGlWireBox();

		}
	
		void generateMesh() {

			mesh = ofMesh::box(dimensions.x, dimensions.y, dimensions.z);
			for (unsigned int i = 0; i <mesh.getVertices().size(); i++) {
				mesh.getVertices()[i].x += centroid.x;
				mesh.getVertices()[i].y += centroid.y;
				mesh.getVertices()[i].z += centroid.z;
			}

		}

		bool isActive;

		ofMesh mesh;
		ofVec3f centroid;
		ofVec3f dimensions;


};


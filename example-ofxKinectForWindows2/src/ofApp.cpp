#include "ofApp.h"

#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

//--------------------------------------------------------------
void ofApp::setup() {

	ofBackground(0);
	ofSetVerticalSync(true);

	kinect.open();
	kinect.initDepthSource();
	//kinect.initColorSource();

	depthImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_GRAYSCALE);
	depthImg.setColor(ofColor::black);
	depthImg.update();

	float sqrResolution = tracker.getResolution();
	sqrResolution *= sqrResolution;
	//minPoints = (int)(0.001*(float)(512 * 424)/sqrResolution);

	camera.setPosition(ofGetWidth() / 4.0, ofGetHeight() / 2.0, 1000.);
	camera.lookAt(ofVec3f(0));

	thresholdNear.set("Threshold Near", 1, 0.1, 5);
	thresholdFar.set("Threshold Far", 2.2, 0.1, 8);

	boxMin.set("boxMin", ofVec3f(-8), ofVec3f(-10), ofVec3f(0));
	boxMax.set("boxMax", ofVec3f(8), ofVec3f(0), ofVec3f(10));
	thresh3D.set("thresh3D", ofVec3f(.2, .2, .3), ofVec3f(0), ofVec3f(1));

	thresh2D.set("thresh2D", 1, 0, 255);
	minVol.set("minVol", 0.02, 0, 10);
	maxVol.set("maxVol", 2, 0, 100);
	minPoints.set("minPoints", 50, 0, 500);
	maxBlobs.set("maxBlobs", 10, 0, 100);
	trackedBlobs.set("trackdBlobs", 0, 0, maxBlobs);

	gui.setup("Settings", "settings.xml");
	gui.add(thresholdNear);
	gui.add(thresholdFar);
	gui.add(boxMin);
	gui.add(boxMax);
	gui.add(thresh3D);
	gui.add(thresh2D);
	gui.add(minVol);
	gui.add(maxVol);
	gui.add(minPoints);
	gui.add(maxBlobs);
	gui.add(trackedBlobs);
	gui.loadFromFile("settings.xml");

	visible = true;
	save = false;

	// Test Hitbox

	hitBoxesManager.setup();

	ofVec3f boxCenter = ofVec3f(0.0f, 0.0f, 0.0f);
	ofPoint boxDimensions = ofPoint(1.5, 1.5, 1.5);
	hitBoxesManager.addHitBoxToWorld(boxCenter, boxDimensions);

	ofAddListener(hitBoxesManager.onHitBoxBecomesActive, this, &ofApp::onHitBoxStateChanged);
	ofAddListener(hitBoxesManager.onHitBoxBecomesInactive, this, &ofApp::onHitBoxStateChanged);

}

//--------------------------------------------------------------
void ofApp::update() {

	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());

	//

	static bool bTrackerInit = false;

	kinect.update();

	// kinect 2 init is async, so just chill till it's ready, OK?
	if (kinect.getDepthSource()->isFrameNew()) {
		if (!bTrackerInit) {
			bTrackerInit = true;
			tracker.init(&kinect, false);
			tracker.setScale(ofVec3f(0.001));
		}

		updateDepthImage();
	}

	if (tracker.isInited()) {
		tracker.findBlobs(&depthImg, boxMin, boxMax, thresh3D, thresh2D, minVol, maxVol, minPoints, maxBlobs);
		trackedBlobs = tracker.nBlobs;
	}

}

//--------------------------------------------------------------
void ofApp::updateDepthImage() {

	if (!depthImg.isAllocated()) {
		depthImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_GRAYSCALE);
	}

	ofShortPixels pix;
	pix.allocate(DEPTH_WIDTH, 1, OF_IMAGE_GRAYSCALE);

	auto& depthPix = kinect.getDepthSource()->getPixels();
	for (int y = 0; y < DEPTH_HEIGHT; y++) {
		for (int x = 0; x < DEPTH_WIDTH; x++) {
			int index = x + (y*DEPTH_WIDTH);

			// Build the 8bit, thresholded image for drawing to screen
			if (depthPix.getWidth() && depthPix.getHeight()) {
				// Multiply thresholds by 1000 because the values are in meters in world
				// space but in mm in the depthPix array
				float depth = depthPix[index];
				float val = depth == 0 ? 0 : ofMap(depth, thresholdNear * 1000, thresholdFar * 1000, 255, 0, true);
				depthImg.setColor(x, y, ofColor(val));
			}
		}
	}

	depthImg.update();

}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(ofColor::white);

	if (tracker.isInited()) {

		depthImg.draw(ofGetWidth() - DEPTH_WIDTH / 2., 0, DEPTH_WIDTH / 2., DEPTH_HEIGHT / 2.);

		glPointSize(2.0);
		ofPushMatrix();
		camera.begin();

		//ofTranslate(ofGetWidth()/2.0, 0.);
		ofScale(100., 100., 100.);
		ofEnableDepthTest();

		// Draw point cloud

		if (drawPointCloud) {

			auto colorSource = kinect.getColorSource();

			//setup some point cloud properties for kicks
			glPushAttrib(GL_POINT_BIT);
			glPointSize(1.0f);
			//glEnable(GL_POINT_SMOOTH);

			ofPushStyle();

			if (colorSource) {
				//bind kinect color camera texture and draw mesh from depth (which has texture coordinates)
				colorSource->getTexture().bind();
			}

			auto opts = ofxKinectForWindows2::Source::Depth::PointCloudOptions(true, ofxKinectForWindows2::Source::Depth::PointCloudOptions::TextureCoordinates::ColorCamera);
			auto mesh = kinect.getDepthSource()->getMesh(opts);

			//draw point cloud
			mesh.drawVertices();

			//draw triangles
			ofSetColor(255, 150);
			//mesh.drawWireframe();

			//draw fills faded
			ofSetColor(255, 50);
			//mesh.drawFaces();

			if (colorSource) {
				//unbind colour camera
				colorSource->getTexture().unbind();
			}

			ofPopStyle();

			//clear the point cloud drawing attributes
			glPopAttrib();

		}

		// Draw hitboxes & puts the state back to normal on every frame:
		hitBoxesManager.drawBoxes();
		//hitBoxesManager.clearBoxesActivationState();

		// draw blobs
		for (unsigned int i = 0; i < tracker.blobs.size(); i++) {

			ofPushMatrix();
			ofColor color;
			color.setSaturation(200);
			color.setBrightness(225);
			color.setHue(ofMap(i, 0, tracker.blobs.size(), 0, 255));

			ofSetColor(color);
			// draw blobs
			tracker.blobs[i].draw();

			ofPopMatrix();

			ofVec3f bbMax = tracker.blobs[i].boundingBoxMax;
			ofVec3f bbMin = tracker.blobs[i].boundingBoxMin;

			ofNoFill();
			ofDrawBox(tracker.blobs[i].centroid, tracker.blobs[i].dimensions.x, tracker.blobs[i].dimensions.y, tracker.blobs[i].dimensions.z);
			ofFill();

		}

		ofSetColor(255);
		ofDisableDepthTest();
		camera.end();
		ofPopMatrix();

		// Check hitboxes status
		hitBoxesManager.checkBlobs(tracker.blobs);

		if (visible) {
			gui.draw();
			hitBoxesManager.drawHitBoxesGui();
		}

	}

}


//--------------------------------------------------------------
void ofApp::onHitBoxStateChanged(int & boxID) {

	ofLog() << "Box " << boxID << " << active: " << hitBoxesManager.getIsHitBoxActive(boxID) << endl;

};

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'g') {
		visible ^= 1;
	}

	if (key == 'p') {
		drawPointCloud ^= 1;
	}


	if (key == 's') {

		hitBoxesManager.saveHitBoxesData();

	}

	if (key == 'l') {

		hitBoxesManager.loadHitboxesSavedData();

	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

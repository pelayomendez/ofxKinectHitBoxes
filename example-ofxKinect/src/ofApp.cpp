#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0);
    ofSetVerticalSync(true);
    
    // Kinect V1
    
    kinect.setRegistration(true);
    
    //kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    kinect.init(false, false); // disable video image (faster fps)
    
    kinect.open();		// opens first available kinect
    //kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
    
    // print the intrinsic IR sensor values
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    depthImg.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);

    float sqrResolution = tracker.getResolution();
    sqrResolution *= sqrResolution;
    //minPoints = (int)(0.001*(float)(512 * 424)/sqrResolution);
    
    camera.setPosition(ofGetWidth() / 4.0, ofGetHeight() / 2.0, 1000.);
    camera.lookAt(ofVec3f(0));
    
    // TODO: Add to GUI
    thresholdNear.set("Threshold Near", 230, 0, depthImg.getPixels().size());
    thresholdFar.set("Threshold Far", 70, 0, depthImg.getPixels().size());
    
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
void ofApp::update(){
    
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
    
    //
    
    static bool bTrackerInit = false;

    kinect.update();

    
    // there is a new frame and we are connected
    if(kinect.isFrameNew()) {
        
        if (!bTrackerInit) {
            bTrackerInit = true;
            tracker.init(&kinect, false);
            tracker.setScale(ofVec3f(0.001));
        }
        
        // load grayscale depth image from the kinect source
        depthImg.setFromPixels(kinect.getDepthPixels());
        
        // or we do it ourselves - show people how they can work with the pixels
        ofPixels & pix = depthImg.getPixels();
        int numPixels = pix.size();
        for(int i = 0; i < numPixels; i++) {
            if(pix[i] < thresholdNear && pix[i] > thresholdFar) {
                pix[i] = 255;
            } else {
                pix[i] = 0;
            }
        }
        
    }
    
    if (tracker.isInited()) {
        tracker.findBlobs(&depthImg, boxMin, boxMax, thresh3D, thresh2D, minVol, maxVol, minPoints, maxBlobs);
        trackedBlobs = tracker.nBlobs;
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(ofColor::white);
    
    if (tracker.isInited()) {
        
        depthImg.draw(ofGetWidth() - depthImg.getWidth() / 2., 0, depthImg.getWidth() / 2., depthImg.getHeight() / 2.);
        
        glPointSize(2.0);
        ofPushMatrix();
        camera.begin();
        
        //ofTranslate(ofGetWidth()/2.0, 0.);
        ofScale(100., 100., 100.);
        ofEnableDepthTest();
        
        // Draw point cloud
        
        if (drawPointCloud) {
            
            // TODO: Draw point cloud for Kinect V1
            
            /*
            camera.begin();
            int w = 640;
            int h = 480;
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_POINTS);
            int step = 2;
            for(int y = 0; y < h; y += step) {
                for(int x = 0; x < w; x += step) {
                    if(kinect.getDistanceAt(x, y) > 0) {
                        mesh.addColor(kinect.getColorAt(x,y));
                        mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                    }
                }
            }
            glPointSize(3);
            ofPushMatrix();
            // the projected points are 'upside down' and 'backwards'
            ofScale(1, -1, -1);
            ofTranslate(0, 0, -1000); // center the points a bit
            ofEnableDepthTest();
            mesh.drawVertices();
            ofDisableDepthTest();
            ofPopMatrix();
            camera.end();
            */
            
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
void ofApp::keyPressed(int key){

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

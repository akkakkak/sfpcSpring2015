#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true); // what does this do?
//    ofSetupScreenPerspective(0, 0, 90);
    
    rotateX = 0;
    rotateY = 1;
    rotateZ = 0;
    tempY = 0;
    
    rotAngle = 0.075;
    keyDown = FALSE; // used to stop Y rotation when rotating on X axis, and restoring it
    
    gridInterval = 20;
    
    counter = 0;
    
    myCam.setPosition( ofGetWidth()/2, ofGetHeight()/2, 0);
    myCam.lookAt(ofPoint(0,0,-500));
    myCam.setFov(10);
    
    bUseCamera = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    counter += rotateY;
    
    for (int wireNum = 0; wireNum < wires.size(); wireNum++) {
        wires[wireNum].update(rotAngle, rotateX, rotateY, rotateZ, counter);
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(127,127,127);
    for (int i = 0; i < (ofGetHeight()/gridInterval) - 1; i++) {
        ofSetLineWidth(1);
        ofLine(0, (i+1)*gridInterval, ofGetWidth(), (i+1)*gridInterval);
    }

    // crosshair (serves as click)
    crosshair(4, 10, ofColor::black, ofColor::red);

    // cursor
    ofCircle(ofGetMouseX(), ofGetMouseY(), 2);

    if (!bUseCamera) {
        ofSetColor(0);
        
        ofEllipse(400, 750, 100, 40);
        
        ofLine(400, 450, 400, 750);
    }
    if (bUseCamera){
        myCam.begin();
    }
    
    for (int wireNum = 0; wireNum < wires.size(); wireNum++) {
        wires[wireNum].draw();
    }
    
    if (bUseCamera){
        myCam.end();
    }
    
    // instructions
    ofDrawBitmapStringHighlight("z<->x to rotate on X axis", ofPoint(20,20));
    ofDrawBitmapStringHighlight("r to reverse playback, s to stop playback", ofPoint(20,40));
    ofDrawBitmapStringHighlight("spacebar to undo", ofPoint(20,60));
}

//--------------------------------------------------------------
void ofApp::crosshair(int size, int time, ofColor color1, ofColor color2){
    
    int beatLength = (360 / rotAngle) / 16;
    int frameCount = ofGetFrameNum();
    
    if (((frameCount+3*beatLength) % (4*beatLength)) < time) size = size;
    if (((frameCount+2*beatLength) % (4*beatLength)) < time) size *= 1.5;
    if (((frameCount+beatLength) % (4*beatLength)) < time) size *= 2;
    if ((frameCount % (4*beatLength)) < time) {
        size *= 2.5;
        ofSetColor(color2);
        ofCircle(ofPoint(ofGetWidth()/2, ofGetHeight()/2), size/2);
    }
    
    if ((frameCount % (beatLength)) < time) {
        ofSetColor(color1);
        ofPoint center = ofPoint(ofGetWidth()/2, ofGetHeight()/2);
        ofLine(center.x-size, center.y, center.x+size, center.y);
        ofLine(center.x, center.y-size, center.x, center.y+size);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    // undo
    if (key == ' ') if (wires.size() > 0) {
        wires.pop_back();
    }
    
    // y rotation control
    if (key == 'r') rotateY *= -1;
    if (key == 's') {
        if (rotateY != 0) {
            tempY = rotateY;
            rotateY = 0;
        } else rotateY = tempY;
    }

    // x rotation
    if (key == 'z' || key == 'x') {
        if (!keyDown) {
            tempY = rotateY;
            keyDown = TRUE;
        }
        rotateY = 0;
        if (key == 'z') rotateX = 1;
        else rotateX = -1;
    }
    
    if (key == 'c'){
        bUseCamera = !bUseCamera;  // ^
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    // x rotation stop
    
    if (key == 'z' || key == 'x') {
        rotateY = tempY;
        rotateX = 0;
        keyDown = FALSE;
    }
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    sonicWire wire;
    wire.cam = &myCam;
    wires.push_back(wire);
    wires.back().setup(rotAngle);
    wires.back().startRec(counter, rotAngle);
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    wires.back().stopRec(counter);
    if (wires.back().noLine()) wires.pop_back();

}
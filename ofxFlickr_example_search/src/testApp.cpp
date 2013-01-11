#include "testApp.h"

// REPLACE THESE WITH YOUR API KEY AND API SECRET
// http://www.flickr.com/services/developer/

string API_KEY = "";
string API_SECRET = "";

//--------------------------------------------------------------
void testApp::setup(){
    bool bAuthenticated = flickrAPI.authenticate( API_KEY, API_SECRET, ofxFlickr::FLICKR_WRITE );
    
    results = flickrAPI.search("openframeworks");
    currentResult = 0;
    lastChanged = ofGetElapsedTimeMillis();
    
    if ( results.size() > 0){
        latestImage.loadImage(results[currentResult].getURL());
    }
}

//--------------------------------------------------------------
void testApp::update(){
    if ( ofGetElapsedTimeMillis() - lastChanged > 1000 ){
        lastChanged = ofGetElapsedTimeMillis();
        currentResult++;
        if ( currentResult >= results.size()){
            currentResult = 0;
        }
        
        if ( results.size() > 0){
            latestImage.loadImage(results[currentResult].getURL());
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if ( latestImage.bAllocated() ){
        latestImage.draw(0,0);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){}

//--------------------------------------------------------------
void testApp::keyReleased(int key){}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){}
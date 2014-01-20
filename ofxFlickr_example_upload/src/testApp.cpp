#include "testApp.h"

// REPLACE THESE WITH YOUR API KEY AND API SECRET
// http://www.flickr.com/services/developer/

string API_KEY = "";
string API_SECRET = "";

//--------------------------------------------------------------
void testApp::setup(){
    bool bAuthenticated = flickrAPI.authenticate( API_KEY, API_SECRET, ofxFlickr::FLICKR_WRITE );
    toLoad = "";
}

//--------------------------------------------------------------
void testApp::update(){
    if ( toLoad != "" ){
        string localCopy = toLoad;
        cout << "load "<<localCopy<<endl;
        latestImage.loadImage(localCopy);
        toLoad = "";
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if ( latestImage.bAllocated() )
        latestImage.draw(0,0);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
#ifdef TARGET_LINUX
	string image = "test.jpg";
	ofImage tester;
    bool bIsImage = tester.loadImage(image);
    if ( bIsImage ){
        
        string photoID = flickrAPI.upload(ofToDataPath(image));
        toLoad = flickrAPI.getMediaById(photoID).getURL();
	ofLog() << toLoad;
    }
#endif
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    ofImage tester;
    bool bIsImage = tester.loadImage(dragInfo.files[0]);
    if ( bIsImage ){
        cout << dragInfo.files[0] << endl;
        string photoID = flickrAPI.upload(dragInfo.files[0]);
        toLoad = flickrAPI.getMediaById(photoID).getURL();
    }
}

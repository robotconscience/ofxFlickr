#include "testApp.h"

// REPLACE THESE WITH YOUR API KEY AND API SECRET
// http://www.flickr.com/services/developer/

string API_KEY = "18d5cc98b35a8ba2996fb97ca97e8806";
string API_SECRET = "5f75e1ab676fd2a3";

//--------------------------------------------------------------
void testApp::setup(){
    bool bAuthenticated = flickrAPI.authenticate( API_KEY, API_SECRET, ofxFlickr::FLICKR_WRITE );
    toLoad = "";
    
    // attach to events
    ofAddListener(ofxFlickr::APIEvent::events, this, &testApp::onFlickrEvent);
    
    // thread!
    flickrAPI.start();
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
    
    // fps counter so we can see the thread!
    ofDrawBitmapString(ofToString(ofGetFrameRate(), 3), 20, 20);
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
void testApp::mousePressed(int x, int y, int button){
#ifdef TARGET_LINUX
	string image = "test.jpg";
	ofImage tester;
    bool bIsImage = tester.loadImage(image);
    if ( bIsImage ){
        
        flickrAPI.uploadThreaded(ofToDataPath(image));
	ofLog() << toLoad;
    }
#endif
}

//--------------------------------------------------------------
void testApp::onFlickrEvent ( ofxFlickr::APIEvent & evt ){
    // we got an image!
    string photoID = evt.resultString;
    
    // make an un-threaded call...
    toLoad = flickrAPI.getMediaById(photoID).getURL();
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
        
        flickrAPI.uploadThreaded(dragInfo.files[0]);
    }
}

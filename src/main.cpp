#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, WINDOW_WIDTH, WINDOW_HEIGHT, OF_WINDOW);			// <-------- setup the GL context
	window.setWindowTitle("NeuroTwitter");
	ofRunApp( new ofApp());
}

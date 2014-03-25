#pragma once

#include "ofMain.h"

#include "tweet3d.h"
#include "ofxTwitter.h"

class ofxTwitter3d : public ofxTwitter{
	
public:
	
	vector<tweet3d> getTweets();
	
private:
	
	//string locationString;    
	ofVec3f location3d;
	
};
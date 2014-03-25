#pragma once

#include "ofMain.h"
#include "ofxTwitter.h"

class tweet3d : public ofxTwitterTweet{
	
public:
	
	void setup();
	void update();
	void draw();
	
	ofVec2f get3dLocation();
	//string get3dVectoStringlocation();

private:
	
	//string locationString;    
	ofVec3f location3d;

};
#ifndef TESTAPP
#define TESTAPP

#define WINDOW_WIDTH 1620
#define WINDOW_HEIGHT 800

#include "ofMain.h"

#include "guiManager.h"
#include "tweetManager.h"
#include "vizManager.h"
#include "ofxSuperLog.h"


class ofApp : public ofBaseApp
{
	public:
	void setup();
	void update();
	void draw();
	void exit(); 

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	guiManager		myGUIManager;
	tweetManager	myTwitterManager;
	vizManager		myVizManager;
	
	bool	drawFill;
	float	red, green, blue, alpha;
        
    ofColor backgroundColor; 
    float	radius;
    int		resolution;
    ofPoint position;
	
	//double click (from easyCam)
	unsigned long lastTap;
	unsigned long doubleclickTime;
	
	// screen Log
	void setupLog();
	
	// Help message
	ofxUICanvas*	helpMessage;
	void 	setupHelpMessage();


};

#endif
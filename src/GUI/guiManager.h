#pragma once

#include "ofMain.h"
//#include "tweetManager.h"
#include "ofxTwitter.h"
#include "ofxUI.h"
#include "guiEvent.h"
#include "myUtils.h"


// Tab
enum tabSelector
{
	SEARCH,
	POST,
	PUBMED
};

class guiManager {
public:

	guiManager();
	~guiManager();
	
	// Canvas (needs public for mouse events)
	ofxUICanvas *tabCanvas;
		
private:
	
	// Load GUIs
	void setupTabBar();
	
	// Events
	void tabCanvasEvent(ofxUIEventArgs &e);
		
	tabSelector currentSelection;
	bool searchToggle;
	bool postToggle;
	
	// tabCanvas
	float tabCanvasX;
	float tabCanvasY;
	float tabCanvasW;
	float tabCanvasH;
	float toggleW;
	float toggleH;

};

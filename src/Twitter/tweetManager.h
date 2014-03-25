#pragma once

#include "ofMain.h"
#include "ofxTwitter.h"
#include "ofxXmlSettings.h"
#include "ofxTwitter3d.h"
#include "guiManager.h"
#include "guiEvent.h"


class tweetManager {
public:
	tweetManager();
	~tweetManager();

	void setup(	string CONSUMER_KEY, string CONSUMER_SECRET);
	ofxTwitter twitterClient;
	
	// Canvas (needs public for mouse events)
	ofxUICanvas *postCanvas;
	ofxUICanvas *textInputCanvas;
	ofxUIScrollableSliderCanvas *scrollCanvas;
	
private:
	
	// events
//	void searchQuery(string s);
	void searchQuery(guiEvent &e);
	void setOneTweetToGui(guiEvent &e);
	void cleanImgUsers( std::vector <ofImage> & a );
	void textInputEvent(ofxUIEventArgs &e);
	void postCanvasEvent(ofxUIEventArgs &e);
	void changeTabBar(guiEvent &e);
	
	// Tweeter objects
	void setupTwitter();
	ofxTwitterTweet tweet;
	void addTwitterContent(ofImage img,
						   string name,
						   string user_name,
						   string tweetText);
	
	// Load GUIs
	void updateSlider();
	void setupScrollCanvas();
	void setupTextInputCanvas();
	void setupPostCanvas();
	
	bool searchBtn;
	bool postBtn;
	
	// tweetsCanvas
	float tweetsCanvasX;
	float tweetsCanvasY;
	float tweetsCanvasW;
	float tweetsCanvasH;
	
	// tabCanvas
	float tabCanvasX;
	float tabCanvasY;
	float tabCanvasW;
	float tabCanvasH;
	float toggleW;
	float toggleH;
	
	// postCanvas
	float postCanvasX;
	float postCanvasY;
	float postCanvasW;
	float postCanvasH;
	float postFieldH;
	float buttonW;
	float buttonH;
	
	// searchCanvas
	float searchCanvasX;
	float searchCanvasY;
	float searchCanvasW;
	float searchCanvasH;
	
	// search field
	float textInputY;
	float searchFieldX;
	float searchFieldY;
	float searchFieldW;
	float searchFieldH;
	
	float space;
	float dim;
	float WidgetW;
	float sliderW;
	
	int	nResponses;
	float nResponsesX;
	float nResponsesY;
	float nResponsesW;

};


// JUNK:
//		int actualTweet;
//		bool reloadTweet;
//		ofImage userImage;
//		vector<ofImage> imgUsers;
//		ofxTwitter3d mytwitter3d;
//		vector<tweet3d> alltweets;
//		vector<tweet3d> selectedTweets;

//		ofxXmlSettings xmltwitter;

//	string screen_name;
//	string myText;
//	string name;

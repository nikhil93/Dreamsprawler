#include "tweetManager.h"

//----------------------------------------------
tweetManager::tweetManager(){

	int lineHeight = 35;
	int totalHeight = 550;
	
	// tabCanvas
	tabCanvasX		= 1220;
	tabCanvasY		= 50;
	tabCanvasW		= 400;
	tabCanvasH		= lineHeight+OFX_UI_GLOBAL_SPACING_HEIGHT*2;
	toggleW			= 100;
	toggleH			= lineHeight;

	// Search field
	searchCanvasX	= tabCanvasX+1;
	searchCanvasY	= tabCanvasY+tabCanvasH-1;
	searchCanvasW	= tabCanvasW;
	searchCanvasH	= lineHeight+OFX_UI_GLOBAL_WIDGET_SPACING*2;
	
	
	searchFieldX	= 0;
	searchFieldY	= lineHeight;
	searchFieldW	= searchCanvasW-OFX_UI_GLOBAL_WIDGET_SPACING*2;
	searchFieldH	= lineHeight;
	
	nResponses		= 10;
	nResponsesX		= searchFieldW+10;
	nResponsesY		= 6;
	nResponsesW		= 50;
	
	// Tweets Canvas
	dim				= 50;
	sliderW			= 20;
	space			= 10;
	WidgetW			= tabCanvasW-sliderW-OFX_UI_GLOBAL_WIDGET_SPACING*2;
	
	tweetsCanvasX	= searchCanvasX;
	tweetsCanvasY	= searchCanvasY+searchCanvasH;
	tweetsCanvasW	= tabCanvasW-sliderW;
	tweetsCanvasH	= totalHeight+tabCanvasH;
	
	// postCanvas
	postCanvasX	= searchCanvasX;
	postCanvasY	= searchCanvasY;
	postCanvasW	= searchCanvasW;
	postCanvasH	= totalHeight+tabCanvasH+searchCanvasH;
	postFieldH	= lineHeight;
	buttonW		= 100;
	buttonH		= lineHeight;

	
	// setup GUIs
	setupTextInputCanvas();
	setupScrollCanvas();
	setupPostCanvas();

	searchBtn=false;
	postBtn=false;
}

//----------------------------------------------
tweetManager::~tweetManager(){
	
	delete postCanvas;
	delete scrollCanvas;
	delete textInputCanvas;

}

//--------------------------------------------------------------
void tweetManager::setup(string CONSUMER_KEY, string CONSUMER_SECRET){
	
    twitterClient.setDiskCache(true);
    twitterClient.setAutoLoadImages(true, false); // Loads images into memory as ofImage;
	
	twitterClient.authorize(CONSUMER_KEY, CONSUMER_SECRET);
	
	// listen to new searches. maybe we will need to place it somewhere else and use removelistener
    ofAddListener(guiEvent::newSearch, this, &tweetManager::searchQuery);

	// listen when tweets are received
	ofAddListener(guiEvent::gotTweet, this, &tweetManager::setOneTweetToGui);

	// listen to tab
	ofAddListener(guiEvent::tabSelected, this, &tweetManager::changeTabBar);

	// start with searh open
	static guiEvent args;
	args.message	= "Search";
	ofNotifyEvent(guiEvent::tabSelected, args);
}

//--------------------------------------------------------------
void tweetManager::searchQuery(guiEvent &e) {
	twitterClient.startQuery(e.message);
	ofLogVerbose("tweetManager ") << "query = " << e.message;
}

//--------------------------------------------------------------
void tweetManager::cleanImgUsers( std::vector <ofImage> & a ) {
    a.clear(); 
}

//--------------------------------------------------------------
void tweetManager::setOneTweetToGui(guiEvent &e){
		
	int i = e.value;
	tweet = twitterClient.getTweetByIndex(i);
	
	ofLogVerbose("tweetManager ") << "Start tweet ************************************************";
	ofLogVerbose("tweetManager ") << "tweet.user.screen_name = " << tweet.user.screen_name;
	ofLogVerbose("tweetManager ") << "tweet.user.name = " << tweet.user.name;
	ofLogVerbose("tweetManager ") << "tweetText = " << tweet.text;

	/* here we will check if the tweet has brain coordinates*/
	if ( true ) {
		
		if(tweet.isProfileImageLoaded()) {
			addTwitterContent(tweet.user.profile_image,
							  tweet.user.name,
							  tweet.user.screen_name,
							  tweet.text);

		}		
	}
}


//--------------------------------------------------------------
void tweetManager::setupPostCanvas(){
    
	postCanvas = new ofxUICanvas(postCanvasX, postCanvasY,postCanvasW, postCanvasH);
	postCanvas->setVisible(false);
	
	postCanvas->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
	
	postCanvas->addTextInput("PostField",
							 "Type here",
							 searchFieldW,
							 postFieldH,
							 searchFieldX,
							 searchFieldY)->setAutoClear(true);
	
	postCanvas->addTextInput("URLfield",
							 "URL here",
							 searchFieldW,
							 postFieldH,
							 searchFieldX,
							 0)->setAutoClear(true);
	
	postCanvas->addWidgetDown(new ofxUILabelButton("Post",
												   postBtn,
												   buttonW,
												   buttonH,
												   buttonW,
												   0,
												   OFX_UI_FONT_MEDIUM,
												   false));
	
	ofAddListener(postCanvas->newGUIEvent,this,&tweetManager::postCanvasEvent);
}

//--------------------------------------------------------------
void tweetManager::setupTextInputCanvas(){
	
	
	textInputCanvas = new ofxUICanvas(searchCanvasX, searchCanvasY,searchCanvasW, searchCanvasH);
	textInputCanvas->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
	textInputCanvas->setVisible(false);
	
	// (string _name, string _textstring, float w, float h, float x, float y, int _size)
	textInputCanvas->addTextInput("searchField", "Type here", searchFieldW, searchFieldH, searchFieldX, searchFieldY)->setAutoClear(true);
	//	textInputCanvas->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
	
	// ofxUINumberDialer(float x, float y, float _min, float _max, float _value, int _precision, string _name, int _size)
	/*	textInputCanvas->addWidget( new ofxUINumberDialer(nResponsesX,
	 nResponsesY,
	 0,
	 200,
	 nResponses,
	 0,
	 "responses",
	 OFX_UI_FONT_MEDIUM));
	 */
	ofAddListener(textInputCanvas->newGUIEvent,this,&tweetManager::textInputEvent);
}

//--------------------------------------------------------------
void tweetManager::setupScrollCanvas()
{
	// Canvas for Tweets
	scrollCanvas = new ofxUIScrollableSliderCanvas(tweetsCanvasX, tweetsCanvasY, tweetsCanvasW, tweetsCanvasH);
	scrollCanvas->setWidgetSpacing(space);
	scrollCanvas->enableScrollBar();
//	scrollCanvas->enableFBO();
	scrollCanvas->setVisible(false);
}

void tweetManager::addTwitterContent(ofImage img, string name, string user_name, std::string tweetText)
{
	// clean emojis:
	name = removeEmojis(name);
	user_name = removeEmojis(user_name);
	tweetText = removeEmojis(tweetText);
	
	float lineHeight = scrollCanvas->getFont()->getLineHeight();
	//(float x, float y, float w, float h, ofImage _image, string _name, bool _showLabel);
	scrollCanvas->addWidgetDown( new ofxUIImage( 0, 0, dim, dim, img, "IMAGE", false));
	
	//(string _name, string _textstring, float w, float h = 0, float x = 0, float y = 0, int _size = OFX_UI_FONT_MEDIUM);
	scrollCanvas->addWidgetRight( new ofxUITextArea("NAME",
													name,
													(WidgetW - dim)/2,
													0,
													0,
													dim*-1. - space,
													OFX_UI_FONT_MEDIUM ),
								 OFX_UI_ALIGN_FREE, false)->setDrawBack(false);
	
	scrollCanvas->addWidgetDown( new ofxUITextArea("USER",
												   "@"+user_name,
												   (WidgetW - dim)/2,		//w
												   0,						//h
												   dim + space*2,			//x
												   0,						//y
												   OFX_UI_FONT_SMALL ),
								OFX_UI_ALIGN_FREE, false)->setDrawBack(false);
	
	scrollCanvas->addWidgetDown( new ofxUITextArea("TEXT",
												   tweetText,
												   WidgetW-space,			//w
												   0,						//h
												   space,					//x
												   lineHeight*-1.9+dim,		//y <- this has to be fixed
												   OFX_UI_FONT_MEDIUM ),
								OFX_UI_ALIGN_FREE, false)->setDrawBack(false);
	
	scrollCanvas->addSpacer( WidgetW -space, 1 );
	scrollCanvas->autoSizeToFitWidgets();
}



//--------------------------------------------------------------
void tweetManager::changeTabBar(guiEvent &e)
{
	if (e.message == "Search")
	{
		postCanvas->setVisible(false);
        ofRemoveListener(postCanvas->newGUIEvent,this,&tweetManager::postCanvasEvent);
		ofLogVerbose("TweetManager") << "RemoveListener: postCanvasEvent";

		scrollCanvas->setVisible(true);
		textInputCanvas->setVisible(true);
		ofAddListener(textInputCanvas->newGUIEvent,this,&tweetManager::textInputEvent);
		ofLogVerbose("TweetManager") << "ofAddListener: textInputEvent";
	}
	else if(e.message == "Post")
	{
		scrollCanvas->setVisible(false);
		textInputCanvas->setVisible(false);
        ofRemoveListener(postCanvas->newGUIEvent,this,&tweetManager::textInputEvent);
		ofLogVerbose("TweetManager") << "RemoveListener: textInputEvent";

		postCanvas->setVisible(true);
		ofAddListener(postCanvas->newGUIEvent,this,&tweetManager::postCanvasEvent);
		ofLogVerbose("TweetManager") << "ofAddListener: postCanvasEvent";
		
	}
}


//--------------------------------------------------------------
void tweetManager::textInputEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	
	if(name == "searchField")
	{
		ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
		if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
		{
			ofLogVerbose("TweetManager") << "ON ENTER: ";
			//ofUnregisterKeyEvents((guiManager*)this);
			
			//send text to Twitter
			static guiEvent newEvent;
			newEvent.message =  textinput->getTextString();
			newEvent.value	= nResponses;
			ofNotifyEvent(guiEvent::newSearch, newEvent);
			
			//copy text to Clipboard
//			bool succeded = ofCopyText(textinput->getTextString());
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
		{
			ofLogVerbose("TweetManager") << "ON FOCUS: ";
			//			textinput->
			//			textinput->recalculateDisplayString();
			//			ofRegisterKeyEvents((guiManager*)this);
			if (textinput->getTextString()=="Type here")
			textinput->setTextString("");
			textInputCanvas->hasKeyboardFocus();
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
		{
			ofLogVerbose("TweetManager") << "ON BLUR: ";
			
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_LOAD)
		{
			ofLogVerbose("TweetManager") << "ON LOAD: ";
		}
		ofLogVerbose("TweetManager") << textinput->getInputTriggerType();
		string output = textinput->getTextString();
		ofLogVerbose("TweetManager") << output;

	}
	else if (name == "responses")
	{
		ofxUINumberDialer *numDialer = (ofxUINumberDialer *) e.widget;
		nResponses = numDialer->getValue();
		ofLogVerbose("TweetManager") << "nResponses: " << nResponses;
	}
}

//--------------------------------------------------------------
void tweetManager::postCanvasEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
	
	if(name == "PostField")
	{
		ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
		if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
		{
			ofLogVerbose("TweetManager") << "ON ENTER: ";
			//ofUnregisterKeyEvents((guiManager*)this);
			
			//send text to Twitter
			static guiEvent newEvent;
			newEvent.message =  textinput->getTextString();
			newEvent.value	= nResponses;
			ofNotifyEvent(guiEvent::newSearch, newEvent);
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
		{
			ofLogVerbose("TweetManager") << "ON FOCUS: ";
			if (textinput->getTextString()=="Type here")
				textinput->setTextString("");
			textInputCanvas->hasKeyboardFocus();
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
		{
			ofLogVerbose("TweetManager") << "ON BLUR: ";
			
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_LOAD)
		{
			ofLogVerbose("TweetManager") << "ON LOAD: ";
		}
		ofLogVerbose("TweetManager") << textinput->getInputTriggerType();
		string output = textinput->getTextString();
		ofLogVerbose("TweetManager") << output;
		
	}
	else if(name == "URLfield")
	{
		ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
		if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
		{
			ofLogVerbose("TweetManager") << "ON ENTER: ";
			//ofUnregisterKeyEvents((guiManager*)this);
			
			//send text to Twitter
			static guiEvent newEvent;
			newEvent.message =  textinput->getTextString();
			newEvent.value	= nResponses;
//			ofNotifyEvent(guiEvent::newSearch, newEvent);
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
		{
			ofLogVerbose("TweetManager") << "ON FOCUS: ";
			if (textinput->getTextString()=="URL here")
				textinput->setTextString("");
			textInputCanvas->hasKeyboardFocus();
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
		{
			ofLogVerbose("TweetManager") << "ON BLUR: ";
			
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_LOAD)
		{
			ofLogVerbose("TweetManager") << "ON LOAD: ";
		}
		ofLogVerbose("TweetManager") << textinput->getInputTriggerType();
		string output = textinput->getTextString();
		ofLogVerbose("TweetManager") << output;
		
	}
	else if(name == "Post")
	{
		ofxUILabelButton *btn = (ofxUILabelButton *) e.widget;
/*		if(btn->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
		{
			ofLogVerbose("TweetManager") << "ON ENTER: ";
			//ofUnregisterKeyEvents((guiManager*)this);
			
			//send text to Twitter
			static guiEvent newEvent;
			newEvent.message =  textinput->getTextString();
			newEvent.value	= nResponses;
			//			ofNotifyEvent(guiEvent::newSearch, newEvent);
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
		{
			ofLogVerbose("TweetManager") << "ON FOCUS: ";
			if (textinput->getTextString()=="URL here")
				textinput->setTextString("");
			textInputCanvas->hasKeyboardFocus();
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
		{
			ofLogVerbose("TweetManager") << "ON BLUR: ";
			
		}
		else if(textinput->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_LOAD)
		{
			ofLogVerbose("TweetManager") << "ON LOAD: ";
		}
*/
	}
	
	
}


#include "guiManager.h"

guiManager::guiManager()
{
	int lineHeight = 35;
	
	// tabCanvas
	tabCanvasX		= 1220;
	tabCanvasY		= 50;
	tabCanvasW		= 400;
	tabCanvasH		= lineHeight+OFX_UI_GLOBAL_SPACING_HEIGHT*2;
	toggleW			= 100;
	toggleH			= lineHeight;
	
	// setup UIs
	currentSelection = SEARCH;
	setupTabBar();
}

//----------------------------------------------
guiManager::~guiManager()
{
	delete tabCanvas;
	//Check number of Images createrd and delete them here
}

//--------------------------------------------------------------
void guiManager::setupTabBar()
{
	tabCanvas = new ofxUICanvas(tabCanvasX, tabCanvasY,tabCanvasW, tabCanvasH);
	tabCanvas->setDrawBack(false);
	tabCanvas->setWidgetSpacing(1);
	tabCanvas->setWidgetFontSize(OFX_UI_FONT_MEDIUM);

	
	//	(string _name, bool _value, float w, float h, float x, float y, bool _justifyLeft)
	tabCanvas->addLabelToggle("Search", searchToggle, toggleW, toggleH, 0, 0, false);
	tabCanvas->setWidgetSpacing(10);
	
	tabCanvas->addWidgetRight(new ofxUILabelToggle("Post",
												   postToggle,
												   toggleW,
												   toggleH,
												   toggleW,
												   0,
												   OFX_UI_FONT_MEDIUM,
												   false));

	// set properties
	ofxUILabelToggle *w;
	w = (ofxUILabelToggle *)  tabCanvas->getWidget("Search");
	ofColor back = ofColor(0,0,0,55);		// OFX_UI_COLOR_PADDED / 2
	w->setColorFill(back);
	w->setColorFillHighlight(back);
	w->setColorBack(back);
	w->setValue(true);						// we will use XML settings for this
	
	w = (ofxUILabelToggle *)  tabCanvas->getWidget("Post");
	w->setColorFill(back);
	w->setColorFillHighlight(back);
	w->setColorBack(back);
	w->setValue(false);						// we will use XML settings for this

	ofAddListener(tabCanvas->newGUIEvent,this,&guiManager::tabCanvasEvent);
}

//--------------------------------------------------------------
void guiManager::tabCanvasEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
	
	if(name == "Search")
	{
		currentSelection=SEARCH;

		searchToggle = true;
		postToggle=false;
		
		toggle = (ofxUILabelToggle *)  tabCanvas->getWidget("Search");
		toggle->setValue(searchToggle);

		toggle = (ofxUILabelToggle *)  tabCanvas->getWidget("Post");
		toggle->setValue(postToggle);
		
		// notify tab selected
		static guiEvent args;
		args.message	= "Search";
		ofNotifyEvent(guiEvent::tabSelected, args);
		
		ofLogVerbose("tabCanvasEvent") << "searchToggle: " << toggle->getValue();

	}
	else if (name == "Post")
	{
		currentSelection=POST;
		
		postToggle=true;
		searchToggle = false;
		
		toggle = (ofxUILabelToggle *)  tabCanvas->getWidget("Search");
		toggle->setValue(searchToggle);
		
		toggle = (ofxUILabelToggle *)  tabCanvas->getWidget("Post");
		toggle->setValue(postToggle);
		
		// notify tab selected
		static guiEvent args;
		args.message	= "Post";
		ofNotifyEvent(guiEvent::tabSelected, args);

		ofLogVerbose("tabCanvasEvent") << "postToggle: " << toggle->getValue();
	}
}





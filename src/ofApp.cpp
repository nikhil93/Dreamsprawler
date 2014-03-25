#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetFrameRate(30);
	
	// Setup log
	setupLog();
	
	// Help message
	setupHelpMessage();
	
	// Twitter keys
	string const CONSUMER_KEY = "zSrKv91OmRK1F2wgqXpvQ";
	string const CONSUMER_SECRET = "vUMkjJE70B4xC4nWpMxtScgZYjqzJsceGUbyE3iQ";
	myTwitterManager.setup(CONSUMER_KEY, CONSUMER_SECRET);
	myVizManager.setup();
}

//--------------------------------------------------------------
void ofApp::update()
{
}

//--------------------------------------------------------------
void ofApp::draw()
{
//	ofBackground(150);
	ofBackgroundGradient(100,20);
	myVizManager.draw();

}

//--------------------------------------------------------------
void ofApp::exit()
{

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	myVizManager.keyPressed(key);
    switch(key)
    {
		case 'h':
			helpMessage->isEnabled() ?	helpMessage->disable(): helpMessage->enable();
			break;
		case 'l':
			myTwitterManager.twitterClient.loadCacheFile();
			break;
		case 'f':
			myTwitterManager.scrollCanvas->toggleFBO();
			break;
		case 's':
			myTwitterManager.scrollCanvas->toggleScrollBar();
			break;
		case OF_KEY_F1:
			ofSetLogLevel(OF_LOG_SILENT);
			break;
		case OF_KEY_F2:
			ofSetLogLevel(OF_LOG_VERBOSE);
			break;
		case OF_KEY_F3:
			ofSetLogLevel(OF_LOG_NOTICE);
			break;
		case OF_KEY_F4:
			ofLog(OF_LOG_VERBOSE, "PubMed");
			ofSetLogLevel("PubMed", OF_LOG_VERBOSE);
			break;
		case OF_KEY_F5:
			ofLog(OF_LOG_VERBOSE, "Twitter");
			ofSetLogLevel("Twitter", OF_LOG_VERBOSE);
			break;
		case OF_KEY_F6:
			ofLog(OF_LOG_VERBOSE, "Vis");
			ofSetLogLevel("Vis", OF_LOG_VERBOSE);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{
	
	if (
		myVizManager.guiSliders->isHit(x, y)	||
		myVizManager.guiVolume->isHit(x, y)		||
		myGUIManager.tabCanvas->isHit(x, y)		||
		myTwitterManager.postCanvas->isHit(x, y)	||
		myTwitterManager.textInputCanvas->isHit(x, y)||
		myTwitterManager.scrollCanvas->isHit(x, y)
		)
	{
		myVizManager.cam.disableMouseInput();
		myVizManager.bSelecting=false;
	}
	else
	{
		if (!myVizManager.bSelecting)
		myVizManager.cam.enableMouseInput();
	}

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	/*
	// this is the default on windows os
	doubleclickTime = 200;
	unsigned long curTap = ofGetElapsedTimeMillis();
	if(lastTap != 0 && curTap - lastTap < doubleclickTime){
//		myVizManager.doubleclick(x, y);
	}
	lastTap = curTap;
	 */
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	ofSetWindowShape( WINDOW_WIDTH, WINDOW_HEIGHT);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

//--------------------------------------------------------------
void ofApp::setupLog()
{
	// OF Log Level
		ofSetLogLevel(OF_LOG_VERBOSE);
	//	ofSetLogLevel(OF_LOG_NOTICE);
	//	ofSetLogLevel(OF_LOG_WARNING);
	//	ofSetLogLevel(OF_LOG_ERROR);
	
	// Log console
	bool logToConsole = true;
	bool logToScreen = false;
	ofSetLoggerChannel(ofxSuperLog::getLogger(logToConsole, logToScreen, "logs"));
}




//--------------------------------------------------------------
void ofApp::setupHelpMessage()
{
	// Screen Messages
	const char *text =
    "Shortcuts:                    \n\
                                   \n\
    space     select/move          \n\
    r         rotate volume        \n\
    r         rotate volume        \n\
    esc       to quit              \n\
                                   \n\
    F1:      Log SILENT            \n\
    F2:      Log VERBOSE           \n\
    F3:      Log NOTICE            \n\
    F4:      Log PubMed            \n\
    F5:      Log Twitter           \n\
    F6:      Log Visualization     \n";

	int w= 300;
	int h= 500;
	helpMessage = new ofxUICanvas(0,0, w,h);
//	helpMessage->setWidgetFontSize(OFX_UI_FONT_LARGE);
	helpMessage->addLabel("Help");
//	helpMessage->setWidgetFontSize(OFX_UI_FONT_MEDIUM);
	helpMessage->addSpacer();
	helpMessage->addTextArea("HelpMessage", text);
	helpMessage->autoSizeToFitWidgets();

	int x= ofGetWidth()/2	-		helpMessage->getRect()->getWidth()/2;
	int y= ofGetHeight()/2	-		helpMessage->getRect()->getHeight()/2;
	
	helpMessage->setPosition( x, y);
	helpMessage->setVisible(false);
}













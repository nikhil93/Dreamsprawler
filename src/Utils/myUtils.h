#pragma once
#include "ofMain.h"

#ifdef TARGET_WIN32
	bool setTextToPasteboard(string text);
	string getTextFromPasteboard();
	
#else
	static OSStatus setTextToPasteboard(char* byteArrayIndex);
	string getTextFromPasteboard();
	
#endif
	bool ofCopyText(string text);
	string ofPasteText();

	string removeEmojis(string s);

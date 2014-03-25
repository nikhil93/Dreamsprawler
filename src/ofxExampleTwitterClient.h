// =============================================================================
//
// Copyright (c) 2010-2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include "ofxOAuth.h"


class ofxExampleTwitterClient: public ofxOAuth
{
public:
    ofxExampleTwitterClient()
    {
    }

    virtual ~ofxExampleTwitterClient()
    {
    }
    
    void setup(const std::string& consumerKey,
               const std::string& consumerSecret)
    {
        ofxOAuth::setup("https://api.twitter.com",
                        consumerKey,
                        consumerSecret);
    }

	// Once setup is called, authenticated calls can be made.
    // This method is just an example of whatyour calls might look like.
	// this call
    std::string exampleMethod()
    {
        return get("/1.1/statuses/retweets_of_me.json");
    }
	
	// Returns the 20 most recent mentions (tweets containing a users's @screen_name) for the authenticating user
	// The timeline returned is the equivalent of the one seen when you view your mentions on twitter.com.
	// This method can only return up to 800 tweets.
	// https://dev.twitter.com/docs/api/1.1/get/statuses/mentions_timeline

	std::string getMentions(){
		return get("/1.1/statuses/mentions_timeline.json");
	}

	// https://dev.twitter.com/docs/api/1.1/get/search/tweets
	std::string getHashTag(std::string& query,
						   std::string& count
						   ){

		const std::string prepend = "q=%23";				//q=#
		query.insert (0, prepend);							//q=#query
		query.append("&count="+count);						//q=#query&count=X
		return get("/1.1/search/tweets.json?"+ query);		///1.1/search/tweets.json?q=#query&count=X
	}
	
	// A list of the available calls to the twitter API is here:
	// https://dev.twitter.com/docs/api/1.1

	//************* My Methods ******************//
	
	std::string postTweet(std::string& query,
						  std::string& hashtag
						  ){
	
		const std::string prepend = "status=";
		query.insert (0, prepend);
		query.append("%20%23"+hashtag);
		//    std::string post(const std::string& uri,const std::string& queryParams = "");
		return post("/1.1/statuses/update.json", query);
	}
	
	//https://dev.twitter.com/docs/api/1.1/post/statuses/update
};









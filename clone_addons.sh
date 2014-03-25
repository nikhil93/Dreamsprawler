#!/bin/bash

# Dependencies for tweets_in_slider

cd ../../../addons

if [ -z $1 ]; then
    PREFIX="git clone https://github.com/"
else
    PREFIX="git clone git@github.com:"
fi

${PREFIX}wasawi/ofxTwitter.git

${PREFIX}jefftimesten/ofxJSON.git

${PREFIX}carlesgutierrez/ofxUI.git

${PREFIX}wasawi/ofxTwitter.git

${PREFIX}wasawi/ofxVolumetrics

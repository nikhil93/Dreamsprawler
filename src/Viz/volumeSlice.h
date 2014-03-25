/*  VolumeSlice - draw volumetric data on a 2d plane
 */

#pragma once
#include "ofxTexture3d.h"

//Volume Cutting Viepoint
enum viewPoint
{
	CORONAL,	//	collar
	SAGITTAL,	//	cresta
	AXIAL		//	diadema
};

class volumeSlice : public  ofxTexture3d, ofTexture{
public:
	
	volumeSlice();
    virtual ~volumeSlice();
	void setup(unsigned char * data, int w, int h, int d, float bW, float bH);
	void redraw(viewPoint vP, int depth);

	int getVoxelValue();
	int getVoxelNumber();
	ofVec3f getVoxelCoordinates(int _index);
	bool getVoxelCoordAndVal(int _index, ofVec3f& _coord, int& _val);
	
	void draw(viewPoint vP);
	
protected:
private:
	void drawBox();
	void redrawSagittal();
	void redrawAxial();
	void redrawCoronal();

	unsigned char * myData;
	ofPixels myPixels;
//	viewPoint myViewPoint;
	int volWidth, volHeight, volDepth;
	int renderWidth, renderHeight;
	
	int halfH;
	int halfW;
	int halfD;
	
	float boxW;
	float boxH;

	// the depth of the current slice
	int coronalS;
	int sagittalS;
	int axialS;

	// one image for eaxh point of view
	ofImage coronal;
	ofImage sagittal;
	ofImage axial;
	
	// one pixel array for each image
	ofPixels coronalPixels;
	ofPixels sagittalPixels;
	ofPixels axialPixels;
	
	ofColor lineColor;
	
	bool insideCoronal;
	bool insideSagittal;
	bool insideAxial;
	
	//	void drawSagittal(float x, float y, float z);
	//	void drawAxial(float x, float y, float z);
	//	void drawCoronal(float x, float y, float z);
};
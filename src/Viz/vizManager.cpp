#include "vizManager.h"


//----------------------------------------------
vizManager::vizManager(){

//	ofAddListener(ofEvents().mousePressed, this, &vizManager::mousePressed);
}

//----------------------------------------------
vizManager::~vizManager()
{
	delete guiVolume;
	delete guiSliders;
}

//--------------------------------------------------------------
void vizManager::setup()
{
	// Talairach Atlas
	talairachAtlas.setup("brainData/TalairachAtlas.txt");
	
//	talClient.setup("../../../data/brainData/talairach.jar");
	outputLabels.resize(10);
	bSelecting = false;
	
	// 2Dpad Canvas
	initX	=0;
	initY	=50;
	sliderW = 20;
	dist	= 20;
	boxW	= boxH = 200;
	
	// coordinates in UIs
	uiRange	= ofVec3f(-1,1,0);
	uiCoord	= ofVec3f(0);
		
	// initialize Talairach coordinates
	talOffset	= ofVec3f(-70,-102,-42);
	talDrawX	= boxW+dist*3+sliderW;
	talDrawY	= initY+dist;
	
	//intersection plane
	rayPlane.setCenter(ofVec3f(0));
	rayPlane.setScale(ofVec3f(1));
	rayPlane.setNormal(ofVec3f(0.0f, 0.0f, 1.0f));
	rayPlane.setUp(ofVec3f(0.0f, 1.0f, 0.0f));
	rayPlane.setInfinite(false);
	
	//Volume
	voxelSize	= ofVec3f(1);
	volPos		= ofVec3f(0);
	volSize		= ofVec3f(0);
	volOffset	= ofVec3f(0);
	
	// Init Volume
	initVolume();

	// Init Slices
	volume2D.setup(volumeData, volWidth, volHeight, volDepth, boxW, boxH);

	//setup GUIs
	setup_guis();

	//camera
	ofxLoadCamera(cam, "GUI/cameraSettings.txt");
	
	// pint cloud
	createPointCloud();
}

//--------------------------------------------------------------
void vizManager::initVolume()
{
	imageSequence.init("volumes/Colin27T1_tight/IM-0001-0",3,".tif", 0);
//	imageSequence.init("volumes/talairach_nii/IM-0001-0",3,".tif", 0);
	
	// calculate volume size
	volWidth	= imageSequence.getWidth();
    volHeight	= imageSequence.getHeight();
    volDepth	= imageSequence.getSequenceLength();
	float size	= ofGetHeight();
	ofVec3f volumeSize = voxelSize * ofVec3f(volWidth,volHeight,volDepth);
    float maxDim = max(max(volumeSize.x, volumeSize.y), volumeSize.z);
    volSize = volumeSize * size / maxDim;

	
	ofLogNotice("vizManager") << "setting up volume data buffer at " << volWidth << "x" << volHeight << "x" << volDepth;
    volumeData = new unsigned char[volWidth*volHeight*volDepth];
	
	//fill out the array pixel in white for easy debugging
	for (int i=0; i<volWidth*volHeight*volDepth; i++ )
	{
		volumeData[i]= (unsigned char) 255;
	}
	
	// fill my array with pixels
    for(int z=0; z<volDepth; z++)
    {
        imageSequence.loadFrame(z);
		int gradient = 0;
		for(int y=0; y<volHeight; y++)
        {
			for(int x=0; x<volWidth; x++)
			{
				if (x<volWidth && y<volHeight)
				{																// get values from image
					int i = ((x + volWidth*y) + z*volWidth*volHeight);			// the pointer position at Array
					int sample = imageSequence.getPixels()[x+y*volWidth];		// the pixel on the image
					volumeData[i] = sample;
//					ofLogVerbose("vizManager") << sample << " ";
				}
            }
        }
    }

	// Init Volume
    myVolume.setup(volWidth, volHeight, volDepth, voxelSize, true);
	myVolume.updateVolumeData(volumeData, volWidth, volHeight, volDepth, 0, 0, 0);
    myVolume.setRenderSettings(FBOq, Zq, density, thresh);
	myVolume.setVolumeTextureFilterMode(GL_LINEAR);
	myVolume.setSlices(&uiClamp);
	myVolume.setRayPlane(&rayPlane);
}

//--------------------------------------------------------------
void vizManager::update()
{

	updateCoordinates();
	updateSlicesImage();
	updateSlices2Volume();
	
	updateTalCoords();
	updateTalAtlasLabel();
//	updateTalLabel();

}

//--------------------------------------------------------------
void vizManager::updateTalLabel()
{
	outputLabels = talClient.get(talCoord);
	for (int i=2; i<outputLabels.size(); i++)
	{
		ofLogVerbose("vizManager") << outputLabels[i];
	}
}

//--------------------------------------------------------------
void vizManager::updateTalAtlasLabel()
{
	voxelValue = volume2D.getVoxelValue();
	voxelNumber = volume2D.getVoxelNumber();
	//mapping from pixel value to index value on the Talairach Atlas
	int mapValue= ofMap(voxelValue, 0, 255, 0, 1105);
	outputLabels [2] = talairachAtlas.getHemisphere(mapValue);
	outputLabels [3] = talairachAtlas.getLobe(mapValue);
	outputLabels [4] = talairachAtlas.getGyrus(mapValue);
	outputLabels [5] = talairachAtlas.getTissueType(mapValue);
	outputLabels [6] = talairachAtlas.getCellType(mapValue);
}

//--------------------------------------------------------------
void vizManager::createPointCloud()
{
//	words.setup("brainData/output.txt");
	points.clear();
	
	ofVec3f position = ofVec3f(0.0f,0.0f,0.0f);
	int row=0;
	int page=0;
	int index=0;
	
	for(int z=0; z<volDepth; z++){
		for(int y=0; y<volHeight; y++){
			for(int x=0; x<volWidth; x++){


//				cout << position<< endl;
				
				row = y*volWidth;
				page = z*volWidth*volHeight;
				index = x + row + page;
				
				int _val = volumeData[index];
				int mapValue= ofMap(_val, 0, 255, 0, 1105);
				if (talairachAtlas.getTissueType(mapValue) == "Gray Matter" && _val<2){
//					cout << position;
					position = ofVec3f(x, y, z);
					voxelToVector(position);
//					position*=ofRandom(1.0, 1.05);
//					position.normalize();
//					position *= 700;
					ofColor random (ofRandom(25),ofRandom(255),ofRandom(255),50);
					points.addColor(random);
					points.addVertex(position);
					pointsWhite.addVertex(position);
//					pointClud.push_back(position);
				}
			}
		}
	}
}


//--------------------------------------------------------------
void vizManager::voxelToVector(ofVec3f& voxel)
{
	// transform current selected voxel coordinates
	// to normalised vector
	
	ofVec3f vector;
	/*
	vector.x=voxel.x/volWidth;
	vector.y=voxel.y/volHeight;
	vector.z=voxel.z/volDepth;
*/
	vector.x= ofMap(voxel.x, 0, volWidth,	0, 1.0, false);
	vector.y= ofMap(voxel.y, 0, volHeight,	0, 1.0, false);
	vector.z= ofMap(voxel.z, 0, volDepth,	0, 1.0, false);
	 
	voxel = vector;
}


//--------------------------------------------------------------
void vizManager::updateTalCoords()
{
	// transform my volume coordinates to Talairach coordinates
	// using the offstet provided in nifti headers
	talCoord = volCoord + talOffset;
	talCoord.x *=-1;
//	ofLogVerbose("vizManager") <<	"tal.x " << ofToString(talCoord.x,2);
//	ofLogVerbose("vizManager") <<	"tal.y " << ofToString(talCoord.y,2);
//	ofLogVerbose("vizManager") <<	"tal.z " << ofToString(talCoord.z,2);
}

//--------------------------------------------------------------
void vizManager::updateCoordinates()
{
	// get the distance between the box and the volume
	float halfW = (boxW - volWidth) /2;
	float halfH = (boxW - volHeight) /2;
	float halfD = (boxW - volDepth) /2;
	
	ofLogVerbose("vizManager") <<	"uiCoord.x " << ofToString(uiCoord.x,2);
	ofLogVerbose("vizManager") <<	"uiCoord.y " << ofToString(uiCoord.y,2);
	ofLogVerbose("vizManager") <<	"uiCoord.z " << ofToString(uiCoord.z,2);

	// it would be nice to do it like this:
	// volCoord = floor(ofMap(uiCoord, uiRangeMin, uiRangeMax, 0, volSize));
	// all using vec3f and mapped all at once.
	volCoord.z = floor(ofMap(uiCoord.z, uiRange.x, uiRange.y, 0 -halfH, volHeight + halfH));
	volCoord.x = floor(ofMap(uiCoord.x, uiRange.x, uiRange.y, 0 -halfW, volWidth + halfW));
	volCoord.y = floor(ofMap(uiCoord.y, uiRange.y, uiRange.x, 0 -halfD, volDepth + halfD));

	ofLogVerbose("vizManager") <<	"volCoord.x " << volCoord.x;
	ofLogVerbose("vizManager") <<	"volCoord.y " << volCoord.y;
	ofLogVerbose("vizManager") <<	"volCoord.z " << volCoord.z;
}

//--------------------------------------------------------------
void vizManager::updateSlicesImage()
{
	// update de Depth of the slices drawn by volumeSlice
	// this uptade must only run when there is a gui event
	volume2D.redraw(CORONAL, volCoord.z);
	volume2D.redraw(SAGITTAL, volCoord.x);
	volume2D.redraw(AXIAL, volCoord.y);
}

//--------------------------------------------------------------
void vizManager::updateSlices2Volume()
{
	// transform current selected normalised vector
	// to voxel coordinates

	//ofClamp(volCoord.z,0, volDepth-1);
	ofVec3f uiCoord_;

	// map it to pixel values;
	float uiSize=100;
	uiCoord_ = uiCoord*uiSize;

	//use the pixel positions that are only inside the volume
	//(excluding areas between volume and uiPad limits)

	// coronal
	uiCoord_.z= ofMap(uiCoord_.z, -volHeight/2, volHeight/2, -uiSize, uiSize);
	// sagittal
	uiCoord_.x= ofMap(uiCoord_.x, -volWidth/2, volWidth/2, -uiSize, uiSize);
	// axial
	uiCoord_.y= ofMap(uiCoord_.y, -volDepth/2, volDepth/2, -uiSize, uiSize);
	
	//normalized again
	uiCoord_ = uiCoord_/100;
	
	//clamp the mapped values
	uiCoord_.x = ofClamp(uiCoord_.x, -1, 1);
	uiCoord_.y = ofClamp(uiCoord_.y, -1, 1);
	uiCoord_.z = ofClamp(uiCoord_.z, -1, 1);
	uiClamp = uiCoord_;
}

//--------------------------------------------------------------
void vizManager::updateVolume2Slices()
{
	// transform current selected voxel coordinates
	// to normalised vector

	//ofClamp(volCoord.z,0, volDepth-1);
	ofVec3f uiCoord_;
	
	// map it to pixel values;
	float uiSize=100;
	uiCoord_ = uiClamp*uiSize;
	
	//use the pixel positions that are only inside the volume
	//(excluding areas between volume and uiPad limits)
	// coronal
	uiCoord_.z= ofMap(uiCoord_.z, -uiSize, uiSize, -volHeight/2, volHeight/2);
	// sagittal
	uiCoord_.x= ofMap(uiCoord_.x, -uiSize, uiSize, -volWidth/2, volWidth/2);
	// axial
	uiCoord_.y= ofMap(uiCoord_.y, -uiSize, uiSize, -volDepth/2, volDepth/2);
	
	//normalized again
	uiCoord_ = uiCoord_/100;
	
	//clamp the mapped values
	uiCoord_.x = ofClamp(uiCoord_.x, -1, 1);
	uiCoord_.y = ofClamp(uiCoord_.y, -1, 1);
	uiCoord_.z = ofClamp(uiCoord_.z, -1, 1);
	uiCoord = uiCoord_;
}

//--------------------------------------------------------------
void vizManager::draw()
{
	if (bDraw)
	{
		// Draw Volume
		ofSetColor(255);
		cam.begin();
			myVolume.updateVolume(volPos, volSize, 0);
		// check collision
		if (bSelecting){
			doesIntersect = myVolume.getIntersection(&cam, intersectionPosition);
			if (doesIntersect) {
				updateVolume2Slices();
				updateCoordinates();
				updateSlicesImage();
			}
		}
		cam.end();
		myVolume.draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());
		
		// Draw ArcBall
		if (!bSelecting && ofGetMousePressed()) cam.drawArcBall();
		
		//Draw Slices "canvas"
		ofPushView();
		ofTranslate(initX, initY);
		ofPushStyle();
			ofSetColor(OFX_UI_COLOR_BACK);
			ofRect(0, 0, boxW+dist*3, (boxW+dist)*3+dist);
		ofPopStyle();
		
		//Draw Slices
		ofPushView();
		ofTranslate(dist, dist);
		volume2D.draw(CORONAL);
		
		ofPushView();
		ofTranslate( 0, boxH+ dist);
		volume2D.draw(SAGITTAL);
		
		ofPushView();
		ofTranslate( 0, boxH+ dist);
		volume2D.draw(AXIAL);
		
		ofPopView();
		ofPopView();
		ofPopView();
		ofPopView();
		
		//Draw talairach pixel value and labels
		ofPushStyle();
//		ofSetColor(voxelValue, 255);
//		ofRect(initX,tempY,boxH*2+dist*5,dist);
		
		ofSetColor(0, 255, 255);
		ofDrawBitmapString("Talairarch coordinate :", talDrawX, talDrawY);
		string str= "x= "+ ofToString(talCoord.x)+" y= "+ ofToString(talCoord.y)+" z= "+ ofToString(talCoord.z);
		ofDrawBitmapString(str, talDrawX, talDrawY+dist);
		for (int i=0; i<outputLabels.size()-2; i++) {
			vector<string> items = ofSplitString(outputLabels[i+2], ",");
				for (int j=0; j<items.size(); j++) {
					ofDrawBitmapString(items[j], talDrawX, talDrawY+i*dist+j*dist+(dist*2));
				}
		}
		ofPopStyle();
		
		cam.begin();
		ofPushView();
		glScalef (-1.0, 1.0, 1.0);	// draw the volume with correct map
		ofTranslate(volPos.x - volSize.x/2, volPos.y - volSize.y/2, volPos.z - volSize.z/2);
		ofScale(volSize.x,volSize.y,volSize.z);
//		points
		glPointSize(4);
		ofSetColor(200,20);
		points.drawVertices();
		glPointSize(1);
		ofSetColor(255);
		pointsWhite.drawVertices();
		ofPopView();
		cam.end();
	}
}

//--------------------------------------------------------------
void vizManager::setup_guis()
{
	allowEvent	= false;
	// Volume UI
	setup_guiVolume();
	guiVolume->loadSettings("GUI/volume_settings.xml");
	guiVolume->setDrawBack(true);
	guiVolume->setVisible(false);
	
	// Sliders UI
	setup_guiSliders();
	guiSliders->loadSettings("GUI/slices_settings.xml");
	guiSliders->setDrawBack(false);
//	guiSliders->setAutoDraw(true);	this method does not work
	
	bDraw		= true;
	allowEvent	= true;
	update();
}

//--------------------------------------------------------------
void vizManager::setup_guiVolume()
{
	// Volume Canvas vars from 2Dpad
	guiVolume = new ofxUICanvas(initX+ (dist*2)+ sliderW + boxH,
									initY+dist + (boxH),
									(dist*2)+ boxH,
									280);
	guiVolume->setPadding(4);
	
	//new vars for widgets
	float canvasW = boxW+dist*2;
	float sliderH = 10;
	float initX = OFX_UI_GLOBAL_WIDGET_SPACING;
	float sliderW = boxW+dist*2-OFX_UI_GLOBAL_WIDGET_SPACING*2;
	
	guiVolume->addWidgetDown(new ofxUILabel("Volume Settings", OFX_UI_FONT_MEDIUM));
	guiVolume->addSlider("FBO quality", 0.0, 1.0, &FBOq, sliderW, sliderH)->setDrawBack(true);
	guiVolume->addSlider("Z quality", 0.0, 2.0, &Zq, sliderW, sliderH)->setDrawBack(true);
	guiVolume->addSlider("Threshold", 0.0, 1.0, &thresh, sliderW, sliderH)->setDrawBack(true);
	guiVolume->addSlider("Density", 0.0, 1.0, &density, sliderW, sliderH)->setDrawBack(true);
	guiVolume->addSlider("Clip depth", -1.0, 1.0, &clipPlaneDepth, sliderW, sliderH)->setDrawBack(true);
	guiVolume->addSlider("Elevation clip angle", -1.0, 1.0, &elevation, sliderW, sliderH)->setDrawBack(true);
	guiVolume->addSlider("Azimuth clip angle", -1.0, 1.0, &azimuth, sliderW, sliderH)->setDrawBack(true);
//	guiVolume->addSlider("Dithering", 0.0, 1.0, dithering, sliderW, sliderH)->setDrawBack(true);
//	guiVolume->addWidgetDown(new ofxUIToggle( sliderH, sliderH, false, "linearFilter"));
//	guiVolume -> autoSizeToFitWidgets();
	ofAddListener(guiVolume->newGUIEvent,this,&vizManager::guiEvent);
}

//--------------------------------------------------------------
void vizManager::setup_guiSliders()
{
	guiSliders = new ofxUICanvas(initX, initY, boxW+sliderW+dist*2, boxW*3+dist*4);
	ofAddListener(guiSliders->newGUIEvent,this,&vizManager::guiEvent);
	
	// ADDING WIDGETS:
	//	It doesn't work like this
	//	ofxUISlider *slider;
	//	slider =(ofxUISlider *) guiSliders->addSlider("uiCoord.z", -1, 1, &uiCoord.z , sliderW,	boxH, boxW+dist, dist);
	//	slider->setLabelVisible(false);
	
	// used vars
	ofxUISlider *slider;
	ofxUI2DPad *pad;
	ofColor transparent;
	transparent.set(0,0,0,0);
	
	//--------------------------- Coronal ---------------------------//
	guiSliders->addWidget(new ofxUISlider("coronalDepth",
										  uiRange.x,
										  uiRange.y,
										  &uiCoord.z,
										  sliderW,
										  boxH,
										  boxW+dist, dist));
	
	slider = (ofxUISlider *) guiSliders->getWidget("coronalDepth");
	slider->setLabelVisible(false);
	
	guiSliders->addWidget(new ofxUI3DPad("coronalPad",
										 uiRange,					//_rangeX
										 uiRange,					//_rangeY
										 uiRange,					//_rangeZ
										 &uiCoord,					//_value
										 boxW, boxH, dist, dist,	// w, h, x, y
										 OFX_UI_FRONT));			// viewPoint

	pad = (ofxUI2DPad *) guiSliders->getWidget("coronalPad");
	pad->setLabelVisible(false);
	pad->setColorBack(transparent);
	
	
	//--------------------------- Sagittal ---------------------------//
	guiSliders->addWidget(new ofxUISlider("sagittalDepth",
										  uiRange.x,
										  uiRange.y,
										  &uiCoord.x,
										  sliderW,
										  boxH,
										  boxW+dist,
										  boxW+dist+sliderW));
	
	slider = (ofxUISlider *) guiSliders->getWidget("sagittalDepth");
	slider->setLabelVisible(false);
	
	guiSliders->addWidget(new ofxUI3DPad("sagittalPad",
										 uiRange,					//_rangeX
										 uiRange,					//_rangeY
										 uiRange,					//_rangeZ
										 &uiCoord,					//_value
										 boxW, boxH, dist, boxW+dist+sliderW,	// w, h, x, y
										 OFX_UI_LEFT));				// viewPoint
	
	pad = (ofxUI2DPad *) guiSliders->getWidget("sagittalPad");
	pad->setLabelVisible(false);
	pad->setColorBack(transparent);
	
	
	//--------------------------- Axial ---------------------------//
	guiSliders->addWidget(new ofxUISlider("axialDepth",
										  uiRange.x,
										  uiRange.y,
										  &uiCoord.y,
										  sliderW,
										  boxH,
										  boxW+dist,
										  boxW*2+dist*2+sliderW));
	
	slider = (ofxUISlider *) guiSliders->getWidget("axialDepth");
	slider->setLabelVisible(false);
	
	guiSliders->addWidget(new ofxUI3DPad("axialPad",
										 uiRange,					//_rangeX
										 uiRange,					//_rangeY
										 uiRange,					//_rangeZ
										 &uiCoord,					//_value
										 boxW, boxH, dist, boxW*2+dist*2+sliderW,	// w, h, x, y
										 OFX_UI_TOP));				// viewPoint
	
	pad = (ofxUI2DPad *) guiSliders->getWidget("axialPad");
	pad->setLabelVisible(false);
	pad->setColorBack(transparent);

}

//--------------------------------------------------------------
void vizManager::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	//	ofLogVerbose("vizManager") << "GUI:: got event from: " << name;
	
	if(name == "FBO quality")
	{
		ofLogVerbose() << "FBO quality " << FBOq;
		myVolume.setXyQuality(FBOq);
	}
	else if(name == "Z quality")
	{
		ofLogVerbose() << "Z quality " << Zq;
		myVolume.setZQuality(Zq);
	}
	else if(name == "Threshold")
	{
		ofLogVerbose() << "Threshold " << thresh;
		myVolume.setThreshold(thresh);
	}
	else if(name == "Density")
	{
		ofLogVerbose() << "Density " << density;
		myVolume.setDensity(density);
	}
	else if(name == "Dithering")
	{
		ofLogVerbose() << "Dithering " << dithering;
		myVolume.setDithering(dithering);
	}
	else if(name == "Clip depth")
	{
		ofLogVerbose() << "Cut Plane Depth " << clipPlaneDepth;
		myVolume.setClipDepth(clipPlaneDepth);
	}
	else if(name == "Elevation clip angle")
	{
		ofLogVerbose() << "Elevation " << elevation;
		myVolume.setElevation(elevation);
	}
	else if(name == "Azimuth clip angle")
	{
		ofLogVerbose() << "Azimuth " << azimuth;
		myVolume.setAzimuth(azimuth);
	}
	else if(name == "linearFilter")
	{
		ofLogVerbose() << "linearFilter " << linearFilter;
		if (linearFilter){
            myVolume.setVolumeTextureFilterMode(GL_LINEAR);
        }else {
            myVolume.setVolumeTextureFilterMode(GL_NEAREST);
        };
	}
	else if(name == "coronalDepth")
	{
		ofLogVerbose() <<	"coronalDepth " << uiCoord.z;
	}
	else if(name == "sagittalDepth")
	{
		ofLogVerbose() <<	"sagittalDepth " << uiCoord.x;
	}
	else if(name == "axialDepth")
	{
		ofLogVerbose() <<	"axialDepth " << uiCoord.y;
	}
	else if (name== "coronalPad")
	{
		ofLogVerbose() <<	"coronalPad.x =  " << uiCoord.x;
		ofLogVerbose() <<	"coronalPad.y =  " << uiCoord.y;
	}
	else if (name== "sagittalPad")
	{
		ofLogVerbose() <<	"SagittalPad.x = " << uiCoord.z;
		ofLogVerbose() <<	"SagittalPad.y = " << uiCoord.y;
	}
	else if (name== "axialPad")
	{
		ofLogVerbose() <<	"axialPad.x = " << uiCoord.x;
		ofLogVerbose() <<	"axialPad.y = " << uiCoord.z;
	}
	else if(name == "latitude")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget;
		latitude = floor(slider->getScaledValue());
		ofLogVerbose() <<	"latitude " << latitude;
	}
	if (allowEvent)update();
}

//--------------------------------------------------------------
void vizManager::keyPressed(int key ){
    switch(key)
    {
		case ' ':
			bSelecting = !bSelecting;
			if (bSelecting) cam.disableMouseInput();
			break;
		case 'h':
            guiVolume->toggleVisible();
			break;
		case 's':
			guiVolume->saveSettings("GUI/volume_settings.xml");
//			guiSliders->saveSettings("GUI/slices_settings.xml");
			ofxSaveCamera(cam, "GUI/cameraSettings.txt");
			break;
		case 'l':
			ofxLoadCamera(cam, "GUI/cameraSettings.txt");
			guiVolume->loadSettings("GUI/volume_settings.xml");
			break;
		case 'f':
			//ofSetWindowPosition(0, 0);
			//ofSetVerticalSync(false);
			//ofSetFullscreen(false);
			break;

		case 'c':
			createPointCloud();
			break;

		case 'r':
			cam.bRotate = !cam.bRotate;
			break;
		case 'F':
			ofSetVerticalSync(true);
			//ofSetFullscreen(true);
			break;
		case 't':
			// talairach tests
			int voxelValue = volume2D.getVoxelValue();
			//mapping from pixel value to index value on the Talairach Atlas
			int currentValue= ofMap(voxelValue, 0, 255, 0, 1105);
			
			string lobe = talairachAtlas.getLobe(currentValue);
			ofLogNotice("Talairach")<< lobe;
			break;
	}
}

//--------------------------------------------------------------
void vizManager::mousePressed(ofMouseEventArgs& e)
{
	
//	checkRay(e.x, e.y);
}











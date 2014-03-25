#include "myCamera.h"
#include "ofMath.h"
#include "ofUtils.h"

// when an myCamera is moving due to momentum, this keeps it
// from moving forever by assuming small values are zero.
float minDifference = 0.1e-5;

// this is the default on windows os
unsigned long doubleclickTime = 200;

//----------------------------------------
myCamera::myCamera(){
	lastTap	= 0;
	lastDistance = 0;
	drag = 0.9f;
	
	//when 1 moving the mouse from one side to the other of the arcball (min(viewport.width, viewport.height))
	//will rotate 180degrees. when .5, 90 degrees.
	sensitivityRot = 1.0f;
	sensitivityXY = .5;
	sensitivityZ= .7;
	fov= 110;
	bDistanceSet = false;
	bMouseInputEnabled = false;
	bDoRotate = false;
	bApplyInertia =false;
	bDoTranslate = false;

	bValidClick = false;
	bEnableMouseMiddleButton = true;
	bAutoDistance = true;
	doTranslationKey = 'm';
	
	reset();
	enableMouseInput();

	
	//Rotation	j
	speed = .5;
	rotation = 0;
	bRotate = false;
	
	//arcBall
	arcBallFactor = .7;
	bInsideArcball = true;
	arcBallSize = getArcBallSize();
}

//----------------------------------------
myCamera::~myCamera(){
	disableMouseInput();
}
//----------------------------------------
void myCamera::update(ofEventArgs & args){
    if(!bDistanceSet && bAutoDistance){
        setDistance(getImagePlaneDistance(viewport), true);
    }
    if(bMouseInputEnabled){
		
		rotationFactor = sensitivityRot * 180 / min(viewport.width, viewport.height);
		if (bMouseInputEnabled) {
			updateMouse();
		}
		
		if (bDoRotate) {
			updateRotation();
		}else if (bDoTranslate) {
			updateTranslation();
		}
	}
}
//----------------------------------------
void myCamera::begin(ofRectangle viewport){
	this->viewport = viewport;
	ofCamera::begin(viewport);
	

	if (bRotate){
		ofRotateZ(rotation);
		rotation+=speed;
	}
}

//----------------------------------------
void myCamera::reset(){
	target.resetTransform();
	
	target.setPosition(0,0, 0);
	lookAt(target);
	
	resetTransform();
	setPosition(0, 0, lastDistance);
	
	xRot = 0;
	yRot = 0;
	zRot = 0;
	
	moveX = 0;
	moveY = 0;
	moveZ = 0;
}
//----------------------------------------
void myCamera::setTarget(const ofVec3f& targetPoint){
	target.setPosition(targetPoint);
	lookAt(target);
}
//----------------------------------------
void myCamera::setTarget(ofNode& targetNode){
	target = targetNode;
	lookAt(target);
}
//----------------------------------------
ofNode& myCamera::getTarget(){
	return target;
}
//----------------------------------------
void myCamera::setDistance(float distance){
	setDistance(distance, true);
}
//----------------------------------------
void myCamera::setDistance(float distance, bool save){//should this be the distance from the camera to the target?
	if (distance > 0.0f){
		if(save){
			this->lastDistance = distance;
		}
		setPosition(target.getPosition() + (distance * getZAxis()));
		bDistanceSet = true;
	}
}
//----------------------------------------
float myCamera::getDistance() const {
	return target.getPosition().distance(getPosition());
}
//----------------------------------------
void myCamera::setAutoDistance(bool bAutoDistance){
    this->bAutoDistance = bAutoDistance;
    if (bAutoDistance) {
        bDistanceSet = false;
    }
}
//----------------------------------------
void myCamera::setDrag(float drag){
	this->drag = drag;
}
//----------------------------------------
float myCamera::getDrag() const {
	return drag;
}
//----------------------------------------
void myCamera::setTranslationKey(char key){
	doTranslationKey = key;
}
//----------------------------------------
char myCamera::getTranslationKey(){
	return doTranslationKey;
}
//----------------------------------------
void myCamera::enableMouseInput(){
	if(!bMouseInputEnabled){
		bMouseInputEnabled = true;
		//	ofRegisterMouseEvents(this);
		ofAddListener(ofEvents().update , this, &myCamera::update);
	}
}
//----------------------------------------
void myCamera::disableMouseInput(){
	if(bMouseInputEnabled){
		bMouseInputEnabled = false;
		//ofUnregisterMouseEvents(this);
		ofRemoveListener(ofEvents().update, this, &myCamera::update);
	}
}
//----------------------------------------
bool myCamera::getMouseInputEnabled(){
	return bMouseInputEnabled;
}
//----------------------------------------
void myCamera::enableMouseMiddleButton(){
	bEnableMouseMiddleButton = true;
}
//----------------------------------------
void myCamera::disableMouseMiddleButton(){
	bEnableMouseMiddleButton = false;
}
//----------------------------------------
bool myCamera::getMouseMiddleButtonEnabled(){
	return bEnableMouseMiddleButton;
}
//----------------------------------------
void myCamera::updateTranslation(){
	if (bApplyInertia) {
		moveX *= drag;
		moveY *= drag;
		moveZ *= drag;
		if (ABS(moveX) <= minDifference && ABS(moveY) <= minDifference && ABS(moveZ) <= minDifference) {
			bApplyInertia = false;
			bDoTranslate = false;
		}
	}
	move((getXAxis() * moveX) + (getYAxis() * moveY) + (getZAxis() * moveZ));
}
//----------------------------------------
void myCamera::updateRotation(){
	
//	if (xRot>10) xRot= 10;
	
	if (bApplyInertia) {
		xRot *=drag;
		yRot *=drag;
		zRot *=drag;
		
		if (ABS(xRot) <= minDifference && ABS(yRot) <= minDifference && ABS(zRot) <= minDifference) {
			bApplyInertia = false;
			bDoRotate = false;
		}
	}
	curRot = ofQuaternion(xRot, ofCamera::getXAxis(), yRot, ofCamera::getYAxis(), zRot, ofCamera::getZAxis());
	setPosition((ofCamera::getGlobalPosition()-target.getGlobalPosition())*curRot +target.getGlobalPosition());
	rotate(curRot);
}
//----------------------------------------
void myCamera::updateMouse(){
	mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
	if(viewport.inside(mouse.x, mouse.y) && !bValidClick && ofGetMousePressed()){
		unsigned long curTap = ofGetElapsedTimeMillis();
		if(lastTap != 0 && curTap - lastTap < doubleclickTime){
			reset();
		}
		
		if ((bEnableMouseMiddleButton && ofGetMousePressed(OF_MOUSE_BUTTON_MIDDLE))
			|| ofGetKeyPressed(doTranslationKey)
			|| ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT)){
			
			bDoTranslate = true;
			bDoRotate = false;
			bApplyInertia = false;
		}else if (ofGetMousePressed(OF_MOUSE_BUTTON_LEFT)) {
			bDoTranslate = false;
			bDoRotate = true;
			bApplyInertia = false;
			getArcBallSize();

			if(ofVec2f(mouse.x - viewport.x - (viewport.width/2), mouse.y - viewport.y - (viewport.height/2)).length()
			   < min(viewport.width/2, viewport.height/2)*	arcBallFactor){
				bInsideArcball = true;
			}else {
				bInsideArcball = false;
			}
		}
		lastTap = curTap;
		//lastMouse = ofVec2f(ofGetPreviousMouseX(),ofGetPreviousMouseY()); //this was causing the camera to have a tiny "random" rotation when clicked.
		lastMouse = mouse;
		bValidClick = true;
		bApplyInertia = false;
	}
	
	if (bValidClick) {
		if (!ofGetMousePressed()) {
			bApplyInertia = true;
			bValidClick = false;
		}else {
			int vFlip;
			if(isVFlipped()){
				vFlip = -1;
			}else{
				vFlip =  1;
			}
			
			mouseVel = mouse  - lastMouse;
			
			if (bDoTranslate) {
				moveX = 0;
				moveY = 0;
				moveZ = 0;
				if (ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT)) {
					moveZ = mouseVel.y * sensitivityZ * (getDistance() + FLT_EPSILON)/ viewport.height;
				}else {
					moveX = -mouseVel.x * sensitivityXY * (getDistance() + FLT_EPSILON)/viewport.width;
					moveY = vFlip * mouseVel.y * sensitivityXY * (getDistance() + FLT_EPSILON)/viewport.height;
				}
			}else {
				xRot = 0;
				yRot = 0;
				zRot = 0;
				if (bInsideArcball) {
					xRot = vFlip * -mouseVel.y * rotationFactor;
					yRot = -mouseVel.x * rotationFactor;
				}else {
					ofVec2f center(viewport.width/2, viewport.height/2);
					zRot = - vFlip * ofVec2f(mouse.x - viewport.x - center.x, mouse.y - viewport.y - center.y).angle(lastMouse - ofVec2f(viewport.x, viewport.y) - center);
				}
			}
			lastMouse = mouse;
		}
	}
}



// j
//----------------------------------------
void myCamera::setArcBallFactor(float factor){
	arcBallFactor = factor;
}
//----------------------------------------
float myCamera::getArcBallSize(){

	arcBallSize = min(viewport.width/2, viewport.height/2)*	arcBallFactor;
	return	arcBallSize;
}
//----------------------------------------
void myCamera::drawArcBall(){
	if (ofGetMousePressed()){
		if(bMouseInputEnabled){
			ofPushStyle();
			ofNoFill();
			ofSetCircleResolution(100);
			ofSetColor(140);
			ofSetLineWidth(.1);
			ofCircle(viewport.width/2, viewport.height/2, arcBallSize);
			ofPopStyle();
		}
	}
}






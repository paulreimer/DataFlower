#pragma once

#include "TuiSystem.h"
#include "testApp.h"

extern testApp* myApp;

TuiSystem::TuiSystem() {
	printf("TuiSystem::TuiSystem()\n");
}

TuiSystem::~TuiSystem() {
	printf("TuiSystem::~TuiSystem()\n");
	destroy();
}

void TuiSystem::setup(){	 
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);
	
	threshold = 80;
	bLearnBakground = true;
	backgroundSubOn = false;
	
	//detect finger is off by default
	fidfinder.detectFinger		= true;
	fidfinder.maxFingerSize		= 25; 
	fidfinder.minFingerSize		= 5;
	fidfinder.fingerSensitivity	= 0.05f; //from 0 to 2.0f
}

//--------------------------------------------------------------
void TuiSystem::update(){		
	if (bLearnBakground == true){
		grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
		bLearnBakground = false;
		backgroundSubOn = true;
	}
	
	// take the abs value of the difference between background and incoming and then threshold:
	if (backgroundSubOn) {
		grayDiff.absDiff( grayBg, grayImage );
	} else {
		grayDiff = grayImage;
	}
	grayDiff.threshold(threshold);
	fidfinder.findFiducials( grayDiff );
}

//--------------------------------------------------------------
void TuiSystem::draw(){
	grayDiff.draw(360,280);
	
	//use this method for the FiducialTracker
	//to get fiducial info you can use the fiducial getter methods
	float _x=0, _y=0, deg;
	for (list<ofxFiducial>::iterator fiducial = fidfinder.fiducialsList.begin(); fiducial != fidfinder.fiducialsList.end(); fiducial++) {
		_x = fiducial->current.xpos*VIDEO_INVWIDTH * myApp->window.width;
		_y = fiducial->current.ypos*VIDEO_INVWIDTH * myApp->window.width;
		ofNoFill();
		ofSetRectMode(OF_RECTMODE_CENTER);
		glPushMatrix();
		glTranslatef(_x, _y, 0);
		deg = fiducial->getAngle()*180.0/PI; // get degree
		glRotatef(deg, 0, 0, 1.0); // must flip degrees to compensate for image flip
		ofSetColor(255, 0, 0);//set color red
		ofRect(0, 0, fiducial->r_size, fiducial->r_size); //draw root size red
		ofSetColor(0, 0, 255); //set color blue
		ofCircle(0, fiducial->l_size*4, fiducial->l_size); //draw leaf size blue
		ofSetColor(0, 255, 0); //set color green
		ofDrawBitmapString(ofToString( fiducial->fidId ), 0, 0); //draw fiducial number green
		glPopMatrix();
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetColor(255,255,255);

		fiducial->computeCorners();
		ofSetColor(0, 255, 0);
		ofNoFill();
		glPushMatrix();
		glTranslatef(_x, _y, 0);
		if (fiducial->cornerPoints.size() > 0) {
			for(int i = 0; i < fiducial->cornerPoints.size() ;i++) {
				_x = fiducial->cornerPoints[i].x*VIDEO_INVWIDTH * myApp->window.width;
				_y = fiducial->cornerPoints[i].x*VIDEO_INVHEIGHT * myApp->window.height;
				ofCircle(_x, _y, 4);
				////printf("corner 0.x: %f corner 0.y %f\n", cornerPoints[i].x, cornerPoints[i].y);
			}
		}
		glPopMatrix();
		ofSetColor(255,255,255);
		
		ofSetColor(0,0,255);//set color to blue
		//if mouse (- 20 to compensate for drawing at 20) is inside fiducial then fill
		if (fiducial->isPointInside(myApp->mouseX - 20, myApp->mouseY - 20)) ofFill();
		else ofNoFill();// else dont
		ofCircle(myApp->mouseX, myApp->mouseY, 10);//draw mouse position
		ofSetColor(255,255,255);//reset color to white
		//like this one below
		//cout << "fiducial " << fiducial->getId() << " found at ( " << fiducial->getX() << "," << fiducial->getY() << " )" << endl;
		//take a good look at the fiducial class to get all the info and a few helper functions
	}
	
	//draw the fingers
	for (list<ofxFinger>::iterator finger = fidfinder.fingersList.begin(); finger != fidfinder.fingersList.end(); finger++) {
		finger->draw(20, 20);
	}

//	printf("fid count %i\n",fidfinder.fiducialsList.size());
}

void TuiSystem::destroy() {
	printf("TuiSystem::destroy()\n");
}

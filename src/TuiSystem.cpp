#pragma once

#include "TuiSystem.h"
#include "testApp.h"

extern testApp* myApp;

TuiSystem::TuiSystem() {
	printf("TuiSystem::TuiSystem()\n");
	fingersList = NULL;
	fiducialsList = NULL;
}

TuiSystem::~TuiSystem() {
	printf("TuiSystem::~TuiSystem()\n");
	destroy();
}

void TuiSystem::setup(){	 
}

//--------------------------------------------------------------
void TuiSystem::update(){
	int f;
	for (list<ofxFiducial>::iterator fiducial = fiducialsList->begin(); fiducial != fiducialsList->end(); fiducial++) {
		switch (fiducial->getId()) {
			case 6: f = 0;
				break;
			case 4: f = 1;
				break;
			case 10: f = 2;
				break;
			default: f = -1;
				break;
		}
		if (f >= 0) {
			VideoFilter* filter = myApp->videoSystem.pipeline(0)->filter(f);
			if (fiducial->current.xpos >= 0 && fiducial->current.ypos >= 0) {
				filter->setPos(fiducial->current.xpos*VIDEO_INVWIDTH*myApp->window.width,
							   fiducial->current.ypos*VIDEO_INVWIDTH*myApp->window.width);
				filter->rotateRad(fiducial->getAngle());
			}
		}
	}
}

//--------------------------------------------------------------
void TuiSystem::draw(){
	// draw fullscreen fiducials
	float _x=0, _y=0, deg;
	for (list<ofxFiducial>::iterator fiducial = fiducialsList->begin(); fiducial != fiducialsList->end(); fiducial++) {
		_x = fiducial->current.xpos*VIDEO_INVWIDTH * myApp->window.width;
		_y = fiducial->current.ypos*VIDEO_INVWIDTH * myApp->window.width;
		// Draw root
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
		glPopMatrix(); // fiducial
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetColor(255,255,255);
		
		// Draw corners
		fiducial->computeCorners();
		ofSetColor(0, 255, 0);
		ofNoFill();
		glPushMatrix();
		if (fiducial->cornerPoints.size() > 0) {
			for(int i = 0; i < fiducial->cornerPoints.size(); i++) {
				_x = fiducial->cornerPoints[i].x*VIDEO_INVWIDTH * myApp->window.width;
				_y = fiducial->cornerPoints[i].y*VIDEO_INVWIDTH * myApp->window.width;
				ofCircle(_x, _y, 4);
				//printf("corner %d.x: %f corner %d.y %f\n", i, fiducial->cornerPoints[i].x, i, fiducial->cornerPoints[i].y);
			}
		}
		glPopMatrix(); // corners
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
	if (fingersList->size()) {
		//draw the fingers
		ofNoFill();
		ofSetRectMode(OF_RECTMODE_CENTER);
		ofSetColor(255, 0, 0);//set color red
		for (list<ofxFinger>::iterator finger = fingersList->begin(); finger != fingersList->end(); finger++) {
			glPushMatrix();
			glTranslatef(finger->current.xpos + _x, finger->current.ypos + _y, 0);
			ofRect(0, 0, finger->width, finger->height); //draw finger red
			ofDrawBitmapString(ofToString( finger->fingerID ), 0, finger->height+2); //draw finger number
			glPopMatrix();
		}
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetColor(255,255,255);	
	}
}	

void TuiSystem::destroy() {
	printf("TuiSystem::destroy()\n");
}

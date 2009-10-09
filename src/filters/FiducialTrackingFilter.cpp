#pragma once

#include "FiducialTrackingFilter.h"

FiducialTrackingFilter::FiducialTrackingFilter()
{
	if (verbose) printf("FiducialTrackingFilter::FiducialTrackingFilter()\n");
}

FiducialTrackingFilter::~FiducialTrackingFilter()
{
	if (verbose) printf("FiducialTrackingFilter::~FiducialTrackingFilter()\n");
	destroy();
}

void FiducialTrackingFilter::setup() 
{
	GrayscaleFilter::setup();
	fidFinder.detectFinger		= true;
	fidFinder.maxFingerSize		= 25;
	fidFinder.minFingerSize		= 5;
	fidFinder.fingerSensitivity	= 0.05f; //from 0 to 2.0f

	addContent("Output", output);
	addToggle("Detect Fingers", fidFinder.detectFinger);
	addSlider("Min Finger Size", fidFinder.minFingerSize, 0, 50);
	addSlider("Max Finger Size", fidFinder.maxFingerSize, 0, 50);
	addSlider("Finger Sensitivity", fidFinder.fingerSensitivity, 0.0, 2.0);
}

void FiducialTrackingFilter::update() 
{
	fidFinder.findFiducials(input);
}

void FiducialTrackingFilter::draw() 
{
	VideoFilter::draw();
	list<ofxFiducial>& fiducials = fidFinder.fiducialsList;	
	list<ofxFinger>& fingers = fidFinder.fingersList;	
	
	if (!fiducials.size() && !fingers.size()) return;
	
	// Draw on top of gui
	float _x=0, _y=0;
	glPushMatrix();
	glTranslatef(x+width/2, y+height/2, 0);
	glRotatef(angle*180.0/PI, 0, 0, 1.0); // must flip degrees to compensate for image flip
	glTranslatef(-width/2, -height/2, 0);
//	glScalef(controls[0]->width/(float)videoSize.x, controls[0]->height/(float)videoSize.y, 0.0);
	glScalef(width/(float)videoSize.x, width/(float)videoSize.x, 0.0);

	for (list<ofxFiducial>::iterator fiducial = fiducials.begin();
		 fiducial != fiducials.end();
		 fiducial++) {

		fiducial->draw(0,0);
		fiducial->drawCorners(0,0);
	}
	
	for (list<ofxFinger>::iterator finger = fingers.begin();
		 finger != fingers.end();
		 finger++) {

		finger->draw(0,0);
	}
	
	glPopMatrix();
}

void FiducialTrackingFilter::destroy() 
{
	if (verbose) printf("FiducialTrackingFilter::destroy()\n");
}

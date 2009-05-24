#pragma once

#include "FiducialTrackingFilter.h"

FiducialTrackingFilter::FiducialTrackingFilter(string name) : VideoFilter(name) {
	printf("FiducialTrackingFilter::FiducialTrackingFilter()\n");
}

FiducialTrackingFilter::~FiducialTrackingFilter() {
	printf("FiducialTrackingFilter::~FiducialTrackingFilter()\n");
	destroy();
}

void FiducialTrackingFilter::setup() {
	VideoFilter::setup();
	grayInput.allocate(VIDEO_SIZE);
	grayOutput.allocate(VIDEO_SIZE);

	fidFinder.detectFinger		= true;
	fidFinder.maxFingerSize		= 25; 
	fidFinder.minFingerSize		= 5;
	fidFinder.fingerSensitivity	= 0.05f; //from 0 to 2.0f

	addContent("Output", &output, VIDEO_WIDTH);
	addToggle("Detect Fingers",
			  &fidFinder.detectFinger);
	addSlider("Min Finger Size",
			  &fidFinder.minFingerSize, 0, 50);
	addSlider("Max Finger Size",
			  &fidFinder.maxFingerSize, 0, 50);
	addSlider("Finger Sensitivity",
			  &fidFinder.fingerSensitivity, 0, 0.5, 0);

}

void FiducialTrackingFilter::update() {
//	VideoFilter::update();
	grayInput = input;

	fidFinder.findFiducials( grayInput );
}

void FiducialTrackingFilter::draw() {
	VideoFilter::draw();
	float _x=0, _y=0;
	for (list<ofxFiducial>::iterator fiducial = fidFinder.fiducialsList.begin(); fiducial != fidFinder.fiducialsList.end(); fiducial++) {
		// Draw on top of gui
		fiducial->draw( x, y );//draw fiducial                                
		fiducial->drawCorners( x, y );//draw corners
	}
}

void FiducialTrackingFilter::destroy() {
	printf("FiducialTrackingFilter::destroy()\n");
}

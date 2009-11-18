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
	
	output.set(0);
}

void FiducialTrackingFilter::update() 
{
	fidFinder.findFiducials(input);
}

void FiducialTrackingFilter::draw() 
{
	VideoFilter::draw(); // try after drawing fiducials?

	list<ofxFiducial>& fiducials = fidFinder.fiducialsList;
	list<ofxFinger>& fingers = fidFinder.fingersList;
	
	if (!fiducials.size() && !fingers.size()) return;
	
	glPushMatrix();

	glTranslatef(x,	y, 0.0);
	glRotatef	(ofRadToDeg(angle), 0.0, 0.0, 1.0);
	glTranslatef(offset.x, offset.y, 0.0);
	glScalef	(scale*width/(float)videoSize.x, scale*width/(float)videoSize.x, 1.0);

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
	GrayscaleFilter::destroy();
}

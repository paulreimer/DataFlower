#include "MarkerFindingFilter.h"

MarkerFindingFilter::MarkerFindingFilter()
{
	if (verbose) printf("MarkerFindingFilter::MarkerFindingFilter()\n");
}

MarkerFindingFilter::~MarkerFindingFilter()
{
	if (verbose) printf("MarkerFindingFilter::~MarkerFindingFilter()\n");
	destroy();

}

void MarkerFindingFilter::setup() 
{
	GrayscaleFilter::setup();

	diagnostics.allocate(videoSize.x, videoSize.y);
	markerFinder.doTracking = true;
	markerFinder.width = videoSize.x;
	markerFinder.height = videoSize.y;
	
	markerFinder.setup();
	
//	addContent("Output", &output);
	addContent("Diagnostics", diagnostics);
	
	addToggle("Enable Tracking", markerFinder.doTracking);
	addToggle("Calc. Geometry", markerFinder.calcGeometry);
	addToggle("Show Image", markerFinder.displayThresholded);

	markerFinder.thresholdMethod = ThresholdFilter::EFixed;
	addSlider("Threshold Method", (int&)markerFinder.thresholdMethod,
			  (int)ThresholdFilter::EBernsen, (int)ThresholdFilter::EFixed);

	addSlider("Threshold Param 1", markerFinder.thresholdParam1, 1, 64);
	addSlider("Threshold Param 2", markerFinder.thresholdParam2, 1, 64);

	markerFinder.trackingThresh = 5;
	addSlider("Tracking Threshold", markerFinder.trackingThresh, 1, 20);

	markerFinder.trackingDist = videoSize.x / 12;
	addSlider("Distance Threshold", markerFinder.trackingDist, 1, 255);

	addSlider("Target Markers", markerFinder.fiducialsTarget, 0, 10);
}

void MarkerFindingFilter::update() 
{
	markerFinder.grayImg = input;

	markerFinder.update();
		
	output = markerFinder.grayImg;
//	diagnostics = markerFinder.displayBufferPtr->getImageBuffer();
	diagnostics = markerFinder.displayImg;
}

void MarkerFindingFilter::draw() 
{
	VideoFilter::draw();
	list<FiducialData>& fiducials = markerFinder.fiducials;	
	
	if (!fiducials.size()) return;
	
//	printf("Found %d fiducials\n", fiducials.size());
	
	// Draw on top of gui
	float _x=0, _y=0;
	glPushMatrix();
	glTranslatef(x+width/2, y+height/2, 0);
	glRotatef(angle*180.0/PI, 0, 0, 1.0); // must flip degrees to compensate for image flip
	glTranslatef(-width/2, -height/2, 0);
//	glScalef(controls[0]->width/(float)videoSize.x, controls[0]->height/(float)videoSize.y, 0.0);
//	glScalef(width/(float)videoSize.x, width*(videoSize.y/videoSize.x)/(float)videoSize.y, 0.0);
	glScalef(width/(float)videoSize.x, width/(float)videoSize.x, 0.0);
	
	ofSetRectMode(OF_RECTMODE_CENTER);
	
	for (list<FiducialData>::iterator fiducial = fiducials.begin();
		 fiducial != fiducials.end();
		 fiducial++) {
		
//		int xi = fiducial->getCentre().x;
//		int yi = fiducial->getCentre().y;
		
//		float x = (float) (2 * xi) / (float) markerFinder.width - 1.0f;
//		float y = 1.0f - (float) (2 * yi) / markerFinder.height;

		printf("Found fiducial: %s (%d,%d), %dx%d\n", fiducial->getSequenceString(),
			   (int)fiducial->getCentre().x,(int)fiducial->getCentre().y,
			   (int)fiducial->getWidth(),	(int)fiducial->getHeight());

		glPushMatrix();
		ofSetColor(0xCACACA);
		glRotatef(fiducial->getAngle(), 0, 0, 1.0);
		ofRect(fiducial->getCentre().x, fiducial->getCentre().y,
			   fiducial->getWidth(), fiducial->getHeight());
		glPopMatrix();
	}
	
	glPopMatrix();
}


void MarkerFindingFilter::destroy() 
{
	if (verbose) printf("MarkerFindingFilter::destroy()\n");
	GrayscaleFilter::destroy();	
	diagnostics.clear();
}

#pragma once

#include "TuiSystem.h"
#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

TuiSystem::TuiSystem() {
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("TuiSystem::TuiSystem()\n");
	doDraw = true;

//	fingersList = NULL;
//	fiducialsList = NULL;
}

TuiSystem::~TuiSystem() {
	if (verbose) printf("TuiSystem::~TuiSystem()\n");
	destroy();
}

void TuiSystem::setup(){
	pipe.setPos(280, 72);
	myApp->videoSystem.addPipeline(&pipe);	

	pipe.addFilter(new HomographyFilter());
	pipe.addFilter(new SimpleThresholdingFilter());
	pipe.addFilter(&fidtracker);

//	markerfinder.markerFinder.storeFilename = ofToDataPath(MARKERS_FILE);
//	pipe.addFilter(&markerfinder);
}

inline bool leftToRightSort(ofxFiducial lhs, ofxFiducial rhs) {
	return lhs.current.xpos < rhs.current.xpos;
}

//--------------------------------------------------------------
void TuiSystem::update(){
//	list<ofxFiducial>* fids = &fidfinder.fiducials;	

	list<ofxFiducial>* fids = &fidtracker.fidFinder.fiducialsList;

	list<ofxFiducial>::iterator fiducial;
	VideoFilter *filter;

	for (fiducial = fids->begin(); fiducial != fids->end(); fiducial++) {
		// delay creating a filter until somewhat stable
		if (fiducial->life == 2)
			if (fiducial->state != FIDUCIAL_LOST) {
				ofNotifyEvent(this->fiducialFound, *fiducial);
//				fiducial->life += MAX_FIDUCIAL_LIFE;
			} else
				ofNotifyEvent(this->fiducialLost, *fiducial);
		else if (fiducial->life < 2)
			continue;

		ofNotifyEvent(this->fiducialUpdated, *fiducial);
	}

}

//--------------------------------------------------------------
void TuiSystem::draw(){
	if (!doDraw) return;
//	list<ofxFiducial>* fids = &markerfinder.fiducials;	

	list<ofxFiducial>* fids = &fidtracker.fidFinder.fiducialsList;	
	
	// draw fullscreen fiducials
	float _x=0, _y=0, deg;
	for (list<ofxFiducial>::iterator fiducial = fids->begin(); fiducial != fids->end(); fiducial++) {
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

VideoFilter* TuiSystem::createFiducialFilter(ofxFiducial* fiducial) {
	VideoFilter *filter;
	int chk_id = fiducial->getId() % 19;
	if (chk_id == NULL_FIDUCIAL_ID)
		filter = new PassthroughFilter();		
	else switch (chk_id) {
		case 0:
			filter = new PassthroughFilter();
			break;
		case 1:
			filter = new GrayscalePassthroughFilter();
			break;
		case 2:
			filter = new HomographyFilter();
			break;
		case 3:
			filter = new ZivkovicBgFilter();
			break;
		case 4:
			filter = new LiBgFilter();
			break;
#ifdef USE_OPENCL
#ifdef USE_GPU_VIS
		case 5:
			filter = new SobelFilterOcl(*(myApp->renderSystem.clScheduler));
			break;
		case 6:
			filter = new VoxelBufferFilter(*(myApp->renderSystem.clScheduler),
										   myApp->renderSystem.rayTracer);
			break;
#endif
#endif
		case 7:
			filter = new DifferencingFilter();
			break;
		case 8:
			filter = new SparseOpticalFlowFilter();
			break;
		case 9:
			filter = new SimpleThresholdingFilter();
			break;
		case 10:
			filter = new AdaptiveThresholdingFilter();
			break;
		case 11:
			filter = new CannyEdgeFilter();
			break;
		case 12:
			filter = new FiducialTrackingFilter();
			break;
		case 13:
			filter = new CaptureFilter();
			break;
/*		case 14:
			filter = new CalibrationFilter();
			break;
*/		case 15:
			filter = new SobelFilter();
			break;
		case 16:
			filter = new ContourFindingFilter();
			break;
		case 17:
			filter = new ThresholdingFilter();
			break;
#ifdef USE_OPENCV_TRUNK
		case 18:
			filter = new DenseOpticalFlowFilter();
			break;
		case 19:
			filter = new AdaptiveSkinFilter();
			break;
#endif
		case 20:
			filter = new BackgroundSubtractionFilter();
			break;
		default:
			filter = new PassthroughFilter();
//			filter = mirror();
	}
	
	filter->fiducial = fiducial;

	return filter;
}

void TuiSystem::toggleDraw() {
	doDraw = !doDraw;
}

void TuiSystem::destroy() {
	if (verbose) printf("TuiSystem::destroy()\n");
}

#pragma once

#include "RemoteSystem.h"
#include "testApp.h"
#include "ofxTuioCursor.h"

extern testApp* myApp;

RemoteSystem::RemoteSystem() {
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("RemoteSystem::RemoteSystem()\n");
}

RemoteSystem::~RemoteSystem() {
	if (verbose) printf("RemoteSystem::~RemoteSystem()\n");
	destroy();
}

void RemoteSystem::setup() {
#ifdef USE_MSAREMOTE
	msaRemote.start(3333);
#endif
	
	pos.rot.set		(0, 0, 0);
	pos.offset.set	(1.5f, 1.0f, 4.0f);
	
	touchesAnglePrev = touchesAngle = 0.0;
	touchesDistPrev = touchesDist = 0.0;
	
	ofAddListener(msaRemote.cursorAdded, this, &RemoteSystem::cursorAdded);
	ofAddListener(msaRemote.cursorRemoved, this, &RemoteSystem::cursorRemoved);
	ofAddListener(msaRemote.cursorUpdated, this, &RemoteSystem::cursorUpdated);
}

void RemoteSystem::update() {
	if (!msaRemote.hasMessage()) return;
	
	msaRemote.getMessage();

	pos.rot.x =	msaRemote.accelData.x*90;
	pos.rot.y = msaRemote.accelData.y*90;

	// do finger stuff
	list<ofxTuioCursor*>cursorList = msaRemote.getTuioCursors();
	list<ofxTuioCursor*>::iterator it = cursorList.begin();
	
	ofxTuioCursor*	first = *it;
	if (cursorList.size() == 2) {
		ofxTuioCursor* second = *(++it);
		touchesAngle = first->getAngleDegrees(second);
		touchesDist = first->getDistance(second);
		
		if (newCursorDetected) {
			newCursorDetected = false;
			touchesAnglePrev = touchesAngle;
			touchesDistPrev = touchesDist;
		}
		
		pos.rot.z += touchesAngle - touchesAnglePrev;
		pos.offset.z -= (touchesDist - touchesDistPrev)*4;
		
		touchesAnglePrev = touchesAngle;
		touchesDistPrev = touchesDist;
		printf("Two fingers\n");
	} else if (cursorList.size() == 1) {
		pos.offset.x += first->getXSpeed() * tuioCursorSpeedMult;
		pos.offset.y += first->getYSpeed() * tuioCursorSpeedMult;
	} //else
	//printf("No fingers or lots of fingers\n");
}

void RemoteSystem::destroy() {
	if (verbose) printf("RemoteSystem::destroy()\n");
}

//--------------------------------------------------------------
void RemoteSystem::cursorAdded(ofxTuioCursor & tuioCursor) {
	if (tuioCursor.getFingerId() < 2) {
		newCursorDetected = true;
	}
	/*
	 cout << " new cursor: " + ofToString(tuioCursor.getFingerId())+
	 " X: "+ofToString(tuioCursor.getX())+
	 " Y: "+ofToString(tuioCursor.getY())
	 << endl;
	 */
}


//--------------------------------------------------------------
void RemoteSystem::cursorRemoved(ofxTuioCursor & tuioCursor) {
	/*
	 cout << " cursor removed: " + ofToString(tuioCursor.getFingerId())+
	 " X: "+ofToString(tuioCursor.getX())+
	 " Y: "+ofToString(tuioCursor.getY())
	 << endl;
	 */
}

//--------------------------------------------------------------
void RemoteSystem::cursorUpdated(ofxTuioCursor & tuioCursor) {
	/*
	 cout << " cursor updated: " + ofToString(tuioCursor.getFingerId())+
	 " X: "+ofToString(tuioCursor.getX())+
	 " Y: "+ofToString(tuioCursor.getY())
	 << endl;
	 */
}

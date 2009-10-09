#pragma once

#include "FiducialBackedObject.h"
#include "ofxPoint2f.h"

#define FIDUCIAL_UNKNOWN_TYPE 0

FiducialBackedObject::FiducialBackedObject() : ofxMSAInteractiveObject() 
{
	ref_type = FIDUCIAL_UNKNOWN_TYPE;
	fiducial = NULL;
	
	angle = 0;
	
	rotateEvents = false;
	translateEvents = false;
	disableAllEvents();

	if (verbose) printf("FiducialBackedObject::FiducialBackedObject()\n");
}

FiducialBackedObject::~FiducialBackedObject()
{
	if (verbose) printf("FiducialBackedObject::~FiducialBackedObject()\n");
}

void FiducialBackedObject::rotateRad(float angle)
{
	this->angle = angle;
}

bool FiducialBackedObject::hitTest(int tx, int ty) 
{
	if (rotateEvents)	{ rotateMouse(&tx, &ty); }
	if (translateEvents){ translateMouse(&tx, &ty); }

	return ((tx > x) && (tx < x + width) && (ty > y) && (ty < y + height));
}

void FiducialBackedObject::translateMouse(int* mx, int* my) 
{
	*mx -= x;
	*my -= y;
}

void FiducialBackedObject::rotateMouse(int* mx, int* my) 
{
	ofxPoint2f _new(*mx, *my);
//	ofxPoint2f _ref(x+width/2, y+height/2);
	ofxPoint2f _ref(x, y);
	_new.rotateRad(-angle, _ref);
	
	*mx = _new.x;
	*my = _new.y;
}

ofMouseEventArgs* FiducialBackedObject::relocateMouseEvent(ofMouseEventArgs* e, bool do_rotate, bool do_translate) 
{
	int mx=e->x;
	int my=e->y;
	if (do_rotate)
		rotateMouse(&mx, &my);

	if (do_translate)
		translateMouse(&mx, &my);

	e->x = mx;
	e->y = my;
	return e;
}

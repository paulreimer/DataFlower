#pragma once

#include "ofxMSAInteractiveObject.h"

class FiducialBackedObject : public ofxMSAInteractiveObject {
public:
	FiducialBackedObject();
	virtual ~FiducialBackedObject();

	float angle;
	void rotateRad(float angle);

	bool hitTest(int tx, int ty); // handles translation and rotation
	bool translateEvents, rotateEvents;
	
	int fid;
	int life;
protected:
	void translateMouse(int* mx, int* my);
	void rotateMouse(int* mx, int* my);
	ofMouseEventArgs* relocateMouseEvent(ofMouseEventArgs *e, bool do_rotate = true, bool do_translate = true);

};

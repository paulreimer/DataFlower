#pragma once

#include "VideoFilter.h"
#include "testApp.h"
#include "ofxPoint2f.h"

//#include "TuiSystem.h"

extern testApp* myApp;

VideoFilter::VideoFilter() {
	config = &defaultSimpleGuiConfig;
	width = height = 0;
	rotateEvents = true;
	translateEvents = false;

	printf("VideoFilter::VideoFilter()\n");
}

VideoFilter::~VideoFilter() {
	printf("VideoFilter::~VideoFilter()\n");
	destroy();
}

void VideoFilter::setConfig(ofxSimpleGuiConfig *config) {
	this->config = config;
}

void VideoFilter::setup() {
    input.allocate(VIDEO_SIZE);

	output.allocate(VIDEO_SIZE);

	enableMouseEvents();
}

void VideoFilter::draw() {
	float posX		= 0;
	float posY		= 0;

	ofSetRectMode(OF_RECTMODE_CORNER);

	for(int i=0; i<controls.size(); i++) {
		glPushMatrix();
			glTranslatef(x+width/2, y+height/2, 0);
			glRotatef(angle*180.0/PI, 0, 0, 1.0); // must flip degrees to compensate for image flip
			glTranslatef(-width/2, -height/2, 0);

			controls[i]->draw(posX, posY);

			ofNoFill();
			ofSetColor(config->borderColor);
			glLineWidth(0.5f);
			ofRect(posX, posY, controls[i]->width, controls[i]->height);
			posY += controls[i]->height + config->padding.y;

		glPopMatrix();
	}
}

ofxSimpleGuiControl *VideoFilter::addControl(ofxSimpleGuiControl* control) {
	// VideoFilter calls the shots
	control->disableAllEvents();
	controls.push_back(control);
	height += control->height + config->padding.y;
	width = MAX(control->width, width);
	return control;
}

ofxSimpleGuiContent *VideoFilter::addContent(string name, ofBaseDraws *content, float fixwidth) {
	if(fixwidth == -1) fixwidth = config->gridSize.x - config->padding.x;
	return (ofxSimpleGuiContent *)addControl(new ofxSimpleGuiContent(name, content, fixwidth));
}

ofxSimpleGuiButton *VideoFilter::addButton(string name, bool *value) {
	return (ofxSimpleGuiButton *)addControl(new ofxSimpleGuiButton(name, value));
}

ofxSimpleGuiFPSCounter *VideoFilter::addFPSCounter() {
	return (ofxSimpleGuiFPSCounter *)addControl(new ofxSimpleGuiFPSCounter());
}

ofxSimpleGuiSliderInt *VideoFilter::addSlider(string name, int *value, int min, int max) {
	return (ofxSimpleGuiSliderInt *)addControl(new ofxSimpleGuiSliderInt(name, value, min, max, 0));
}

ofxSimpleGuiSliderFloat *VideoFilter::addSlider(string name, float *value, float min, float max, float smoothing) {
	return (ofxSimpleGuiSliderFloat *)addControl(new ofxSimpleGuiSliderFloat(name, value, min, max, smoothing));
}

ofxSimpleGuiSlider2d *VideoFilter::addSlider2d(string name, ofPoint* value, float xmin, float xmax, float ymin, float ymax) {
	return (ofxSimpleGuiSlider2d *)addControl(new ofxSimpleGuiSlider2d(name, value, xmin, xmax, ymin, ymax));
}

ofxSimpleGuiTitle *VideoFilter::addTitle(string name, bool *value) {
	return (ofxSimpleGuiTitle *)addControl(new ofxSimpleGuiTitle(name, value));
}

ofxSimpleGuiToggle *VideoFilter::addToggle(string name, bool *value) {
	return (ofxSimpleGuiToggle *)addControl(new ofxSimpleGuiToggle(name, value));
}

void VideoFilter::update() {
	for(int i=0; i<controls.size(); i++) controls[i]->update();
}

void VideoFilter::_mouseMoved(ofMouseEventArgs &e) {
	ofMouseEventArgs new_e = e;
	relocateMouseEvent(&new_e);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseMoved(new_e);
	
	FiducialBackedObject::_mouseMoved(e);
}

void VideoFilter::_mousePressed(ofMouseEventArgs &e) {
	ofMouseEventArgs new_e = e;
	relocateMouseEvent(&new_e);
	for(int i=0; i<controls.size(); i++) controls[i]->_mousePressed(new_e);

	FiducialBackedObject::_mousePressed(e);
}

void VideoFilter::_mouseDragged(ofMouseEventArgs &e) {
	ofMouseEventArgs new_e = e;
	relocateMouseEvent(&new_e);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseDragged(new_e);

	FiducialBackedObject::_mouseDragged(e);
}

void VideoFilter::_mouseReleased(ofMouseEventArgs &e) {
	ofMouseEventArgs new_e = e;
	relocateMouseEvent(&new_e);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseReleased(new_e);
	
	FiducialBackedObject::_mouseReleased(e);
}

void VideoFilter::_keyPressed(ofKeyEventArgs &e) {
	for(int i=0; i<controls.size(); i++) controls[i]->_keyPressed(e);
}

void VideoFilter::_keyReleased(ofKeyEventArgs &e) {
	for(int i=0; i<controls.size(); i++) controls[i]->_keyReleased(e);
}

void VideoFilter::onPress(int mx, int my, int button) {
	// save the offset of where the mouse was clicked...
	// ...relative to the position of the object
	bool content = false;
	for(int i=0; i<controls.size(); i++) {
		if (controls[i]->controlType == "Content"
			&& controls[i]->isMouseDown()) {
			content = true;
			break;
		}
	}
	if (content) { //ahhh
		saveX = mx - x;
		saveY = my - y;
	}
}

void VideoFilter::onDragOver(int mx, int my, int button) {
	bool content = false;
	for(int i=0; i<controls.size(); i++) {
		if (controls[i]->controlType == "Content"
			&& controls[i]->isMouseDown()) {
			content = true;
			break;
		}
	}
	
	if (content) { //ahhh
		x = mx - saveX;    // update x position
		y = my - saveY;    // update mouse y position
	}
}

void VideoFilter::fiducialDetected(ofxFiducial &fiducial) {
	printf("Detected fiducial #%d\n", fiducial.getId());
}

void VideoFilter::destroy() {
	printf("VideoFilter::destroy()\n");
}

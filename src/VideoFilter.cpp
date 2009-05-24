#pragma once

#include "VideoFilter.h"
#include "testApp.h"
#include "ofxPoint2f.h"

extern testApp* myApp;

VideoFilter::VideoFilter(string name) : ofxSimpleGuiPage(name) {
	width = height = 0;
	angle = 0;
	disableAllEvents();
	
	printf("VideoFilter::VideoFilter()\n");
}

VideoFilter::~VideoFilter() {
	printf("VideoFilter::~VideoFilter()\n");
	destroy();
}

void VideoFilter::setup() {
	ofxSimpleGuiPage::setup();
    input.allocate(VIDEO_SIZE);

	output.allocate(VIDEO_SIZE);
	
//	enableMouseEvents();
}

void VideoFilter::update() {
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

void VideoFilter::repositionMouseEvent(ofMouseEventArgs* e) {
	ofxPoint2f _ref(x, y);
	ofxPoint2f _new(e->x, e->y);
	_new.rotateRad(-angle, _ref);
	e->x = _new.x;// - x;
	e->y = _new.y;//- y;
}

void VideoFilter::mouseMoved(ofMouseEventArgs &e) {
	printf("Before: (%d, %d)\n", e.x, e.y);
	ofMouseEventArgs e_new = e;
	repositionMouseEvent(&e_new);
	printf("After: (%d, %d)\n", e_new.x, e_new.y);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseMoved(e_new);
}

void VideoFilter::mousePressed(ofMouseEventArgs &e) {
	ofMouseEventArgs e_new = e;
	repositionMouseEvent(&e_new);
	for(int i=0; i<controls.size(); i++) controls[i]->_mousePressed(e_new);
}

void VideoFilter::mouseDragged(ofMouseEventArgs &e) {
	ofMouseEventArgs e_new = e;
	repositionMouseEvent(&e_new);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseDragged(e_new);
}

void VideoFilter::mouseReleased(ofMouseEventArgs &e) {
	ofMouseEventArgs e_new = e;
	repositionMouseEvent(&e_new);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseReleased(e_new);
}

ofxSimpleGuiControl *VideoFilter::addControl(ofxSimpleGuiControl* control) {
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

void VideoFilter::onPress(int mx, int my, int button) {
	// save the offset of where the mouse was clicked...
	// ...relative to the position of the object
	saveX = mx - x;
	saveY = my - y;
}

void VideoFilter::onDragOver(int mx, int my, int button) {
	x = mx - saveX;    // update x position
	y = my - saveY;    // update mouse y position
//	setPos(x, y);
	printf("Got pos: (%f, %f)\n", x, y);
	ofSetColor(255, 0, 0);
	ofFill();
	ofCircle(x, y, 5.0);
}

void VideoFilter::rotateRad(float _angle){
	angle=_angle;
	printf("Rotate by %f rad.\n", _angle);
}

void VideoFilter::destroy() {
	printf("VideoFilter::destroy()\n");
}

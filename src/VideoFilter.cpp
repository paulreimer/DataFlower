#pragma once

#include "VideoFilter.h"
#include "testApp.h"

extern testApp* myApp;

VideoFilter::VideoFilter(string name) : ofxSimpleGuiPage(name) {
	width = height = 0;
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
	
	addContent("Output", &output);
	enableMouseEvents();
}

void VideoFilter::update() {
}

void VideoFilter::draw() {
	float posX		= 0;
	float posY		= 0;
	
	ofSetRectMode(OF_RECTMODE_CORNER);
	
	for(int i=0; i<controls.size(); i++) {
		float controlX = posX + x;
		float controlY = posY + y;
		
		controls[i]->draw(controlX, controlY);
		ofNoFill();
		ofSetColor(config->borderColor);
		glLineWidth(0.5f);
		ofRect(controlX, controlY, controls[i]->width, controls[i]->height);
		posY = getNextY(posY + controls[i]->height + config->padding.y);
		
	}
}

ofxSimpleGuiControl *VideoFilter::addControl(ofxSimpleGuiControl* control) {
	controls.push_back(control);
	height += control->height + config->padding.y;
	width = MAX(control->width, width);
	return control;
}

ofxSimpleGuiButton *VideoFilter::addButton(string name, bool *value) {
	return (ofxSimpleGuiButton *)addControl(new ofxSimpleGuiButton(name, value));
}

ofxSimpleGuiContent *VideoFilter::addContent(string name, ofBaseDraws *content, float fixwidth) {
	if(fixwidth == -1) fixwidth = config->gridSize.x - config->padding.x;
	return (ofxSimpleGuiContent *)addControl(new ofxSimpleGuiContent(name, content, fixwidth));
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
	
	ofxSimpleGuiPage::onPress(mx, my, button);
}

void VideoFilter::onDragOver(int mx, int my, int button) {
	x = mx - saveX;    // update x position
	y = my - saveY;    // update mouse y position
	ofxSimpleGuiPage::onDragOver(mx, my, button);
}

void VideoFilter::destroy() {
	printf("VideoFilter::destroy()\n");
}

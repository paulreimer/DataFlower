#pragma once

#include "VideoFilter.h"
#include "testApp.h"

extern testApp* myApp;

VideoFilter::VideoFilter()
{
//	ref_type = FIDUCIAL_FILTER_TYPE;

	config = &defaultSimpleGuiConfig;
	width = height = 0;
	rotateEvents = true;
	translateEvents = false;

	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("VideoFilter::VideoFilter()\n");
	
	disableAppEvents();
	enableDrawEvents();
	enableMouseEvents();
}


VideoFilter::~VideoFilter()
{
	if (verbose) printf("VideoFilter::~VideoFilter()\n");
	destroy();
}

void VideoFilter::setConfig(ofxSimpleGuiConfig* config) 
{
	this->config = config;

	for(int i=0; i<controls.size(); i++)
		controls[i]->setConfig(this->config);
}

void VideoFilter::draw()/* const */
{
	float posX		= 0;
	float posY		= 0;

	ofSetRectMode(OF_RECTMODE_CORNER);

	glPushMatrix();

	glTranslatef(x, y, 0);
	glRotatef(angle*180.0/PI, 0, 0, 1.0); // must flip degrees to compensate for image flip

	for(int i=0; i<controls.size(); i++) {
		// TODO: should be in MODEL_VIEW?

		controls[i]->draw(posX, posY);

		ofNoFill();
		ofSetColor(config->borderColor);
		glLineWidth(0.5f);
		ofRect(posX, posY, controls[i]->width, controls[i]->height);
		posY += controls[i]->height + config->padding.y;

	}
	glPopMatrix();
}

void VideoFilter::setPos(float _x, float _y, float _angle) 
{
	ofxMSAInteractiveObject::setPos(_x, _y);
	rotateRad(_angle);
}

ofxSimpleGuiControl &VideoFilter::addControl(ofxSimpleGuiControl& control) 
{
	// VideoFilter calls the shots
	control.disableAllEvents();
	control.setConfig(this->config);
	
	controls.push_back(&control);
	height += control.height + config->padding.y;
	width = MAX(control.width, width);
	return control;
}

ofxSimpleGuiContent &VideoFilter::addContent(std::string name, ofBaseDraws &content, float fixwidth) 
{
	if(fixwidth == -1) fixwidth = config->gridSize.x - config->padding.x;
	return (ofxSimpleGuiContent &)addControl(* new ofxSimpleGuiContent(name, content, fixwidth));
}

ofxSimpleGuiButton &VideoFilter::addButton(std::string name, bool &value) 
{
	return (ofxSimpleGuiButton &)addControl(* new ofxSimpleGuiButton(name, value));
}

ofxSimpleGuiSliderInt &VideoFilter::addSlider(std::string name, int &value, int min, int max, float smoothing) 
{
	return (ofxSimpleGuiSliderInt &)addControl(* new ofxSimpleGuiSliderInt(name, value, min, max, smoothing));
}

ofxSimpleGuiSliderByte &VideoFilter::addSlider(std::string name, byte &value, byte min, byte max, float smoothing)
{
	return (ofxSimpleGuiSliderByte &)addControl(* new ofxSimpleGuiSliderByte(name, value, min, max, smoothing));
}

ofxSimpleGuiSliderFloat &VideoFilter::addSlider(std::string name, float &value, float min, float max, float smoothing) 
{
	return (ofxSimpleGuiSliderFloat &)addControl(* new ofxSimpleGuiSliderFloat(name, value, min, max, smoothing));
}

ofxSimpleGuiSliderDouble &VideoFilter::addSlider(std::string name, double &value, double min, double max, double smoothing) 
{
	return (ofxSimpleGuiSliderDouble &)addControl(* new ofxSimpleGuiSliderDouble(name, value, min, max, smoothing));
}

ofxSimpleGuiSlider2d &VideoFilter::addSlider2d(std::string name, ofPoint& value, float xmin, float xmax, float ymin, float ymax) 
{
	return (ofxSimpleGuiSlider2d &)addControl(* new ofxSimpleGuiSlider2d(name, value, xmin, xmax, ymin, ymax));
}

ofxSimpleGuiTitle &VideoFilter::addTitle(std::string name) 
{
	return (ofxSimpleGuiTitle &)addControl(* new ofxSimpleGuiTitle(name));
}

ofxSimpleGuiToggle &VideoFilter::addToggle(std::string name, bool &value) 
{
	return (ofxSimpleGuiToggle &)addControl(* new ofxSimpleGuiToggle(name, value));
}

ofxSimpleGuiFPSCounter &VideoFilter::addFPSCounter() 
{
	return (ofxSimpleGuiFPSCounter &)addControl(* new ofxSimpleGuiFPSCounter());
}

void VideoFilter::_mouseMoved(ofMouseEventArgs &e) 
{
	ofMouseEventArgs new_e = e;
	relocateMouseEvent(&new_e);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseMoved(new_e);
	
	ofxMSAInteractiveObject::_mouseMoved(e);
}

void VideoFilter::_mousePressed(ofMouseEventArgs &e) 
{
	ofMouseEventArgs new_e = e;
	relocateMouseEvent(&new_e);
	for(int i=0; i<controls.size(); i++) controls[i]->_mousePressed(new_e);

	ofxMSAInteractiveObject::_mousePressed(e);
}

void VideoFilter::_mouseDragged(ofMouseEventArgs &e) 
{
	ofMouseEventArgs new_e = e;
	relocateMouseEvent(&new_e);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseDragged(new_e);

	ofxMSAInteractiveObject::_mouseDragged(e);
}

void VideoFilter::_mouseReleased(ofMouseEventArgs &e) 
{
	ofMouseEventArgs new_e = e;
	relocateMouseEvent(&new_e);
	for(int i=0; i<controls.size(); i++) controls[i]->_mouseReleased(new_e);
	
	ofxMSAInteractiveObject::_mouseReleased(e);
}

void VideoFilter::_keyPressed(ofKeyEventArgs &e) 
{
	for(int i=0; i<controls.size(); i++) controls[i]->_keyPressed(e);
}

void VideoFilter::_keyReleased(ofKeyEventArgs &e) 
{
	for(int i=0; i<controls.size(); i++) controls[i]->_keyReleased(e);
}

void VideoFilter::onPress(int mx, int my, int button) 
{
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

void VideoFilter::onDragOver(int mx, int my, int button) 
{
	bool content = false;
	for(int i=0; i<controls.size(); i++) {
		if (controls[i]->controlType == "Content"
			&& controls[i]->isMouseDown()) {
			content = true;
			break;
		}
	}
	
	if (content) { //ahhh
//		x = mx - saveX;    // update x position
//		y = my - saveY;    // update mouse y position
		setPos(mx - saveX, my - saveY, angle);
	}
}

void VideoFilter::destroy() 
{
	disableMouseEvents();
	disableAppEvents();

	if (verbose) printf("VideoFilter::destroy()\n");
}

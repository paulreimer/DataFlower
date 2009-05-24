#pragma once

#include "VideoPipeline.h"
#include "testApp.h"

extern testApp* myApp;

VideoPipeline::VideoPipeline() : ofxSimpleGuiToo() {
	config			= new ofxSimpleGuiConfig();
	config->offset.set(0,0);
	config->gridSize.x = 120;

//	guiFocus		= 0;
	
	verbose			= true;
	
	xmlFile			= OFX_SIMPLEGUITOO_XML_NAME;
	doSave			= false;
	doSaveBackup	= false;
	changePage		= false;
	
	setAutoSave(false);
	setDraw(true);
	setPage(1);
	printf("VideoPipeline::VideoPipeline()\n");
}

VideoPipeline::~VideoPipeline() {
	printf("VideoPipeline::~VideoPipeline()\n");
	destroy();
}

void VideoPipeline::updateListeners() {
//	ofAddListener(ofEvents.update, this, &ofxSimpleGuiToo::update);
	ofAddListener(ofEvents.mousePressed, this, &VideoPipeline::mousePressed);
	ofAddListener(ofEvents.mouseMoved, this, &VideoPipeline::mouseMoved);
	ofAddListener(ofEvents.mouseDragged, this, &VideoPipeline::mouseDragged);
	ofAddListener(ofEvents.mouseReleased, this, &VideoPipeline::mouseReleased);
	
	ofAddListener(ofEvents.keyPressed, this, &VideoPipeline::keyPressed);
	ofAddListener(ofEvents.keyReleased, this, &VideoPipeline::keyReleased);
}

void VideoPipeline::setup() {
	input.allocate(VIDEO_SIZE);
	output.allocate(VIDEO_SIZE);

//	outputPage = addPage("Input");

//	outputPage = addPage("Output");
//	outputPage->addContent("Result", &output);
	ofxSimpleGuiToo::removeListeners();
	updateListeners();

	for (int i=0; i<pages.size(); i++) {
		filter(i)->setup();
		filter(i)->setPos((i+1)*VIDEO_WIDTH,60);
	}

}

void VideoPipeline::update() {
	pages.front()->input = input;
	pages.front()->update();

	int i;
	for (i = 1; i<pages.size(); i++) {
		filter(i)->input = filter(i-1)->output;
		filter(i)->update();		
	}

	output = pages.back()->output;
}

VideoFilter *VideoPipeline::filter(int i) {
	return pages.at(i);
}

VideoFilter *VideoPipeline::filter(string name) {
	VideoFilter *page;
	for(int i=1; i<pages.size(); i++) if(name.compare(filter(i)->name) == 0) return filter(i);
	return NULL;
}

VideoFilter *VideoPipeline::addFilter(VideoFilter* filter) {
	pages.push_back(filter);
	return filter;
}

void VideoPipeline::draw() {
	if(!doDraw) return;
	glDisableClientState(GL_COLOR_ARRAY);
	
	ofLine(256, 210,
		   filter(0)->x, filter(0)->y+filter(0)->height/2);
	for (int i = 0; i<pages.size(); i++) {
		ofNoFill();
        ofSetColor(0xFFFFFF);
		ofSetLineWidth(5.0);
        ofRect(filter(i)->x, filter(i)->y, filter(i)->width, filter(i)->height - config->padding.y);
		
		if (i > 0)
			ofLine(filter(i-1)->x+filter(i-1)->width, filter(i-1)->y+filter(i-1)->height/2,
				filter(i)->x, filter(i)->y+filter(i)->height/2);

		filter(i)->draw();
	}
}

void VideoPipeline::destroy() {
	printf("VideoPipeline::destroy()\n");
}

void VideoPipeline::mouseMoved(ofMouseEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator f;
	for (f=pages.rbegin() ; f < pages.rend(); ++f ) {
		(*f)->mouseMoved(e);
//		if ((*f)->isMouseOver())
//			return;
	}
}

void VideoPipeline::mousePressed(ofMouseEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator f;
	for (f=pages.rbegin() ; f < pages.rend(); ++f ) {
		(*f)->mousePressed(e);
		//		if ((*f)->isMouseOver())
		//			return;
	}
}

void VideoPipeline::mouseDragged(ofMouseEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator f;
	for (f=pages.rbegin() ; f < pages.rend(); ++f ) {
		
		(*f)->mouseDragged(e);
		//		if ((*f)->isMouseOver())
		//			return;
	}
}

void VideoPipeline::mouseReleased(ofMouseEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator f;
	for (f=pages.rbegin() ; f < pages.rend(); ++f ) {
		(*f)->mouseReleased(e);
		//		if ((*f)->isMouseOver())
		//			return;
	}
	if(doAutoSave) saveToXML(xmlFile);
}

void VideoPipeline::keyPressed(ofKeyEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator f;
	for (f=pages.rbegin() ; f < pages.rend(); ++f ) {		
		(*f)->keyPressed(e);
	}
	if(doAutoSave) saveToXML(xmlFile);
}

void VideoPipeline::keyReleased(ofKeyEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator f;
	for (f=pages.rbegin() ; f < pages.rend(); ++f ) {		
		(*f)->keyReleased(e);
	}
}

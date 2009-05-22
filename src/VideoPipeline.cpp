#pragma once

#include "VideoPipeline.h"
#include "testApp.h"

extern testApp* myApp;

VideoPipeline::VideoPipeline() {
	config			= &defaultSimpleGuiConfig;
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

void VideoPipeline::setup() {
	input.allocate(VIDEO_SIZE);
	output.allocate(VIDEO_SIZE);

//	outputPage = addPage("Input");

//	outputPage = addPage("Output");
//	outputPage->addContent("Result", &output);

	vector<VideoFilter*>::iterator it;
	VideoFilter* cur;
//	for(int i = pages.begin(); it != pages.end(); it++) {
	for (int i=0; i<pages.size(); i++) {
//		cur = *it;
		pages[i]->setup();
		pages[i]->setPos((i+1)*VIDEO_WIDTH, (i+1)*VIDEO_HEIGHT);
	}
}

void VideoPipeline::addListeners() {
//	ofAddListener(ofEvents.update, this, &VideoPipeline::update);
	
	ofAddListener(ofEvents.mousePressed, this, &VideoPipeline::mousePressed);
	ofAddListener(ofEvents.mouseMoved, this, &VideoPipeline::mouseMoved);
	ofAddListener(ofEvents.mouseDragged, this, &VideoPipeline::mouseDragged);
	ofAddListener(ofEvents.mouseReleased, this, &VideoPipeline::mouseReleased);
	
	ofAddListener(ofEvents.keyPressed, this, &VideoPipeline::keyPressed);
	ofAddListener(ofEvents.keyReleased, this, &VideoPipeline::keyReleased);
}


void VideoPipeline::removeListeners() {
//	ofRemoveListener(ofEvents.update, this, &VideoPipeline::update);
	
	ofRemoveListener(ofEvents.mousePressed, this, &VideoPipeline::mousePressed);
	ofRemoveListener(ofEvents.mouseMoved, this, &VideoPipeline::mouseMoved);
	ofRemoveListener(ofEvents.mouseDragged, this, &VideoPipeline::mouseDragged);
	ofRemoveListener(ofEvents.mouseReleased, this, &VideoPipeline::mouseReleased);
	
	ofRemoveListener(ofEvents.keyPressed, this, &VideoPipeline::keyPressed);
	ofRemoveListener(ofEvents.keyReleased, this, &VideoPipeline::keyReleased);
}

void VideoPipeline::update() {
	pages.front()->input = input;
	pages.front()->update();

	int i;
	for (i = 1; i<pages.size(); i++) {
		pages[i]->input = pages[i-1]->output;
		pages[i]->update();		
	}

	output = pages.back()->output;
}

void VideoPipeline::draw() {
	if(!doDraw) return;
	glDisableClientState(GL_COLOR_ARRAY);
	
	ofLine(256, 210,
		   pages[0]->x, pages[0]->y+pages[0]->height/2);
	for (int i = 0; i<pages.size(); i++) {
		ofNoFill();
        ofSetColor(0xFFFFFF);
        ofRect(pages[i]->x, pages[i]->y, pages[i]->width, pages[i]->height);
		
		if (i)
			ofLine(pages[i-1]->x+pages[i-1]->width, pages[i-1]->y+pages[i-1]->height/2,
				pages[i]->x, pages[i]->y+pages[i]->height/2);

		pages[i]->draw();
	}
}

void VideoPipeline::destroy() {
	printf("VideoPipeline::destroy()\n");
}

void VideoPipeline::setDraw(bool b) {
	doDraw = b;
	if(doDraw) addListeners();
	else removeListeners();	
	if(doAutoSave) saveToXML(xmlFile);
}

void VideoPipeline::mouseMoved(ofMouseEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator rit;
	for (rit=pages.rbegin() ; rit < pages.rend(); ++rit ) {
		(*rit)->mouseMoved(e);
		if ((*rit)->isMouseOver())
			return;
	}
}

void VideoPipeline::mousePressed(ofMouseEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator rit;
	for (rit=pages.rbegin() ; rit < pages.rend(); ++rit ) {
		(*rit)->mousePressed(e);
		if ((*rit)->isMouseOver())
			return;
	}
}

void VideoPipeline::mouseDragged(ofMouseEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator rit;
	for (rit=pages.rbegin() ; rit < pages.rend(); ++rit ) {
		(*rit)->mouseDragged(e);
		if ((*rit)->isMouseOver())
			return;
	}
}

void VideoPipeline::mouseReleased(ofMouseEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator rit;
	for (rit=pages.rbegin() ; rit < pages.rend(); ++rit ) {
		(*rit)->mouseReleased(e);
		if ((*rit)->isMouseOver())
			return;
	}
	if(doAutoSave) saveToXML(xmlFile);
}

void VideoPipeline::keyPressed(ofKeyEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator rit;
	for (rit=pages.rbegin() ; rit < pages.rend(); ++rit ) {
		(*rit)->keyPressed(e);
	}
	if(doAutoSave) saveToXML(xmlFile);
}

void VideoPipeline::keyReleased(ofKeyEventArgs &e) {
	vector<VideoFilter*>::reverse_iterator rit;
	for (rit=pages.rbegin() ; rit < pages.rend(); ++rit ) {
		(*rit)->keyReleased(e);
	}
}

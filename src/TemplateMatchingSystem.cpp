#pragma once

#include "TemplateMatchingSystem.h"
#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

TemplateMatchingSystem::TemplateMatchingSystem() {
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("TemplateMatchingSystem::TemplateMatchingSystem()\n");
}

TemplateMatchingSystem::~TemplateMatchingSystem() {
	if (verbose) printf("TemplateMatchingSystem::~TemplateMatchingSystem()\n");
	destroy();
}

void TemplateMatchingSystem::setup(){
	pipe.addFilter(new BackgroundSubtractionFilter());
//	pipe->addFilter(&templCaptureFilter);
	
	myApp->videoSystem.addPipeline(&pipe);
}

//--------------------------------------------------------------
void TemplateMatchingSystem::update(){
}
//--------------------------------------------------------------
void TemplateMatchingSystem::draw(){
}

void TemplateMatchingSystem::destroy() {
	if (verbose) printf("TemplateMatchingSystem::destroy()\n");
}

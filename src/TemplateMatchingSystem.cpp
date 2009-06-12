#pragma once

#include "TemplateMatchingSystem.h"
#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

TemplateMatchingSystem::TemplateMatchingSystem() {
	printf("TemplateMatchingSystem::TemplateMatchingSystem()\n");
}

TemplateMatchingSystem::~TemplateMatchingSystem() {
	printf("TemplateMatchingSystem::~TemplateMatchingSystem()\n");
	destroy();
}

void TemplateMatchingSystem::setup(){
	VideoPipeline* pipe = new VideoPipeline();
	pipe->addFilter(new BackgroundSubtractionFilter());
//	pipe->addFilter(&templCaptureFilter);
	
	myApp->videoSystem.addPipeline(pipe);
}

//--------------------------------------------------------------
void TemplateMatchingSystem::update(){
}
//--------------------------------------------------------------
void TemplateMatchingSystem::draw(){
}

void TemplateMatchingSystem::destroy() {
	printf("TemplateMatchingSystem::destroy()\n");
}

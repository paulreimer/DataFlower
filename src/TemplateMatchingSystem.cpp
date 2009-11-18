#include "TemplateMatchingSystem.h"
#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

TemplateMatchingSystem::TemplateMatchingSystem()
{
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("TemplateMatchingSystem::TemplateMatchingSystem()\n");
}

TemplateMatchingSystem::~TemplateMatchingSystem()
{
	if (verbose) printf("TemplateMatchingSystem::~TemplateMatchingSystem()\n");
	destroy();
}

void TemplateMatchingSystem::setup()
{	
	myApp->videoSystem.addPipeline(&pipe);

	pipe.addFilter(new BackgroundSubtractionFilter());
//	pipe->addFilter(&templCaptureFilter);
}

//--------------------------------------------------------------
void TemplateMatchingSystem::update()
{
}
//--------------------------------------------------------------
void TemplateMatchingSystem::draw() const
{
}

void TemplateMatchingSystem::destroy() 
{
	if (verbose) printf("TemplateMatchingSystem::destroy()\n");
}

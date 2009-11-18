#include "VideoFilter.h"
#include "testApp.h"

VideoFilter::VideoFilter()
{
	config = &defaultSimpleGuiConfig;
//	width = height = 0;

//	disableAppEvents();
	GuiPage::setup();
	
	enableDrawEvents();
	enableMouseEvents();

	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("VideoFilter::VideoFilter()\n");	
}


VideoFilter::~VideoFilter()
{
	if (verbose) printf("VideoFilter::~VideoFilter()\n");
	destroy();
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
	
	if (content) //ahhh
		setPos(mx - saveX, my - saveY);
}


void VideoFilter::destroy() 
{
	if (verbose) printf("VideoFilter::destroy()\n");
}

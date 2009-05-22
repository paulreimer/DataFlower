#pragma once

#include "ofMain.h"

#define USE_TUIO
#define USE_GUI
#define USE_PHYSICS
#define USE_VIDEO
#define USE_TUI

#ifdef USE_TUIO
#include "ofxTuio.h"
#define tuioCursorSpeedMult				0.02f	// the iphone screen is so small, easy to rack up huge velocities! need to scale down 
#endif

#ifdef USE_VIDEO
#include "VideoSystem.h"
#endif

#ifdef USE_GUI
#include "GuiSystem.h"
#endif

#ifdef USE_TUI
	#include "TuiSystem.h"
#endif

#include "settings.h"

//#include "julius-simple.h"

class testApp : public ofSimpleApp{
public:
#ifdef USE_TUIO
	myTuioClient tuioClient;
#endif
	
#ifdef USE_GUI
	GuiSystem guiSystem;
#endif

#ifdef USE_TUI
	TuiSystem tuiSystem;
#endif
	
#ifdef USE_VIDEO
	VideoSystem videoSystem;
#endif
	
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);

	void windowResized(int w, int h);
	
	int pmouseX, pmouseY;

	// cache these for slightly better performance
	struct WindowSettings {
		int		width;
		int		height;
		float	invWidth;
		float	invHeight;
		float	aspectRatio;
		float	aspectRatio2;
	} window;

};

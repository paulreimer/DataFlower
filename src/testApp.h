#pragma once

#include "ofMain.h"
#include "settings.h"

#ifdef USE_VIDEO
#include "VideoSystem.h"
#endif

#ifdef USE_GUI
#include "GuiSystem.h"
#endif

#ifdef USE_TUI
	#include "TuiSystem.h"
	#include "ofxPoint2f.h"
#endif

#ifdef USE_SPEECH_TO_TEXT
	#include "SpeechSystem.h"
#endif

#ifdef USE_DATA
	#include "DataSystem.h"
#endif

#ifdef USE_TEMPLATE_MATCHING
	#include "TemplateMatchingSystem.h"
#endif

#ifdef USE_GPU_VIS
	#include "RenderSystem.h"
#endif

#ifdef USE_REMOTE_CONTROL
	#include "ofxRemoteControl.h"
#endif

#ifdef USE_OPENCL
	#include "ofxClScheduler.h"
#endif

//#include "ofxFiducial.h"
//#include "VideoPipeline.h"

class testApp : public ofSimpleApp 
{
public:
	virtual ~testApp();

#ifdef USE_OSC
	ofxOscReceiver  osc_in;
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

#ifdef USE_SPEECH_TO_TEXT
	SpeechSystem speechSystem;
#endif

#ifdef USE_DATA
	DataSystem dataSystem;
#endif
	
#ifdef USE_TEMPLATE_MATCHING
	TemplateMatchingSystem templSystem;
#endif	

#ifdef USE_GPU_VIS
	RenderSystem renderSystem;
#endif

#ifdef USE_REMOTE_CONTROL
	ofxRemoteControl remoteSystem;
#endif

#ifdef USE_OPENCL
	ofxClScheduler clScheduler;
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

	struct AppSettings {
		float	lerpSpeed;
	} settings;
};
/*
	void fiducialFound(ofxFiducial &fiducial);
	void fiducialLost(ofxFiducial &fiducial);
	void fiducialUpdated(ofxFiducial &fiducial);
};

ofxPoint2f intersects(ofxPoint2f origin, ofxPoint2f endpoint, double angle,
					  ofxPoint2f box_origin, double box_angle, int box_w, int box_h);

ofxPoint2f intersects_window_edge(ofxPoint2f origin, double angle);
ofxPoint2f intersects_window_edge(ofxPoint2f origin, ofxPoint2f endpoint, double angle);
*/
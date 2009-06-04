#pragma once

#include "ofMain.h"
#include "settings.h"
#include "msaColor.h"

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

#ifdef USE_SPEECH_TO_TEXT
	#include "SpeechSystem.h"
#endif

class testApp : public ofSimpleApp{
public:
	virtual ~testApp();

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

#ifdef USE_SPEECH_TO_TEXT
	SpeechSystem speechSystem;
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
		ofPoint rot;
		float	frame_spacing;
		float	zoom;
		int		numVertices;
		ofPoint render_offset;
		float	lerpSpeed;
		int		gl_draw_mode;
	} settings;
	
	void saveTuioCursorPos(ofxTuioCursor &cursor);
	
	GLuint			vbo[2];

	float			pos[MAX_VERTICES][3];
	float			colors[MAX_VERTICES][4];
	msaColor		frameColor;
	
	ofxOscReceiver  osc_in;

	int numFrames;
	int numVerts, numVertsPrev;
	ContourFindingFilter contourFilter;
	
	ofPoint iPhoneAccel;
	int iPhoneOrientation, iPhoneHomeButton;
};


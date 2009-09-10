#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#include "ofxSimpleGuiToo.h"

#include "settings.h"

#ifdef USE_MSAREMOTE
#include "msaRemoteClient.h"
#endif

#ifdef USE_TOON
#include <TooN/TooN.h>
#include <TooN/se3.h>
#endif

class RemoteSystem : public ofxMSAInteractiveObject {
public:
	RemoteSystem();
	virtual ~RemoteSystem();
	
	void setup();
	void update();
	
	void destroy();

#ifdef USE_MSAREMOTE
	msaRemoteClient msaRemote;

	void cursorAdded(ofxTuioCursor& tuioCursor);
	void cursorRemoved(ofxTuioCursor& tuioCursor);
	void cursorUpdated(ofxTuioCursor& tuioCursor);
#endif
	
	struct RemotePosition {
		ofPoint rot;
		ofPoint offset;
	} pos;
	
	struct RemoteSettings {
		float	lerpSpeed;
	} settings;
	
	float touchesAnglePrev, touchesAngle;
	float touchesDistPrev, touchesDist;
	bool newCursorDetected;

#ifdef USE_TOON
	TooN::SE3<> se3FromRemote;
#endif
	
	bool verbose;
};

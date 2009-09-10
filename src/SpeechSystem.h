#pragma once

#include "ofMain.h"
#include "ofxThread.h"
#include "ofxMSAInteractiveObject.h"
#include "ofxDraggable.h"

extern "C" {
	#include <julius/juliuslib.h>
}

#define JULIUS_DEBUG

class SpeechSystem : public ofxThread, public ofxMSAInteractiveObject {
public:
	SpeechSystem();
	virtual ~SpeechSystem();

	void setup();

	void threadedFunction();
	
	void destroy();

	class DetectedSpeech : public ofBaseDraws {
	public:
		string sentences;
		float width, height;
		float getWidth() { return width; }
		float getHeight() { return height; }
		
		void draw(float x, float y, float w, float h) {
			ofDrawBitmapString(sentences, x, y);
		}
		void draw(float x, float y) {
			draw(x, y, width, height);
		}
	} detected;
	
	ofxDraggable lastSentDragger;

	bool verbose;
private:
	Jconf *jconf;
	string jconf_filename;

	Recog *recog;
};

static void recready(Recog *recog, void *speech_sys);
static void recstart(Recog *recog, void *speech_sys);
static void recdone(Recog *recog, void *speech_sys);

void debug_julius_result(RecogProcess *r);

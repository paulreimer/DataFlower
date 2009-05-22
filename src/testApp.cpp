#include "testApp.h"

#pragma mark App callbacks

//--------------------------------------------------------------
void testApp::setup() {
	// initialize stuff according to current window size
	windowResized(ofGetWidth(), ofGetHeight());	
	
	ofBackground(0, 0, 0);
	ofSetBackgroundAuto(true);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

#ifdef USE_TUIO
	tuioClient.start(12345);
#endif

#ifdef USE_VIDEO
	videoSystem.setup();
#endif

#ifdef USE_GUI
	guiSystem.setup();
#endif

#ifdef USE_TUI
	tuiSystem.setup();
#endif
}


//--------------------------------------------------------------
void testApp::update(){
#ifdef USE_TUIO
	tuioClient.getMessage();
	
	// do finger stuff
	list<ofxTuioCursor*>cursorList = tuioClient.getTuioCursors();
	for(list<ofxTuioCursor*>::iterator it=cursorList.begin(); it != cursorList.end(); it++) {
		ofxTuioCursor *tcur = (*it);
        float vx = tcur->getXSpeed() * tuioCursorSpeedMult;
        float vy = tcur->getYSpeed() * tuioCursorSpeedMult;
//        physicsSystem.addToFluid(tcur->getX(), tcur->getY(), vx, vy);
    }
#endif
#ifdef USE_VIDEO
	videoSystem.update();
#endif

	// save old mouse position (openFrameworks doesn't do this automatically like processing does)
	pmouseX = mouseX;
	pmouseY = mouseY;

#ifdef USE_TUI	
	tuiSystem.grayImage = videoSystem.colorImg;
	tuiSystem.update();
#endif
}

//--------------------------------------------------------------
void testApp::draw(){
#ifdef USE_GUI
	guiSystem.draw();
#endif

#ifdef USE_TUI
	tuiSystem.draw();
#endif

#ifdef USE_VIDEO
	videoSystem.draw();
#endif

}

void testApp::windowResized(int w, int h) {
	printf("TEST windowResized(%i, %i)\n", w, h);
	window.width		= w;
	window.height		= h;
	
	window.invWidth		= 1.0f/window.width;
	window.invHeight	= 1.0f/window.height;
	window.aspectRatio	= window.width * window.invHeight;
	window.aspectRatio2 = window.aspectRatio * window.aspectRatio;
}


#pragma mark Input callbacks

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
    switch(key) {
#ifdef USE_GUI
		case ' ':
			guiSystem.gui.toggleDraw();
			glClear(GL_COLOR_BUFFER_BIT);
			break;
#endif			
		case 'f':
			ofToggleFullscreen();
			break;
		case 's':
			static char fileNameStr[255];
			sprintf(fileNameStr, "output_%0.4i.png", ofGetFrameNum());
			static ofImage imgScreen;
			imgScreen.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			printf("Saving file: %s\n", fileNameStr);
			imgScreen.saveImage(fileNameStr);
			break;
		case '1':
			videoSystem.vidGrabber.videoSettings();
			break;
	}
}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	float mouseNormX = x * window.invWidth;
    float mouseNormY = y * window.invHeight;
    float mouseVelX = (x - pmouseX) * window.invWidth;
    float mouseVelY = (y - pmouseY) * window.invHeight;
}

void testApp::mouseDragged(int x, int y, int button) {
	float mouseNormX = x * window.invWidth;
    float mouseNormY = y * window.invHeight;
    float mouseVelX = (x - pmouseX) * window.invWidth;
    float mouseVelY = (y - pmouseY) * window.invHeight;
}

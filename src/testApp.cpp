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
	
	settings.rot = 0;
	settings.frame_spacing = 1;
	settings.render_offset.set((window.width-VIDEO_WIDTH)/2, (window.height-VIDEO_HEIGHT)/2, -750.0),
	settings.lerpSpeed = 0.095;
	
	numFrames = 0;
	numVerts = 0;
	settings.numVertices = MAX_VERTICES;

	settings.gl_draw_mode = GL_POINTS;
#ifdef USE_TUIO
	tuioClient.start(12345);
	
//	ofAddListener(tuioClient.cursorAdded, this, &testApp::saveTuioCursorPos);
#endif

#ifdef USE_OSC
	osc_in.setup( 1234 );
#endif

#ifdef USE_TUI
	tuiSystem.enableAppEvents();
#endif
	
#ifdef USE_GUI
	guiSystem.enableAppEvents();
#endif	
	
#ifdef USE_SPEECH_TO_TEXT
	speechSystem.enableAppEvents();
#endif

#ifdef USE_DATA
	dataSystem.enableAppEvents();
#endif
	
#ifdef USE_TEMPLATE_MATCHING
	templSystem.enableAppEvents();
#endif
	
#ifdef USE_VIDEO
	VideoPipeline *pipe = new VideoPipeline();
	DifferencingFilter* differ = new DifferencingFilter();
	differ->settings.once = false;

//	pipe->addFilter(new CannyEdgeFilter());
//	pipe->addFilter(differ);
//	pipe->addFilter(new AdaptiveSkinFilter());
//	pipe->addFilter(new SimpleThresholdingFilter());
	pipe->addFilter(new AdaptiveThresholdingFilter());
	pipe->addFilter(&contourFilter);

	videoSystem.addPipeline(pipe);

	videoSystem.enableAppEvents();
#endif

#ifdef USE_VBO
	for(int i=0; i<MAX_VERTICES; i++) {
		pos[i][0] = pos[i][1] = pos[i][2] = 0;
		colors[i][3] = 1;
	}
	// Setup the VBO buffers
	glGenBuffersARB(2, &vbo[0]);
	
	// vbo for vertex positions
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(pos), pos, GL_STREAM_DRAW_ARB);


	// vbo for vertex colors
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(colors), colors, GL_STREAM_DRAW_ARB);

#endif
}


//--------------------------------------------------------------
void testApp::update(){
#ifdef USE_TUIO
	tuioClient.getMessage();
/*
	// do finger stuff
	list<ofxTuioCursor*>cursorList = tuioClient.getTuioCursors();
	
	ofxTuioCursor* first = *(cursorList.begin());
	switch(cursorList.size()) {
		case 2:
			ofxTuioCursor* second = first+1;
			settings.rot.z = first->getAngleDegrees(second);
			settings.render_offset.z = first->getDistance(second);
			break;
		case 1:
			settings.render_offset.x = (int)first->getX()*window.width;
			settings.render_offset.y = (int)first->getY()*window.height;
			printf("Set render_offset = (%d, %d)\n)", settings.render_offset.x, settings.render_offset.y);
		default:
			break;
	}

	for(list<ofxTuioCursor*>::iterator it=cursorList.begin(); it != cursorList.end(); it++) {
		ofxTuioCursor *tcur = (*it);
        float vx = tcur->getXSpeed() * tuioCursorSpeedMult;
        float vy = tcur->getYSpeed() * tuioCursorSpeedMult;
    }
*/
#endif

#ifdef USE_VBO
	numVertsPrev = numVerts;

	msaColor c;
	c.setHSV(ofGetFrameNum() % 360, 1, 1);
	frameColor = c;

	float new_z = numFrames*settings.frame_spacing;
	
	ofxCvContourFinder* contours = &contourFilter.contourFinder;
	for( int i = 0; i < contours->blobs.size(); i++ ) {
		if (numVerts+contours->blobs[i].pts.size() > settings.numVertices) {
			numVerts = 0;
			numFrames = 0;
		}
		for(int j = 0; j < contours->blobs[i].pts.size(); j++){
			contours->blobs[i].pts[j].z = new_z;
			pos[numVerts][0] = contours->blobs[i].pts[j].x;
			pos[numVerts][1] = contours->blobs[i].pts[j].y;
			pos[numVerts][2] = contours->blobs[i].pts[j].z;

			colors[numVerts][0] = c.r;
			colors[numVerts][1] = c.g;
			colors[numVerts][2] = c.b;
			numVerts++;
		}
	}

	if (videoSystem.bGotFrame)
		numFrames++;
	
#endif
	
#ifdef USE_OSC
	while( osc_in.hasWaitingMessages() ) {
		ofxOscMessage m;
		osc_in.getNextMessage( &m );
		
		if ( m.getAddress() == "/offset/xyz" ) {
			if( m.getNumArgs() >= 2 ) {
				settings.render_offset.x += m.getArgAsFloat(0);
				settings.render_offset.y += m.getArgAsFloat(1);
			}
			if( m.getNumArgs() == 3 ) {
				settings.render_offset.z += m.getArgAsFloat(2) * 10.0;
			}

//			printf("Offset: (%f, %f, %f)\n", m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2));
//			printf("Orientation: (%f, %f)\n", m.getArgAsFloat(0), m.getArgAsFloat(1));
		}
		else if ( m.getAddress() == "/acceleration/xyz" ) {
			if( m.getNumArgs() == 3 ) {
				printf("Before: %f, After: %f\n", iPhoneAccel.z, m.getArgAsFloat(2));
				
				iPhoneAccel.x = ofLerp(iPhoneAccel.x, m.getArgAsFloat(0), settings.lerpSpeed);
				iPhoneAccel.y = ofLerp(iPhoneAccel.y, m.getArgAsFloat(1), settings.lerpSpeed);
				
				// don't lerp, rotation has been constrained within [-TWO_PI, TWO_PI]
				if (fabs(iPhoneAccel.z - m.getArgAsFloat(2)) > PI)
					iPhoneAccel.z = m.getArgAsFloat(2);
				else
					iPhoneAccel.z = ofLerp(iPhoneAccel.z, m.getArgAsFloat(2), settings.lerpSpeed);

				iPhoneOrientation = (fabs(iPhoneAccel.y) > fabs(iPhoneAccel.x))? PORTRAIT : LANDSCAPE;
				if (iPhoneOrientation == LANDSCAPE)
					iPhoneHomeButton = (iPhoneAccel.x < 0)? ON_THE_RIGHT : ON_THE_LEFT;
				else
					iPhoneHomeButton = (iPhoneAccel.y < 0)? ON_THE_BOTTOM: ON_THE_TOP;
				
				// swap x, y axes (more intuitive interface)
				settings.rot.set(iPhoneAccel.x*90,iPhoneAccel.z*90,-iPhoneAccel.y*90);
			}
		}
	}
#endif
	// save old mouse position (openFrameworks doesn't do this automatically like processing does)
	pmouseX = mouseX;
	pmouseY = mouseY;
}

//--------------------------------------------------------------
void testApp::draw(){
#ifdef USE_VBO
	glPushMatrix();
	glTranslatef(settings.render_offset.x, settings.render_offset.y, settings.render_offset.z);
	
	glTranslatef(VIDEO_WIDTH/2, VIDEO_HEIGHT/2, FRAMES_IN_BUFFER/2);
	glRotatef(settings.rot.x, 1.0,   0,   0);
	glRotatef(settings.rot.y,   0, 1.0,   0);
	glRotatef(settings.rot.z,   0,   0, 1.0);
	glTranslatef(-VIDEO_WIDTH/2, -VIDEO_HEIGHT/2, -FRAMES_IN_BUFFER/2);
	
	glEnable(GL_DEPTH_TEST);
	glPointSize(2);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	
	
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, numVerts * 3 * sizeof(float), pos);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, numVerts * 4 * sizeof(float), colors);
	glColorPointer(4, GL_FLOAT, 0, 0);	


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

//	frameColor.setGL();
	glDrawArrays(settings.gl_draw_mode, 0, numVerts);
	
	glDisableClientState(GL_VERTEX_ARRAY); 
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	glDisable(GL_DEPTH_TEST);	
	glPopMatrix();
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
	static int modkey;
    switch(key) {
		case ' ':
#ifdef USE_GUI
			guiSystem.toggleDraw();
#endif			
			glClear(GL_COLOR_BUFFER_BIT);
		case 'g':
#ifdef USE_TUI
			tuiSystem.toggleDraw();
#endif

#ifdef USE_VIDEO
			videoSystem.toggleDraw();
#endif			
			break;
		case 'f':
			ofToggleFullscreen();
			break;
		case 'c':
			numFrames = 0;
			numVerts = 0;
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
	modkey = key;
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

void testApp::saveTuioCursorPos(ofxTuioCursor &cursor) {
	printf("Found new cursor\n");
}

testApp::~testApp() {
	printf("Goodbye!\n");
}
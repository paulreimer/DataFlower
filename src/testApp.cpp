#include "testApp.h"

#pragma mark App callbacks

//--------------------------------------------------------------
void testApp::setup() 
{
	// initialize stuff according to current window size
#ifdef START_FULLSCREEN_SECONDARY
	glutReshapeWindow(1152, 864);
	glutPositionWindow(-1152,0);
	glutFullScreen();
#endif

	windowResized(ofGetWidth(), ofGetHeight());

	ofBackground(0, 0, 0);
	ofSetBackgroundAuto(true);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	settings.lerpSpeed = 0.095;

#ifdef USE_GUI
	guiSystem.gui.setup();
#endif
	
#ifdef USE_GPU_VIS
#ifdef USE_OPENCL
	renderSystem.clScheduler = &clScheduler;
#endif	
#ifdef USE_REMOTE_CONTROL
	renderSystem.settings.rot = &remoteSystem.pos.rot;
	renderSystem.settings.offset = &remoteSystem.pos.offset;
#else
	renderSystem.settings.rot = new ofPoint(0,0,0);
	renderSystem.settings.offset = new ofPoint(0,0,0);
#endif
	ofDisableSetupScreen();

	renderSystem.enableKeyEvents();

	renderSystem.enabled = false;
#endif

#ifdef USE_TUIO
	tuioClient.start(12345);
#endif

#ifdef USE_OSC
	osc_in.setup( 1234 );
#endif
	
#ifdef USE_VIDEO	
	ofxSimpleGuiPage* page = &guiSystem.gui.page(1);
	page->setName("Video Inputs");
	videoSystem.gui = page;
	
	videoSystem.grabSizes.resize(2, ofPoint(VIDEO_SIZE));
	
	videoSystem.addGraph(new VideoFilterGraph());
#endif
	
#ifdef USE_TUI
	ofAddListener(tuiSystem.fiducialFoundEvt,	&videoSystem,	&VideoSystem::fiducialFound);
	ofAddListener(tuiSystem.fiducialLostEvt,	&videoSystem,	&VideoSystem::fiducialLost);
	ofAddListener(tuiSystem.fiducialUpdatedEvt,	&videoSystem,	&VideoSystem::fiducialUpdated);

	ofAddListener(tuiSystem.fiducialRayIntersectionFoundEvt,	&videoSystem,	&VideoSystem::fiducialRayIntersectionFound);
	ofAddListener(tuiSystem.fiducialRayIntersectionLostEvt,		&videoSystem,	&VideoSystem::fiducialRayIntersectionLost);
	ofAddListener(tuiSystem.fiducialRayIntersectionUpdatedEvt,	&videoSystem,	&VideoSystem::fiducialRayIntersectionUpdated);
#endif	
	
#ifdef USE_REMOTE_CONTROL
	remoteSystem.enableAppEvents();
#endif

#ifdef USE_VIDEO	
	videoSystem.enableAppEvents();
#endif
	
#ifdef USE_TUI
	tuiSystem.enableAppEvents();
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
	
#ifdef USE_GUI
	guiSystem.enableAppEvents();
#endif
}

//--------------------------------------------------------------
void testApp::update()
{
	// save old mouse position (openFrameworks doesn't do this automatically like processing does)
	pmouseX = mouseX;
	pmouseY = mouseY;
	
	tuiSystem.update();
	videoSystem.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
#ifdef USE_GPU_VIS
	if (renderSystem.enabled)
		renderSystem.draw();

	glViewport(0,0, window.width,window.height);
	//	glRasterPos2f(0.0,0.0);
	ofSetupScreen();
#endif
/*
	fid_objs_table* fid_objs = &tuiSystem.fid_objs;

	if (fid_objs->size() > 0)
	{
		fid_objs_lookup filters_lookup = fid_objs->find(FIDUCIAL_FILTER_TYPE);
		fid_obj_table& filters = filters_lookup->second;

		fid_obj_lookup filter_obj;

		ofxPoint2f corners[BOX_CORNERS];

		VideoFilterPtr filter;
		ofxFiducial *fiducial;

		ofxPoint2f box_origin;
		float box_angle;
		int box_w, box_h;
		
		ofxPoint2f ref;

		for (filter_obj = filters.begin(); filter_obj != filters.end(); filter_obj++) {
			// Don't check initial pipeline stages, ourselves, or any pre-assigned fiducials
			fiducial = filter_obj->first;
			filter = dynamic_cast<VideoFilter*>(filter_obj->second);
			
			if (null_fiducial_id(*fiducial))
				continue;
			
			box_origin.set(filter->x, filter->y);
			box_angle = TWO_PI-filter->angle;

			box_w = filter->width;
			box_h = filter->height;
			
			corners[0].set(box_origin.x,		box_origin.y);
			corners[1].set(box_origin.x+box_w,	box_origin.y);
			corners[2].set(box_origin.x,		box_origin.y+box_h);
			corners[3].set(box_origin.x+box_w,	box_origin.y+box_h);

			ref.set(box_origin.x+(box_w/2),box_origin.y+(box_h/2));
			
			for (int c=0; c<BOX_CORNERS; c++) {
				corners[c].rotateRad(box_angle, ref);
			
				printf("Fiducial %d, corner %d: (%4.2f,%4.2f)\n", fiducial->getId(), c, corners[c].x, corners[c].y);
				ofSetColor(0xfffff);
				ofFill();
				
				ofCircle(corners[c].x, corners[c].y, 8);
			}
		}
	}
*/
}

void testApp::windowResized(int w, int h) 
{
	printf("TEST windowResized(%i, %i)\n", w, h);
	window.width		= w;
	window.height		= h;

	window.invWidth		= 1.0f/window.width;
	window.invHeight	= 1.0f/window.height;
	window.aspectRatio	= window.width * window.invHeight;
	window.aspectRatio2 = window.aspectRatio * window.aspectRatio;
	
	tuiSystem.x_scale=window.width/tuiSystem.videoSize.x;
	tuiSystem.y_scale=window.height/tuiSystem.videoSize.y;
}


#pragma mark Input callbacks

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	static int modkey;
    switch(key)
	{
		case 'g':
#ifdef USE_GUI
			guiSystem.toggleDraw();
#endif			
			glClear(GL_COLOR_BUFFER_BIT);

		case 't':
#ifdef USE_TUI
			tuiSystem.toggleDraw();
#endif
		case 'v':
#ifdef USE_VIDEO
			videoSystem.toggleDraw();
#endif			
			break;
		case 'r':
#ifdef USE_GPU_VIS
			renderSystem.toggleDraw();
#endif
			break;
		case 'f':
			ofToggleFullscreen();
			break;
		case 'c':
			// TODO: capture
			break;
		case 's':
			static char fileNameStr[255];
			sprintf(fileNameStr, "output_%0.4i.png", ofGetFrameNum());
			static ofImage imgScreen;
			imgScreen.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			printf("Saving file: %s\n", fileNameStr);
			imgScreen.saveImage(fileNameStr);
			break;
			break;
	}
	modkey = key;

	if ('0' <= key && key <= '9') {
	int v = key-'0'-1;
/*
	if (modkey == SHIFT_KEY)
	{
		list<ofVideoPlayer>::iterator v_player = videoSystem.vidPlayers.find(v);
		if (v_player != videoSystem.vidPlayers.end())
			v_player->videoSettings();
*/
//	else if (modkey == CONTROL_KEY
//	else if (videoSystem.vidGrabbers.size() > v)
		videoSystem.vidGrabbers[v].videoSettings();
	}
	
}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) 
{
	float mouseNormX = x * window.invWidth;
    float mouseNormY = y * window.invHeight;
    float mouseVelX = (x - pmouseX) * window.invWidth;
    float mouseVelY = (y - pmouseY) * window.invHeight;
}

void testApp::mouseDragged(int x, int y, int button) 
{
	float mouseNormX = x * window.invWidth;
    float mouseNormY = y * window.invHeight;
    float mouseVelX = (x - pmouseX) * window.invWidth;
    float mouseVelY = (y - pmouseY) * window.invHeight;
}

testApp::~testApp()
{
	printf("Goodbye!\n");
}

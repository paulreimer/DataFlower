#include "testApp.h"

#pragma mark App callbacks

//--------------------------------------------------------------
void testApp::setup() 
{
	// initialize stuff according to current window size
/*
	glutReshapeWindow(1152, 864);
	glutPositionWindow(-1152,0);
	glutFullScreen();
*/

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
	renderSystem.enableAppEvents();
	renderSystem.enableKeyEvents();
#endif

#ifdef USE_VIDEO
	videoSystem.gui = &guiSystem.gui.addPage("Video Inputs");
	
	videoSystem.grabSizes.resize(2, ofPoint(VIDEO_SIZE));
	
	videoSystem.enableAppEvents();
#endif
	
#ifdef USE_TUIO
	tuioClient.start(12345);
	
//	ofAddListener(tuioClient.cursorAdded, this, &testApp::saveTuioCursorPos);
#endif

#ifdef USE_OSC
	osc_in.setup( 1234 );
#endif
	
#ifdef USE_REMOTE_CONTROL
	remoteSystem.enableAppEvents();
#endif

#ifdef USE_TUI
//	tuiSystem.toggleDraw();
	tuiSystem.enableAppEvents();
	
	ofAddListener(tuiSystem.fiducialFoundEvt,	&videoSystem,	&VideoSystem::fiducialFound);
	ofAddListener(tuiSystem.fiducialLostEvt,	&videoSystem,	&VideoSystem::fiducialLost);
	ofAddListener(tuiSystem.fiducialUpdatedEvt,	&videoSystem,	&VideoSystem::fiducialUpdated);

	ofAddListener(tuiSystem.fiducialRayIntersectionFoundEvt,	&videoSystem,	&VideoSystem::fiducialRayIntersectionFound);
	ofAddListener(tuiSystem.fiducialRayIntersectionLostEvt,		&videoSystem,	&VideoSystem::fiducialRayIntersectionLost);
	ofAddListener(tuiSystem.fiducialRayIntersectionUpdatedEvt,	&videoSystem,	&VideoSystem::fiducialRayIntersectionUpdated);
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

	/*
#ifdef USE_TUI
	
	fid_objs_table* fid_objs = &tuiSystem.fid_objs;

	if (fid_objs->size() > 0)
	{
	 	map<ofxFiducial*, bool> marked_fiducials;

		fid_objs_lookup pipelines_lookup = fid_objs->find(FIDUCIAL_PIPELINE_TYPE);
		fid_objs_lookup filters_lookup = fid_objs->find(FIDUCIAL_FILTER_TYPE);
		
		if (pipelines_lookup != fid_objs->end() && filters_lookup != fid_objs->end()) {
			fid_obj_table& pipelines = pipelines_lookup->second;
			fid_obj_table& filters = filters_lookup->second;
			
			fid_obj_lookup pipeline_obj, pipeline_1st_stage;
			fid_obj_lookup filter_obj, chk_filter_obj;//, pipelines;
			
			VideoPipelinePtr pipeline;
			VideoFilterPtr filter, chk_filter, hit_filter;

			ofxFiducial *fiducial, *chk_fiducial, *hit_fiducial;
			
			ofxPoint2f first_hit_point, edge_hit_point, hit_point;
			ofxPoint2f origin, endpoint, ref;
			ofxPoint2f box_origin;
			float angle, box_angle;
			float dist, min_dist;
			
			bool bPipelineFirstStage;

			// We create a pipeline for each null fiducial
			for(pipeline_obj = pipelines.begin(); pipeline_obj != pipelines.end(); pipeline_obj++) {

				fiducial = pipeline_obj->first;
#ifdef USE_SMART_POINTERS
				pipeline = pipeline_obj->second->cast<VideoPipeline>();
#else
				pipeline = dynamic_cast<VideoPipeline*>(pipeline_obj->second);
#endif
				pipeline->truncate();
				
				pipeline_1st_stage = filters.find(pipeline->fiducial);
				if (pipeline_1st_stage == filters.end())
					continue;
				
#ifdef USE_SMART_POINTERS
				filter = pipeline_1st_stage->second->cast<VideoFilter>();
#else
				filter = dynamic_cast<VideoFilter*>(pipeline_1st_stage->second);
#endif
				pipeline->addFilter(filter);

				angle = filter->angle;
				origin = filter->output_point();

				ref = filter->origin();

				endpoint.set(origin.x + window.width + window.height, origin.y);
				endpoint.rotateRad(angle, ref);

				// Compute closest intersections for each fiducial vs. other fiducials
				edge_hit_point = intersects_window_edge(origin, endpoint, angle);

				bPipelineFirstStage = true;
				
				filter_obj = filters.begin();
				while (first_hit_point != edge_hit_point && filter_obj != filters.end()) {

					if (bPipelineFirstStage)
						bPipelineFirstStage = false;
					else {
						fiducial = filter_obj->first;
#ifdef USE_SMART_POINTERS
						filter = filter_obj->second->cast<VideoFilter>();
#else
						filter = dynamic_cast<VideoFilter*>(filter_obj->second);
#endif
					}

					// Setup this candidate fiducial as the ray source
					angle = filter->angle;

					origin = filter->output_point();

					ref = filter->origin();

					endpoint.set(origin.x + window.width + window.height, origin.y);
					endpoint.rotateRad(angle, ref);

					first_hit_point = edge_hit_point = intersects_window_edge(origin, endpoint, angle);

					min_dist = origin.distance(first_hit_point);

					hit_fiducial = NULL;
					hit_filter = NULL;

					// Check if this candidate fiducial intersects any others, and choose the one
					// with minimum distance intersection
					for (chk_filter_obj = filters.begin(); chk_filter_obj != filters.end(); chk_filter_obj++) {
						// Don't check initial pipeline stages, ourselves, or any pre-assigned fiducials
						chk_fiducial = chk_filter_obj->first;
#ifdef USE_SMART_POINTERS
						chk_filter = chk_filter_obj->second->cast<VideoFilter>();
#else
						chk_filter = dynamic_cast<VideoFilter*>(chk_filter_obj->second);
#endif

						if (null_fiducial_id(*chk_fiducial) || chk_fiducial == fiducial)
							continue;

						box_origin.set(chk_filter->x, chk_filter->y);
						box_angle = chk_filter->angle;

						hit_point = intersects(origin, first_hit_point, TWO_PI-angle,
											   box_origin, box_angle,
											   chk_filter->width, chk_filter->height);
						
						if (hit_point.x >= 0 && hit_point.y >= 0)
	{
							dist = origin.distance(hit_point);
							if (dist < min_dist)
	{
								min_dist = dist;
								first_hit_point = hit_point;

								hit_fiducial = &(*chk_fiducial);

								hit_filter = &(*chk_filter);
								hit_filter->setHitPoint(hit_point);
								
								filter_obj = chk_filter_obj;
							}
						}
					}
					
					if (hit_fiducial != NULL)
	{
						pipeline->addFilter(hit_filter);
					} else
						filter_obj++;
				}
				
				pipeline->setEdgeHitPoint(edge_hit_point);
			}
		}
	}
#endif
	*/
}

//--------------------------------------------------------------
void testApp::draw() 
{
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

/*

void testApp::fiducialFound(ofxFiducial &fiducial) 
{
//	printf("Found fiducial #%d\n", fiducial.getId());
	fid_obj_table& filters = tuiSystem.fid_objs[FIDUCIAL_FILTER_TYPE];
	fid_obj_lookup filter_obj = filters.find(&fiducial);

	if (filter_obj == filters.end()) {
		VideoFilter* filter = tuiSystem.createFiducialFilter(&fiducial);
		filter->setConfig(&fid_gui_conf);

		// TODO: make this user-adjustable, or set automatically
		filter->videoSize.set(VIDEO_SIZE);
		filter->setup();

		filters.insert(make_pair(&fiducial,filter));
	}

	if (fiducial.getId() == NULL_FIDUCIAL_ID)
	{
		fid_obj_table& pipelines = tuiSystem.fid_objs[FIDUCIAL_PIPELINE_TYPE];
		fid_obj_lookup pipeline_obj = pipelines.find(&fiducial);

		if (pipeline_obj == pipelines.end()) {
//			VideoPipelinePtr pipeline = new VideoPipeline();
			VideoPipeline* pipeline = new VideoPipeline();
			pipeline->fiducial = &fiducial;
			videoSystem.addPipeline(pipeline, &(videoSystem.grabImgs.back()));

			pipelines.insert(make_pair(&fiducial,pipeline));
		}
	}

	fiducial.life += MAX_FIDUCIAL_LIFE;	
}

void testApp::fiducialLost(ofxFiducial &fiducial) 
{
//	printf("Lost fiducial #%d\n", fiducial.getId());
	
	fid_objs_lookup filters_lookup = tuiSystem.fid_objs.find(FIDUCIAL_FILTER_TYPE);
	VideoFilterPtr filter = NULL;
	
	if (filters_lookup != tuiSystem.fid_objs.end()) {
		fid_obj_table& filters = filters_lookup->second;
		fid_obj_lookup filter_obj = filters.find(&fiducial);

		if (filter_obj != filters.end()) {
#ifdef USE_SMART_POINTERS
			filter = filter_obj->second->cast<VideoFilter>();
#else
			filter = dynamic_cast<VideoFilter*>(filter_obj->second);
#endif
			
			filter->fiducial = NULL;
			filter->enabled = false;
			filter->disableAllEvents();

			filters.erase(filter_obj);
		}
	}

	fid_objs_lookup pipelines_lookup = tuiSystem.fid_objs.find(FIDUCIAL_PIPELINE_TYPE);
	VideoPipelinePtr pipeline = NULL;
	
	if (pipelines_lookup != tuiSystem.fid_objs.end()) {
		fid_obj_table& pipelines = pipelines_lookup->second;
		fid_obj_lookup pipeline_obj = pipelines.find(&fiducial);
		if (pipeline_obj != pipelines.end()) {
#ifdef USE_SMART_POINTERS
			pipeline = pipeline_obj->second->cast<VideoPipeline>();
#else
			pipeline = dynamic_cast<VideoPipeline*>(pipeline_obj->second);
#endif

			pipeline->fiducial = NULL;
			pipeline->enabled = false;
			pipeline->disableAllEvents();

			videoSystem.dropPipeline(pipeline);

			pipelines.erase(pipeline_obj);
#ifndef USE_SMART_POINTERS			
			delete pipeline;
#endif
		}
		
#ifdef USE_SMART_POINTERS			
		if (!filter.isNull()) {
#else
		if (filter != NULL)
	{
#endif
			for (pipeline_obj=pipelines.begin(); pipeline_obj!=pipelines.end(); pipeline_obj++) {
#ifdef USE_SMART_POINTERS
				pipeline = pipeline_obj->second->cast<VideoPipeline>();
#else
				pipeline = dynamic_cast<VideoPipeline*>(pipeline_obj->second);
#endif
				
				if (pipeline->dropFilter(filter))
					break;
			}
		}
	}
#ifndef USE_SMART_POINTERS
	if (filter != NULL)
		delete filter;
#endif
}

void testApp::fiducialUpdated(ofxFiducial &fiducial) 
{
	fid_objs_lookup filters_lookup = tuiSystem.fid_objs.find(FIDUCIAL_FILTER_TYPE);
	
	if (filters_lookup != tuiSystem.fid_objs.end()) {
		fid_obj_table& filters = filters_lookup->second;
		fid_obj_lookup filter_obj = filters.find(&fiducial);
		
		if (filter_obj != filters.end()) {
			VideoFilterPtr filter;
#ifdef USE_SMART_POINTERS
			filter = filter_obj->second->cast<VideoFilter>();
#else
			filter = dynamic_cast<VideoFilter*>(filter_obj->second);
#endif
			float x_scale=window.width/tuiSystem.getVideoSize().x;
			float y_scale=window.height/tuiSystem.getVideoSize().y;
			
			ofxPoint2f fidPos(fiducial.current.xpos*x_scale,
							  fiducial.current.ypos*y_scale);
			
			float angle = fiducial.getAngle();
			float aspect_ratio = filter->videoSize.y/filter->videoSize.x;

			ofxPoint2f filterPos(fidPos.x - (filter->width/2),
								 fidPos.y + (filter->width*aspect_ratio) - 20);

			filterPos.rotateRad(angle, fidPos);

			filter->setPos(filterPos.x, filterPos.y, angle);
		}
	}
}
 */

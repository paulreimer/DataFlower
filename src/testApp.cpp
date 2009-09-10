#include "testApp.h"

#pragma mark App callbacks

//--------------------------------------------------------------
void testApp::setup() {
	// initialize stuff according to current window size
	glutReshapeWindow(1152, 864);
	glutPositionWindow(-1152,0);
	glutFullScreen();
	
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
	
	videoSystem.grabSizes.resize(3, ofPoint(VIDEO_SIZE));
	
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
	tuiSystem.toggleDraw();
	tuiSystem.enableAppEvents();
	
	ofAddListener(tuiSystem.fiducialFound, this, &testApp::fiducialFound);
	ofAddListener(tuiSystem.fiducialLost, this, &testApp::fiducialLost);
	ofAddListener(tuiSystem.fiducialUpdated, this, &testApp::fiducialUpdated);
	
#ifdef USE_GUI
	fid_gui_conf.sliderHeight		= 10;
	fid_gui_conf.sliderTextHeight	= 12;

	fid_gui_conf.fontSize			= 8;
	fid_gui_conf.fontOffset.set		(0, fid_gui_conf.fontSize-1);
	
	fid_gui_conf.setup();
#endif

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
/*	
	mx = my = 1.0;
	bx = by = 0;
	guiSystem.gui.addSlider("mx", &mx, 0.8, 1.2, 0.0);
	guiSystem.gui.addSlider("my", &my, 0.8, 1.2, 0.0);
	guiSystem.gui.addSlider("bx", &bx, -40, 40);
	guiSystem.gui.addSlider("by", &by, -40, 40);
*/
#endif
}

// TODO: these don't belong here
bool null_fiducial_id (ofxFiducial& l) {
	return (l.getId()==NULL_FIDUCIAL_ID);
}

bool null_fid_obj_id (pair<ofxFiducial*,FiducialBackedObject*> l) {
	return (l.first->getId()==NULL_FIDUCIAL_ID);
}

bool null_fid_filter (pair<ofxFiducial*,FiducialBackedObject*> l) {
	return null_fid_obj_id(l) && (l.second->type()==FIDUCIAL_FILTER_TYPE);
}

bool null_fid_pipeline (pair<ofxFiducial*,FiducialBackedObject*> l) {
	return null_fid_obj_id(l) && (l.second->type()==FIDUCIAL_PIPELINE_TYPE);
}

struct fiducial_id_asc {
	bool operator() (const ofxFiducial* & l, const ofxFiducial* & r) const {
		// TODO: stupid const casts
		return (const_cast<ofxFiducial* &>(l)->getId() < const_cast<ofxFiducial* &>(r)->getId());
	}
};

//--------------------------------------------------------------
void testApp::update(){
	// save old mouse position (openFrameworks doesn't do this automatically like processing does)
	pmouseX = mouseX;
	pmouseY = mouseY;

#ifdef USE_TUI
	fid_objs_table* fid_objs = &tuiSystem.fid_objs;

	if (fid_objs->size() > 0) {
	 	map<ofxFiducial*, bool> marked_fiducials;

		fid_objs_lookup pipelines_lookup = fid_objs->find(FIDUCIAL_PIPELINE_TYPE);
		fid_objs_lookup filters_lookup = fid_objs->find(FIDUCIAL_FILTER_TYPE);
		
		if (pipelines_lookup != fid_objs->end() && filters_lookup != fid_objs->end()) {
			fid_obj_table& pipelines = pipelines_lookup->second;
			fid_obj_table& filters = filters_lookup->second;
			
			fid_obj_lookup pipeline_obj, pipeline_1st_stage;
			fid_obj_lookup filter_obj, chk_filter_obj;//, pipelines;
			
			VideoPipeline *pipeline;
			VideoFilter *filter, *chk_filter, *hit_filter;

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
				pipeline = dynamic_cast<VideoPipeline*>(pipeline_obj->second);
				pipeline->truncate();
				
				pipeline_1st_stage = filters.find(pipeline->fiducial);
				if (pipeline_1st_stage == filters.end())
					continue;
				
				filter = dynamic_cast<VideoFilter*>((*pipeline_1st_stage).second);
				pipeline->addFilter(filter);

				angle = filter->angle;

				ref = filter->mid_point();
				
				origin = filter->output_point();
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
						filter = dynamic_cast<VideoFilter*>(filter_obj->second);
					}

					// Don't check any fiducial already assigned to a pipeline
					if (marked_fiducials.find(fiducial) != marked_fiducials.end()) {
						filter_obj++;
						continue;
					}

					// Setup this candidate fiducial as the ray source
					angle = filter->angle;

					ref = filter->mid_point();

					origin = filter->output_point();
//					origin.rotateRad(angle, ref);
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
						chk_filter = dynamic_cast<VideoFilter*>(chk_filter_obj->second);
/*
						printf("Fiducial %d (from %d) is: ", chk_fiducial->getId(), fiducial->getId());
						if (null_fiducial_id(*chk_fiducial))
							printf("null\n");
						if (chk_fiducial == fiducial)
							printf("a dupe\n");
						if (marked_fiducials.find(chk_fiducial) != marked_fiducials.end())
							printf("marked\n");
*/						
						if (null_fiducial_id(*chk_fiducial)
							|| chk_fiducial == fiducial
							|| marked_fiducials.find(chk_fiducial) != marked_fiducials.end()) continue;

						box_origin.set(chk_filter->x, chk_filter->y);
						box_angle = chk_filter->angle;

						hit_point = intersects(origin, first_hit_point, TWO_PI-angle,
											   box_origin, box_angle,
											   chk_filter->width, chk_filter->height);
						
						if (hit_point.x >= 0 && hit_point.y >= 0) {
							dist = origin.distance(hit_point);
							if (dist < min_dist) {
								min_dist = dist;
								first_hit_point = hit_point;

								hit_fiducial = &(*chk_fiducial);

								hit_filter = &(*chk_filter);
								hit_filter->setHitPoint(hit_point);
								
								filter_obj = chk_filter_obj;
							}
						}
					}
					
					if (hit_fiducial != NULL) {
						pipeline->addFilter(hit_filter);

						marked_fiducials[fiducial] = true;
					} else
						filter_obj++;
				}
				
				pipeline->setHitPoint(edge_hit_point);
			}
		}
	}
#endif
}

//--------------------------------------------------------------
void testApp::draw() {
/*
	fid_objs_table* fid_objs = &tuiSystem.fid_objs;

	if (fid_objs->size() > 0) {
		fid_objs_lookup filters_lookup = fid_objs->find(FIDUCIAL_FILTER_TYPE);
		fid_obj_table& filters = filters_lookup->second;

		fid_obj_lookup filter_obj;

		ofxPoint2f corners[BOX_CORNERS];

		VideoFilter *filter;
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
void testApp::keyPressed(int key){
	static int modkey;
    switch(key) {
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
	if (modkey == SHIFT_KEY) {
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
void testApp::mouseMoved(int x, int y ) {
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

testApp::~testApp() {
	printf("Goodbye!\n");
}

ofxPoint2f intersects_window_edge(ofxPoint2f origin, ofxPoint2f endpoint, double angle) {
	return intersects(origin, endpoint, angle,
					  ofxPoint2f(0,0), 0.0, ofGetWidth(), ofGetHeight());
}

//http://newsgroups.archived.at/comp/graphics.algorithms/200603/12/0603122551.html
ofxPoint2f intersects(ofxPoint2f origin, ofxPoint2f endpoint, double angle,
					  ofxPoint2f box_origin, double box_angle, int box_w, int box_h) {
	
	ofxPoint2f corners[BOX_CORNERS];
	corners[0].set(box_origin.x,		box_origin.y);
	corners[1].set(box_origin.x+box_w,	box_origin.y);
	corners[2].set(box_origin.x+box_w,	box_origin.y+box_h);
	corners[3].set(box_origin.x,		box_origin.y+box_h);

	ofxPoint2f ref(box_origin.x + box_w/2, box_origin.y + box_h/2);
	ofxPoint2f hit_point = endpoint;

	int c, next_c;
	for (int c=0; c<BOX_CORNERS; c++)
		corners[c].rotateRad(box_angle, ref);
	
	double min_dist = origin.distance(endpoint);
	double d, la, lb, dist;
	for (int c=0; c<BOX_CORNERS; c++) {
		next_c=(c+1)%BOX_CORNERS;
		ofxPoint2f dp(corners[c].x - origin.x, corners[c].y - origin.y);
		ofxPoint2f qa(endpoint.x - origin.x, endpoint.y - origin.y);
		ofxPoint2f qb(corners[next_c].x - corners[c].x, corners[next_c].y - corners[c].y);
		
		d  = qa.y * qb.x - qb.y * qa.x;
		la = (qb.x * dp.y - qb.y * dp.x)/d;
		lb = (qa.x * dp.y - qa.y * dp.x)/d;
		
		if (0 <= la && la <= 1 && 0 <= lb && lb <= 1) {
			hit_point.set(origin.x + la * qa.x,
						  origin.y + la * qa.y);
			dist = origin.distance(hit_point);
			
			if (d-(5*DBL_EPSILON) <= dist && dist <= d+(5*DBL_EPSILON))
				printf("Trust d value");

			if (dist < min_dist) {
				min_dist = dist;
				endpoint = hit_point;
			}
		}
	}
	return endpoint;
}

ofxPoint2f intersects_window_edge(ofxPoint2f origin, double angle) {
	int w=ofGetWidth();
	int h=ofGetHeight();
	ofxPoint2f endpoint;
	double slope=tan(angle), wall_y;
	if (slope >=0) {
		if (angle < PI) {	// Quadrant 1 [TR]
			wall_y = slope*(w - origin.x) + origin.y;
			if (wall_y < h)	// Right!
				endpoint.set(w, wall_y);
			else			// Top!
				endpoint.set(origin.x+(h-origin.y)/slope, h);
		}
		else {				// Quadrant 3 [BL]
			wall_y = -slope*(w-origin.x) + origin.y;
			if (wall_y > 0)	// Left!
				endpoint.set(0, wall_y);
			else			// Bottom!
				endpoint.set(origin.x + (-origin.y/slope), 0);
		}
	} else {
		if (angle < PI)	{	// Quadrant 2 [TL]
			wall_y = -slope*(w - origin.x) + origin.y;
			if (wall_y < h)	// Left!
				endpoint.set(0, wall_y);
			else			// Top!
				endpoint.set(origin.x + origin.y/slope, h);
		}
		else {				// Quadrant 4 [BR]
			wall_y = slope*(w - origin.x) + origin.y;
			if (wall_y > 0)	// Right!
				endpoint.set(w, wall_y);
			else			// Bottom!
				endpoint.set(origin.x - origin.y/slope, 0);
		}
	}
	return endpoint;
}

void testApp::fiducialFound(ofxFiducial &fiducial) {
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

	if (fiducial.getId() == NULL_FIDUCIAL_ID) {
		fid_obj_table& pipelines = tuiSystem.fid_objs[FIDUCIAL_PIPELINE_TYPE];
		fid_obj_lookup pipeline_obj = pipelines.find(&fiducial);

		if (pipeline_obj == pipelines.end()) {
			VideoPipeline* pipeline = new VideoPipeline();
			pipeline->fiducial = &fiducial;
			videoSystem.addPipeline(pipeline, &(videoSystem.grabImgs.back()));
			pipelines.insert(make_pair(&fiducial,pipeline));
		}
	}

	fiducial.life += MAX_FIDUCIAL_LIFE;	
}

void testApp::fiducialLost(ofxFiducial &fiducial) {
//	printf("Lost fiducial #%d\n", fiducial.getId());
	
	fid_objs_lookup filters_lookup = tuiSystem.fid_objs.find(FIDUCIAL_FILTER_TYPE);
	if (filters_lookup != tuiSystem.fid_objs.end()) {
		fid_obj_table& filters = filters_lookup->second;
		fid_obj_lookup filter_obj = filters.find(&fiducial);
		VideoFilter *filter;
		if (filter_obj != filters.end()) {
			filter = dynamic_cast<VideoFilter*>(filter_obj->second);

			filter->fiducial = NULL;
			filter->enabled = false;
			delete filter;
			
			filters.erase(filter_obj);
		}
	}

	fid_objs_lookup pipelines_lookup = tuiSystem.fid_objs.find(FIDUCIAL_PIPELINE_TYPE);
	if (pipelines_lookup != tuiSystem.fid_objs.end()) {
		fid_obj_table& pipelines = pipelines_lookup->second;
		fid_obj_lookup pipeline_obj = pipelines.find(&fiducial);
		VideoPipeline *pipeline;
		if (pipeline_obj != pipelines.end()) {
			pipeline = dynamic_cast<VideoPipeline*>(pipeline_obj->second);

			pipeline->fiducial = NULL;
			pipeline->enabled = false;
			videoSystem.dropPipeline(pipeline);
			delete pipeline;
			
			pipelines.erase(pipeline_obj);
		}
	}
}

void testApp::fiducialUpdated(ofxFiducial &fiducial) {
//	printf("fiducial %d < %4.2f\n", fiducial.getId(), filter->angle);
	fid_objs_lookup filters_lookup = tuiSystem.fid_objs.find(FIDUCIAL_FILTER_TYPE);
	
	if (filters_lookup != tuiSystem.fid_objs.end()) {
		fid_obj_table& filters = filters_lookup->second;
		fid_obj_lookup filter_obj = filters.find(&fiducial);
		ofxPoint2f pos, ref;
		
		if (filter_obj != filters.end()) {
			VideoFilter* filter = dynamic_cast<VideoFilter*>(filter_obj->second);

			float x_scale=window.width/tuiSystem.getVideoSize().x;
			float y_scale=window.height/tuiSystem.getVideoSize().y;
			
			float angle = fiducial.getAngle();
			float aspect_ratio = filter->videoSize.y/filter->videoSize.x;

			ref.set(fiducial.current.xpos*x_scale,
					fiducial.current.ypos*y_scale);

			pos = ref;
			pos.y += filter->width*aspect_ratio;
			
			pos.rotateRad(angle, ref);

			filter->setMidPoint(pos.x, pos.y, angle);

/*			
			pos.set(ref.x - filter->width/2,
					ref.y + filter->width*aspect_ratio);
			pos.rotateRad(-angle, ref);

			filter->setPos(pos.x, pos.y, angle);
			filter->setPos((fiducial.current.xpos - 1.4*fiducial.r_size)*x_scale,
						   (fiducial.current.ypos - 0.6*fiducial.r_size)*y_scale,
						   angle);

			filter->setPos(fiducial.current.xpos*x_scale,
						   fiducial.current.ypos*y_scale,
						   fiducial.getAngle());
*/
		}
	}
}

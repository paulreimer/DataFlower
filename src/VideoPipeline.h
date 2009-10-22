#pragma once

#include "ofMain.h"
#include "filters.h"

#include "FiducialBackedObject.h"
#include "ofxSimpleGuiConfig.h"
#include "ofxOpenCv.h"
#include "ofxPoint2f.h"

class VideoPipeline : public FiducialBackedObject 
{
public:
	VideoPipeline();
	virtual ~VideoPipeline();

	void setup();

	void update();
	void draw();// const;

	void destroy();

	void setDraw(bool b);
	void toggleDraw();
	bool					doDraw;
	ofPoint					videoSize;

	VideoFilter*			filter(int i);				// 1 based index of page
	VideoFilter*			filter(const std::string& name);

	VideoFilter*			addFilter(VideoFilter* const filter);
	bool					dropFilter(const VideoFilter* const filter);

	ofxCvColorImage			input;

	ofxCvColorImage			output;

	void setConfig(ofxSimpleGuiConfig* const config);
	void setEdgeHitPoint(const ofxPoint2f& edgeHitPoint);
	void setVideoSize(const ofPoint& sze);

	int size() const { return filters.size(); }
	void truncate(int newlen=0);
	
	ofxPoint2f edgeHitPoint;

//	list<lineSegment> rays;

protected:
	list <VideoFilterPtr>	filters;

	ofxSimpleGuiConfig*		config;
};

#ifdef USE_SMART_POINTERS
	typedef Poco::SharedPtr<VideoPipeline> VideoPipelinePtr;
#else
	typedef VideoPipeline* VideoPipelinePtr;
#endif

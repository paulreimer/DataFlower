#pragma once

#include "ofMain.h"
#include "filters.h"

#include "GuiSet.h"
#include "ofxSimpleGuiConfig.h"

#include "ofxOpenCv.h"

class VideoPipeline : public ofxFiducialBacked::gui::GuiSet
{
public:
	VideoPipeline();
	virtual ~VideoPipeline();

	void setup();
	void update();
	void draw();

	ofPoint					videoSize;

	VideoFilter*			addFilter(VideoFilter* /*const*/ filter);
	void					dropFilter(VideoFilter* filter);

	ofxCvColorImage			input;

	ofxCvColorImage			output;

	void setConfig(ofxSimpleGuiConfig* const config);
	void setVideoSize(const ofPoint& sze);

	void truncate(int newlen=0);
	
protected:
	ofxSimpleGuiConfig*		config;
};

#ifdef USE_SMART_POINTERS
	typedef Poco::SharedPtr<VideoPipeline> VideoPipelinePtr;
#else
	typedef VideoPipeline* VideoPipelinePtr;
#endif

#pragma once

#include "ofMain.h"
#include "settings.h"

#include "GuiGraph.h"
#include "ofxOpenCv.h"
#include "ofxFidMain.h"

typedef unsigned char filter_type_t;
enum filter_types {
	NULL_FILTER,
	COLOR_FILTER,
	GRAYSCALE_FILTER,
	MULTI_FILTER,
	SOURCE_FILTER,
	MIRROR_FILTER,
	PRISM_FILTER,
};

#ifdef USE_SMART_POINTERS
typedef Poco::SharedPtr<ofxCvColorImage>		ColorImagePtr;
typedef Poco::SharedPtr<ofxCvGrayscaleImage>	GrayscaleImagePtr;
#else
typedef ofxCvGrayscaleImage*					GrayscaleImagePtr;
typedef ofxCvColorImage*						ColorImagePtr;
#endif

//template <class ofxCvImageType>
class VideoFilter : public ofxFiducialBacked::gui::GuiPage 
{
public:
	VideoFilter();

	virtual ~VideoFilter();

	byte type;
//	vector<ofxCvImageType*>	inputs;
//	vector<ofxCvImageType*>	outputs;
	
	virtual ofxCvImage& input_ref()	=0;
	virtual ofxCvImage& output_ref()=0;
/*
	virtual ofxCvImage* input_ref()
	{
		return inputs.empty()?	NULL : inputs[0];
	}
	virtual ofxCvImage* output_ref()
	{
		return outputs.empty()?	NULL : outputs[0];
	}
*/	
	virtual void setup()	{ GuiPage::setup();	}
	virtual void update()	{ GuiPage::update();}
	virtual void draw()		{ GuiPage::draw();	}
	
	virtual void destroy();

	virtual bool isAllocated() =0;
	
	ofPoint					videoSize;
	
protected:
	void onPress(int mx, int my, int button);
	void onDragOver(int mx, int my, int button);
	
	int saveX, saveY;
};


class GrayscaleFilter : public VideoFilter/*<ofxCvGrayscaleImage>*/ 
{
public:
	GrayscaleFilter() { type = GRAYSCALE_FILTER; }

	ofxCvGrayscaleImage	input;
	ofxCvGrayscaleImage	output;

	ofxCvGrayscaleImage& input_ref() { return input; }
	ofxCvGrayscaleImage& output_ref() { return output; }

	void setup() {
		input.allocate(videoSize.x, videoSize.y);
		output.allocate(videoSize.x, videoSize.y);
	}

	void destroy() {
		input.clear();
		output.clear();
		controls.clear();
	}
	
	bool isAllocated() {
		return (input.bAllocated && output.bAllocated);
	}
};

class ColorFilter : public VideoFilter/*<ofxCvColorImage>*/ 
{
public:
	ColorFilter() { type = COLOR_FILTER; }

	ofxCvColorImage		input;
	ofxCvColorImage		output;

	ofxCvColorImage& input_ref() { return input; }
	ofxCvColorImage& output_ref() { return output; }

	void setup() {
		input.allocate(videoSize.x, videoSize.y);
		output.allocate(videoSize.x, videoSize.y);
	}

	void destroy() {
		input.clear();
		output.clear();
		controls.clear();
	}

	bool isAllocated() {
		return (input.bAllocated && output.bAllocated);
	}
};

//template VideoFilter<ofxCvImageType>	GrayscaleFilter<ofxCvGrayscaleImage>;
//template VideoFilter<ofxCvImageType>	ColorFilter<ofxCvGrayscaleImage>;

//typedef VideoFilter<ofxCvGrayscaleImage>	GrayscaleFilter;
//typedef VideoFilter<ofxCvColorImage>	ColorFilter;

#ifdef USE_SMART_POINTERS
typedef Poco::SharedPtr<VideoFilter> VideoFilterPtr;
#else
typedef VideoFilter* VideoFilterPtr;
#endif

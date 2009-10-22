#pragma once

#include "ofMain.h"
#include "settings.h"


#include "ofxSimpleGuiIncludes.h"
#include "FiducialBackedObject.h"
#include "ofxOpenCv.h"
#include "ofxFidMain.h"
#include "ofxPoint2f.h"

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
class VideoFilter : public FiducialBackedObject 
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
	virtual void setup()	{};
	virtual void update()	{};
	void draw();// const;

	void destroy();

	ofPoint					videoSize;

	ofxSimpleGuiControl		&addControl(ofxSimpleGuiControl& control);
	ofxSimpleGuiButton		&addButton	(std::string name, bool &value);
	ofxSimpleGuiContent		&addContent	(std::string name, ofBaseDraws &content, float fixwidth = -1);
	ofxSimpleGuiSliderInt	&addSlider	(std::string name, int &value, int min, int max, float smoothing=0);
	ofxSimpleGuiSliderByte	&addSlider	(std::string name, byte &value, byte min=0, byte max=255, float smoothing=0);
	ofxSimpleGuiSliderFloat	&addSlider	(std::string name, float &value, float min, float max, float smoothing = 0);
	ofxSimpleGuiSliderDouble &addSlider	(std::string name, double &value, double min, double max, double smoothing = 0);
	ofxSimpleGuiSlider2d	&addSlider2d(std::string name, ofPoint& value, float xmin, float xmax, float ymin, float ymax);
	ofxSimpleGuiTitle		&addTitle	(std::string name);
	ofxSimpleGuiToggle		&addToggle	(std::string name, bool &value);
	ofxSimpleGuiFPSCounter	&addFPSCounter();

	void onPress(int mx, int my, int button);
	void onDragOver(int mx, int my, int button);

	void _mouseMoved	(ofMouseEventArgs &e);
	void _mousePressed	(ofMouseEventArgs &e);
	void _mouseDragged	(ofMouseEventArgs &e);
	void _mouseReleased	(ofMouseEventArgs &e);

	void _keyPressed	(ofKeyEventArgs &e);
	void _keyReleased	(ofKeyEventArgs &e);

	void setConfig		(ofxSimpleGuiConfig* config);
	
	virtual bool isAllocated() =0;

	ofxSimpleGuiConfig	*config;
	
	void setPos(float _x, float _y, float _angle=0.0);
	
protected:
	vector <ofxSimpleGuiControl*>	controls;

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

#pragma once

#include "ofMain.h"
#include "settings.h"


#include "ofxSimpleGuiIncludes.h"
#include "FiducialBackedObject.h"
#include "ofxOpenCv.h"
#include "ofxFidMain.h"
#include "ofxPoint2f.h"

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
	virtual void setup() =0;
	virtual void update()=0;
	void draw();

	void destroy();

	ofPoint					videoSize;

	ofxSimpleGuiControl		&addControl(ofxSimpleGuiControl& control);
	ofxSimpleGuiButton		&addButton(string name, bool &value);
	ofxSimpleGuiContent		&addContent(string name, ofBaseDraws &content, float fixwidth = -1);
	ofxSimpleGuiFPSCounter	&addFPSCounter();
	ofxSimpleGuiSliderInt	&addSlider(string name, int &value, int min, int max, float smoothing=0);
	ofxSimpleGuiSliderByte	&addSlider(string name, byte &value, byte min=0, byte max=255, float smoothing=0);
	ofxSimpleGuiSliderFloat	&addSlider(string name, float &value, float min, float max, float smoothing = 0);
	ofxSimpleGuiSliderDouble &addSlider(string name, double &value, double min, double max, double smoothing = 0);
	ofxSimpleGuiSlider2d	&addSlider2d(string name, ofPoint& value, float xmin, float xmax, float ymin, float ymax);
	ofxSimpleGuiTitle		&addTitle(string name);
	ofxSimpleGuiToggle		&addToggle(string name, bool &value);

	void onPress(int mx, int my, int button);
	void onDragOver(int mx, int my, int button);

	void _mouseMoved(ofMouseEventArgs &e);
	void _mousePressed(ofMouseEventArgs &e);
	void _mouseDragged(ofMouseEventArgs &e);
	void _mouseReleased(ofMouseEventArgs &e);

	void _keyPressed(ofKeyEventArgs &e);
	void _keyReleased(ofKeyEventArgs &e);

	void setConfig(ofxSimpleGuiConfig *config);
	
	virtual bool isAllocated() =0;

	ofxSimpleGuiConfig	*config;
	
//	void setWasHitPoint(ofxPoint2f hitPoint);
//	bool wasHit();
//	ofxPoint2f hitPoint;

//	ofxPoint2f midPoint, outputPoint, inputPoint, originPoint;

	void setPos(float _x, float _y, float _angle=0.0);
	
protected:
	vector <ofxSimpleGuiControl*>	controls;

	int saveX, saveY;
	
};


class GrayscaleFilter : public VideoFilter/*<ofxCvGrayscaleImage>*/ 
{
public:
	ofxCvGrayscaleImage	input;
	ofxCvGrayscaleImage	output;

	ofxCvGrayscaleImage& input_ref() { return input; }
	ofxCvGrayscaleImage& output_ref() { return output; }
/*
	GrayscaleImagePtr input_ref()
	{
#ifdef USE_SMART_POINTERS
		return GrayscaleImagePtr::cast<ofxCvGrayscaleImage>(VideoFilter::input_ref());
#else
		return dynamic_cast<GrayscaleImagePtr>(VideoFilter::input_ref());
#endif
	}

	GrayscaleImagePtr output_ref()
	{
#ifdef USE_SMART_POINTERS
		return GrayscaleImagePtr::cast<ofxCvGrayscaleImage>(VideoFilter::output_ref());
#else
		return dynamic_cast<GrayscaleImagePtr>(VideoFilter::output_ref());
#endif
	}
*/
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
	ofxCvColorImage		input;
	ofxCvColorImage		output;
	ofxCvColorImage& input_ref() { return input; }
	ofxCvColorImage& output_ref() { return output; }
/*	
	ColorImagePtr input_ref()
	{
#ifdef USE_SMART_POINTERS
		return ColorImagePtr::cast<ofxCvGrayscaleImage>(VideoFilter::input_ref());
#else
		return dynamic_cast<ColorImagePtr>(VideoFilter::input_ref());
#endif
	}
	
	ColorImagePtr output_ref()
	{
#ifdef USE_SMART_POINTERS
		return ColorImagePtr::cast<ofxCvGrayscaleImage>(VideoFilter::output_ref());
#else
		return dynamic_cast<ColorImagePtr>(VideoFilter::output_ref());
#endif
	}
*/	
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

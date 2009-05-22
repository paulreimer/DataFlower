#pragma once

#include "VideoFilter.h"

class DifferencingFilter : public VideoFilter  {
public:
	DifferencingFilter(string name = "Frame Differencing");
	virtual ~DifferencingFilter();

	void setup();
	void update();

	void destroy();

	struct DifferencingSettings {
		bool capture;
		bool once;
	} settings;
	
protected:
	ofxCvGrayscaleImage		grayInput;
	ofxCvGrayscaleImage 	grayOutput;

	ofxCvColorImage			inputCapture;
	ofxCvGrayscaleImage 	grayCapture;
};

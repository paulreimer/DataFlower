#pragma once

#include "VideoFilter.h"
#include "ofxCvRgbaImage.h"

#include "../kernels/sobelKernel.cpp"
// Unfortunately, we schedule our own invocation
#include "ofxClScheduler.h"

class SobelFilterOcl : public ColorFilter  {
public:
	friend class testApp;

	SobelFilterOcl(ofxClScheduler& _clScheduler);
	SobelFilterOcl();
	virtual ~SobelFilterOcl();
	
	void setup();
	void update();
	
	void destroy();
	
	class SobelSettings {
		friend class SobelFilterOcl;
	protected:
		float threshold;
	} settings;
	
protected:
	ofxCvRgbaImage		rgbaInput;
	ofxCvGrayscaleImage	grayOutput;
	SobelKernel			oclSobelFilter;

	ofxClScheduler* clScheduler;
};

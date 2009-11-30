#pragma once

#include "VideoFilter.h"
#include "ofxCvRgbaImage.h"

#ifdef USE_OPENCL
#include "../kernels/sobelKernel.h"
// Unfortunately, we schedule our own invocation
#include "ofxClScheduler.h"
#endif

class SobelFilterOcl : public ColorFilter
{
public:
	friend class testApp;

#ifdef USE_OPENCL
	SobelFilterOcl(ofxClScheduler& _clScheduler);
	SobelFilterOcl();
	virtual ~SobelFilterOcl();

	void setup();
	void update();

	void destroy();
#endif

	class SobelSettings {
		friend class SobelFilterOcl;
	protected:
		float threshold;
	} settings;

protected:
	ofxCvRgbaImage		rgbaInput;
	ofxCvGrayscaleImage	grayOutput;
#ifdef USE_OPENCL
	SobelKernel			oclSobelFilter;

	ofxClScheduler* clScheduler;
#endif
};

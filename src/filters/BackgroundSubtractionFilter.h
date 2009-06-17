#pragma once

#include "VideoFilter.h"

class BackgroundSubtractionFilter : public VideoFilter  {
public:
	BackgroundSubtractionFilter();
	virtual ~BackgroundSubtractionFilter();

	void setup();
	void update();

	void destroy();

	struct BackgroundSubtractionSettings {
		bool capture;
//		int		blocksize;
//		int		offset;
//		bool	use_gauss;

		bool	invert;
		int	 threshold;
	} settings;

protected:
	ofxCvColorImage			inputCapture;
	ofxCvGrayscaleImage 	grayCapture;

	ofxCvGrayscaleImage 	fgMask;
};

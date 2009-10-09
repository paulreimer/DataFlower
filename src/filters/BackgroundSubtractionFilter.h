#pragma once

#include "VideoFilter.h"

class BackgroundSubtractionFilter : public ColorFilter  
{
public:
	BackgroundSubtractionFilter();
	virtual ~BackgroundSubtractionFilter();

	void setup();
	void update();

	void destroy();

	class BackgroundSubtractionSettings {
		friend class BackgroundSubtractionFilter;
	protected:
		bool capture;
//		int		blocksize;
//		int		offset;
//		bool	use_gauss;

		bool	invert;
		int	 threshold;
		
		BackgroundSubtractionSettings() {
			capture		= true;
			
			//blocksize	= 9;
			//offset	= 0;
			//use_gauss	= false;
			invert		= false;
			
			threshold	= 120;
		}
	} settings;

protected:
	ofxCvColorImage			inputCapture;
	ofxCvGrayscaleImage 	grayCapture;

	ofxCvGrayscaleImage 	fgMask;
};

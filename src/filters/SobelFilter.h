#pragma once

#include "VideoFilter.h"

class SobelFilter : public ColorFilter  
{
public:
	SobelFilter();
	virtual ~SobelFilter();

	void setup();
	void update();

	void destroy();

	class SobelSettings {
		friend class SobelFilter;
	protected:
		int x_order;
		int y_order;
		int aperture_size;

		SobelSettings() {
			x_order			= 0;
			y_order			= 0;
			aperture_size	= 3;			
		}
	} settings;
	
protected:
	ofxCvGrayscaleImage 	grayInput;

	ofxCvGrayscaleImage 	outputX;
	ofxCvGrayscaleImage 	outputY;

	IplImage*				tmpOutputX;
	IplImage*				tmpOutputY;
	ofxCvGrayscaleImage 	blankImg;
};

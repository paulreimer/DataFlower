#pragma once

#include "VideoFilter.h"

class DifferencingFilter : public GrayscaleFilter  
{
public:
	DifferencingFilter();
	virtual ~DifferencingFilter();

	void setup();
	void update();

	void destroy();

	class DifferencingSettings {
		friend class DifferencingFilter;
	protected:
		bool capture;
		bool once;

		DifferencingSettings() {
			capture	= true;
			once	= true;
		}
	} settings;

protected:
	ofxCvGrayscaleImage 	capture;
};

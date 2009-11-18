#pragma once

#include "VideoFilter.h"

class MotionHistoryImageFilter : public ColorFilter  
{
public:
	MotionHistoryImageFilter();
	virtual ~MotionHistoryImageFilter();

	void setup();
	void update();

	void destroy();

	class MotionHistoryImageSettings {
		friend class MotionHistoryImageFilter;
	protected:
		double	duration;
		
		MotionHistoryImageSettings() {
			duration	= 5.0;
		}
	} settings;

protected:
	ofxCvGrayscaleImage		fgMask;
};

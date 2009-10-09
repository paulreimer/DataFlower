#pragma once

#include "VideoFilter.h"

class CamShiftTrackingFilter : public GrayscaleFilter  
{
public:
	CamShiftTrackingFilter();
	virtual ~CamShiftTrackingFilter();

	void setup();
	void update();

	void destroy();

	class CamShiftTrackingSettings {
		friend class CamShiftTrackingFilter;
	protected:
		int		threshold;
		bool	invert;

		CamShiftTrackingSettings() {
			threshold	= 120;
			invert		= false;
		}
	} settings;
};

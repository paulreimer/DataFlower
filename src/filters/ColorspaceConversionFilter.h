#pragma once

#include "VideoFilter.h"

class ColorspaceConversionFilter : public ColorFilter  
{
public:
	ColorspaceConversionFilter();
	virtual ~ColorspaceConversionFilter();

	void setup();
	void update();

	void destroy();

	class ColorspaceConversionSettings {
		friend class ColorspaceConversionFilter;
	protected:
		int code;
		
		ColorspaceConversionSettings() {
			code = CV_RGB2HSV;
		}
	} settings;
};

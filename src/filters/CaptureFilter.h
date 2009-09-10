#pragma once

#include "VideoFilter.h"

class CaptureFilter : public ColorFilter  {
public:
	CaptureFilter();
	virtual ~CaptureFilter();

	void setup();
	void update();

	void destroy();

	class CaptureSettings {
		friend class CaptureFilter;
	protected:
		bool capture;
		
		CaptureSettings() {
			capture = false;
		}
	} settings;

	vector <ofxCvColorImage*>	captures;
};

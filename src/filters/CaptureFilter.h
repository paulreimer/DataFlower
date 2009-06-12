#pragma once

#include "VideoFilter.h"

class CaptureFilter : public VideoFilter  {
public:
	CaptureFilter();
	virtual ~CaptureFilter();

	void setup();
	void update();

	void destroy();

	struct CaptureSettings {
		bool capture;
	} settings;

	vector <ofxCvColorImage*>	captures;
};

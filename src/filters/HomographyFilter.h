#pragma once

#include "VideoFilter.h"

class HomographyFilter : public VideoFilter  {
public:
	HomographyFilter();
	virtual ~HomographyFilter();

	void setup();
	void update();

	void destroy();

	struct HomographySettings {
		bool mirror_h;
		bool mirror_v;
	} settings;
	
protected:
};

#pragma once

#include "VideoFilter.h"

class HomographyFilter : public ColorFilter  {
public:
	HomographyFilter();
	virtual ~HomographyFilter();

	void setup();
	void update();

	void destroy();

protected:
	class HomographySettings {
		friend class HomographyFilter;
	protected:
		bool mirror_h;
		bool mirror_v;
		
		HomographySettings() {
			mirror_h = false;
			mirror_v = false;
		}
	} settings;
};

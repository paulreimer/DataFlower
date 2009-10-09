#pragma once

#include "VideoFilter.h"
#include "ofxCvDtouchMarkers.h"

class MarkerFindingFilter : public GrayscaleFilter  
{
public:
	MarkerFindingFilter();
	virtual ~MarkerFindingFilter();

	void setup();
	void update();
	void draw();

	void destroy();

	class MarkerFindingSettings {
		friend class MarkerFindingFilter;
	protected:
		MarkerFindingSettings() {
		}
	} settings;
	
	ofxCvDtouchMarkerFinder markerFinder;
protected:

	ofxCvGrayscaleImage diagnostics;
};

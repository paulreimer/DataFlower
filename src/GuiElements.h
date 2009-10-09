#pragma once

#include "ofMain.h"
#include "ofxPoint2f.h"

namespace GuiElements 
{
	namespace types {
		typedef unsigned char actor_type_t;
		enum actor_types {
			ACTOR_TYPE_NULL,
			ACTOR_TYPE_RGB,
			ACTOR_TYPE_GRAY,
		};

		typedef unsigned char arrow_type_t;
		enum arrow_types {
			ARROW_TYPE_NONE,
			ARROW_TYPE_START,
			ARROW_TYPE_END,
			ARROW_TYPE_BOTH,
		};
		
		struct lineSegment {
			ofxPoint2f from;
			ofxPoint2f to;
			actor_type_t from_type;
			actor_type_t to_type;
		};
	}
	
	namespace style {
		static bool bSetup = false;

		static ofTexture grayLineTex;
		static ofTexture rgbLineTex;
		static int lineHeight;
		static int arrowheadRadius;
		
		void setup();
	}	

	namespace renderer {
		void drawLine(types::lineSegment segment);
		void drawLine(ofxPoint2f& startpoint, ofxPoint2f& endpoint,
					  types::actor_type_t starttype,
					  types::actor_type_t fromtype=types::ACTOR_TYPE_NULL);
		void drawArrow(ofxPoint2f& startpoint, ofxPoint2f& endpoint,
					   types::actor_type_t starttype,
					   types::actor_type_t fromtype=types::ACTOR_TYPE_NULL,
					   types::arrow_type_t arrowtype=types::ARROW_TYPE_BOTH);
		
		void grayLine(ofxPoint2f& startpoint, ofxPoint2f& endpoint);
		void rgbLine(ofxPoint2f& startpoint, ofxPoint2f& endpoint);
		void drawTexLine(ofxPoint2f& startpoint, ofxPoint2f& endpoint, ofTexture& tex);
	}
}

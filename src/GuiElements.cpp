#pragma once

#include "GuiElements.h"

namespace GuiElements {
	namespace style {
		void setup() {
			lineHeight = 9;
			arrowheadRadius = 5;

			grayLineTex.allocate(1,3, GL_LUMINANCE_ALPHA);
			unsigned char gray_line_data[3*2] = {
				255,0,	255,255,	255,0
				//	DARK	BRIGHT		DARK
			};
			grayLineTex.loadData(gray_line_data, 1,3, GL_LUMINANCE_ALPHA); 
			
			rgbLineTex.allocate(1,7, GL_RGBA);
			unsigned char rgb_line_data[7*4] = {
				255,0,0,0,	255,0,0,255,
				//r	DARK		BRIGHT
				0,255,0,0,	0,255,0,255,
				//g	DARK		BRIGHT
				0,0,255,0,	0,0,255,255,
				//b	DARK		BRIGHT
				0,0,255,0
				//	DARK
			};
			rgbLineTex.loadData(rgb_line_data, 1,7, GL_RGBA);

			bSetup = true;
		}
	}
	
	namespace renderer {
		void drawTexLine(ofxPoint2f& startpoint, ofxPoint2f& endpoint, ofTexture& tex) 
		{
			float dist = startpoint.distance(endpoint);
			float angle = atan2(endpoint.y-startpoint.y,
								endpoint.x-startpoint.x);;
			
			glPushMatrix();
			
			glTranslatef(startpoint.x, startpoint.y, 0.0);
			glRotatef(angle*180.0/PI, 0, 0, 1.0);
			
			tex.draw(style::lineHeight/2,-style::lineHeight/2,
					 dist - style::lineHeight,style::lineHeight);
			
			glPopMatrix();
		}

		void drawArrow(types::lineSegment segment,
					   types::arrow_type_t arrowtype)
		{
			drawLine(segment);
			
			if (arrowtype == types::ARROW_TYPE_BOTH || arrowtype == types::ARROW_TYPE_START)
				ofCircle(segment.from.x, segment.from.y, style::arrowheadRadius);
			
			if (arrowtype == types::ARROW_TYPE_BOTH || arrowtype == types::ARROW_TYPE_END)
				ofCircle(segment.to.x, segment.to.y, style::arrowheadRadius);
		}
		
		void drawArrow(ofxPoint2f& startpoint, ofxPoint2f& endpoint,
					   types::actor_type_t starttype, types::actor_type_t endtype,
					   types::arrow_type_t arrowtype)
		{
			drawLine(startpoint, endpoint, starttype, endtype);

			if (arrowtype == types::ARROW_TYPE_BOTH || arrowtype == types::ARROW_TYPE_START)
				ofCircle(startpoint.x, startpoint.y, style::arrowheadRadius);

			if (arrowtype == types::ARROW_TYPE_BOTH || arrowtype == types::ARROW_TYPE_END)
				ofCircle(endpoint.x, endpoint.y, style::arrowheadRadius);
		}

		void drawLine(types::lineSegment segment) {
			drawLine(segment.from, segment.to, segment.from_type, segment.to_type);
		}
		
		void drawLine(ofxPoint2f& startpoint, ofxPoint2f& endpoint,
					  types::actor_type_t starttype, types::actor_type_t endtype)
		{
			switch (starttype)
			{
				case types::ACTOR_TYPE_RGB:
					drawTexLine(startpoint, endpoint, style::rgbLineTex);
					break;
				case types::ACTOR_TYPE_GRAY:
					drawTexLine(startpoint, endpoint, style::grayLineTex);
					break;
			}
		}

		void grayLine(ofxPoint2f& startpoint, ofxPoint2f& endpoint) 
		{
			drawTexLine(startpoint, endpoint, style::grayLineTex);
		}

		void rgbLine(ofxPoint2f& startpoint, ofxPoint2f& endpoint) 
		{
			drawTexLine(startpoint, endpoint, style::rgbLineTex);
		}
	}
}
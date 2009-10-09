#include "ofxCvGrayscaleImage.h"
#include "ofxCvColorImage.h"
#include "ofxCvFloatImage.h"
#include "ofxCvRgbaImage.h"

//--------------------------------------------------------------------------------
ofxCvRgbaImage::ofxCvRgbaImage()
{
    init();
}

//--------------------------------------------------------------------------------
ofxCvRgbaImage::ofxCvRgbaImage( const ofxCvRgbaImage& _mom )
{
    init();
    if( _mom.bAllocated ) {
        // cast non-const,  to get read access to the mon::cvImage
        ofxCvRgbaImage& mom = const_cast<ofxCvRgbaImage&>(_mom);
        allocate(mom.width, mom.height);
        cvCopy( mom.getCvImage(), cvImage, 0 );
    }
	else {
        ofLog(OF_LOG_NOTICE, "in ofxCvRgbaImage copy constructor, mom not allocated");
    }
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::init() 
{
    ipldepth = IPL_DEPTH_8U;
    iplchannels = 4;
    gldepth = GL_UNSIGNED_BYTE;
    glchannels = GL_RGBA;
    cvGrayscaleImage = NULL;
    cvColorImage = NULL;
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::clear() 
{
    if (bAllocated == true && cvGrayscaleImage != NULL)
	{
        cvReleaseImage( &cvGrayscaleImage );
    }
    ofxCvImage::clear();    //call clear in base class
}




// Set Pixel Data

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::set( float value )
{
    cvSet(cvImage, cvScalar(value, value, value));
    flagImageChanged();
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::set(int valueR, int valueG, int valueB, int valueA)
{
    cvSet(cvImage, cvScalar(valueR, valueG, valueB, valueA));
    flagImageChanged();
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::operator -= ( float value ) 
{
	cvSubS( cvImage, cvScalar(value, value, value), cvImageTemp );
	swapTemp();
    flagImageChanged();
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::operator += ( float value ) 
{
	cvAddS( cvImage, cvScalar(value, value, value), cvImageTemp );
	swapTemp();
    flagImageChanged();
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::setFromPixels( unsigned char* _pixels, int w, int h ) 
{
    ofRectangle roi = getROI();
    ofRectangle inputROI = ofRectangle( roi.x, roi.y, w, h);
    ofRectangle iRoi = getIntersectionROI( roi, inputROI );
	
    if( iRoi.width > 0 && iRoi.height > 0 ) {
        // copy pixels from _pixels, however many we have or will fit in cvImage
		if( cvColorImage == NULL ) {
			cvColorImage = cvCreateImage( cvSize(cvImage->width,cvImage->height), IPL_DEPTH_8U, 3 );
		}
		for( int i=0; i < iRoi.height; i++ ) {
            memcpy( cvColorImage->imageData + ((i+(int)iRoi.y)*cvColorImage->widthStep) + (int)iRoi.x*3,
				   _pixels + (i*w*3),
				   (int)(iRoi.width*3) );
        }
		cvCvtColor(cvColorImage, cvImage, CV_RGB2RGBA);
        flagImageChanged();
    }
	else {
        ofLog(OF_LOG_ERROR, "in setFromPixels, ROI mismatch");
    }
}


//--------------------------------------------------------------------------------
void ofxCvRgbaImage::setAlpha(ofxCvGrayscaleImage& alpha) 
{
	if( alpha.width == width && alpha.height == height)
	{
		cvCvtPlaneToPix(NULL, NULL, NULL, alpha.getCvImage(), cvImage);
        flagImageChanged();
    }
	else {
        ofLog(OF_LOG_ERROR, "in setFromPixels, images are different sizes");
    }
}


// From RGBA
//--------------------------------------------------------------------------------
void ofxCvRgbaImage::setFromGrayscalePlanarImages(ofxCvGrayscaleImage& red,
												  ofxCvGrayscaleImage& green,
												  ofxCvGrayscaleImage& blue,
												  ofxCvGrayscaleImage& alpha)
	{
	if( red.width == width && red.height == height &&
	   green.width == width && green.height == height &&
	   blue.width == width && blue.height == height &&
	   alpha.width == width && alpha.height == height )
    {
		cvCvtPlaneToPix(red.getCvImage(),
						green.getCvImage(),
						blue.getCvImage(),
						alpha.getCvImage(),
						cvImage);
		flagImageChanged();
	}
	else {
        ofLog(OF_LOG_ERROR, "in setFromGrayscalePlanarImages, images are different sizes");
	}
}

// From RGB
//--------------------------------------------------------------------------------
void ofxCvRgbaImage::setFromGrayscalePlanarImages(ofxCvGrayscaleImage& red,
												  ofxCvGrayscaleImage& green,
												  ofxCvGrayscaleImage& blue)
	{
	if( red.width == width && red.height == height &&
	   green.width == width && green.height == height &&
	   blue.width == width && blue.height == height )
    {
		cvCvtPlaneToPix(red.getCvImage(),
						green.getCvImage(),
						blue.getCvImage(),
						NULL,
						cvImage);
		flagImageChanged();
	}
	else {
        ofLog(OF_LOG_ERROR, "in setFromGrayscalePlanarImages, images are different sizes");
	}
}


//--------------------------------------------------------------------------------
void ofxCvRgbaImage::operator = ( unsigned char* _pixels ) 
{
    setFromPixels( _pixels, width, height );
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::operator = ( const ofxCvGrayscaleImage& _mom ) 
{
    // cast non-const,  no worries, we will reverse any chages
    ofxCvGrayscaleImage& mom = const_cast<ofxCvGrayscaleImage&>(_mom);
	if( pushSetBothToTheirIntersectionROI(*this,mom) ) {
		cvCvtColor( mom.getCvImage(), cvImage, CV_GRAY2RGBA );
        popROI();       //restore prevoius ROI
        mom.popROI();   //restore prevoius ROI
        flagImageChanged();
	}
	else {
        ofLog(OF_LOG_ERROR, "in =, ROI mismatch");
	}
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::operator = ( const ofxCvRgbaImage& _mom ) 
{
    if(this != &_mom) {  //check for self-assignment
        // cast non-const,  no worries, we will reverse any chages
        ofxCvRgbaImage& mom = const_cast<ofxCvRgbaImage&>(_mom);
        if( pushSetBothToTheirIntersectionROI(*this,mom) ) {
            cvCopy( mom.getCvImage(), cvImage, 0 );
            popROI();       //restore prevoius ROI
            mom.popROI();   //restore prevoius ROI
            flagImageChanged();
        }
	else {
            ofLog(OF_LOG_ERROR, "in =, ROI mismatch");
        }
    }
	else {
        ofLog(OF_LOG_WARNING, "in =, you are assigning a ofxCvRgbaImage to itself");
    }
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::operator = ( const ofxCvColorImage& _mom ) 
{
    // cast non-const,  no worries, we will reverse any chages
    ofxCvColorImage& mom = const_cast<ofxCvColorImage&>(_mom);

/*	if( pushSetBothToTheirIntersectionROI(*this,mom) ) 
{
		cvCvtColor(mom.getCvImage(), cvImage, CV_RGB2RGBA);
        popROI();       //restore prevoius ROI
        mom.popROI();   //restore prevoius ROI
        cvSetImageROI(cvGrayscaleImage, cvRect(roiX,roiY,width,height));
        flagImageChanged();
*/
	if (true)
	{
		cvCvtColor(mom.getCvImage(), cvImage, CV_RGB2RGBA);
	}
	else {
        ofLog(OF_LOG_ERROR, "in =, ROI mismatch");
	}

//	ofLog(OF_LOG_ERROR, "operator = ( const ofxCvColorImage& _mom) not implemented");
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::operator = ( const ofxCvFloatImage& _mom ) 
{
    // cast non-const,  no worries, we will reverse any chages
    ofxCvFloatImage& mom = const_cast<ofxCvFloatImage&>(_mom);
	if( pushSetBothToTheirIntersectionROI(*this,mom) ) {
        if( cvGrayscaleImage == NULL ) {
            cvGrayscaleImage = cvCreateImage( cvSize(cvImage->width,cvImage->height), IPL_DEPTH_8U, 1 );
        }
        cvSetImageROI(cvGrayscaleImage, cvRect(roiX,roiY,width,height));
		cvConvertScale( mom.getCvImage(), cvGrayscaleImage, 1, 0 );
		cvCvtColor( cvGrayscaleImage, cvImage, CV_GRAY2RGBA );
        popROI();       //restore prevoius ROI
        mom.popROI();   //restore prevoius ROI
        cvSetImageROI(cvGrayscaleImage, cvRect(roiX,roiY,width,height));
        flagImageChanged();
	}
	else {
        ofLog(OF_LOG_ERROR, "in =, ROI mismatch");
	}
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::operator = ( const IplImage* _mom ) 
{
    ofxCvImage::operator = (_mom);
}


// Get Pixel Data

//--------------------------------------------------------------------------------
unsigned char* ofxCvRgbaImage::getPixels() 
{
    if(bPixelsDirty)
	{
        if(pixels == NULL)
	{
            // we need pixels, allocate it
            pixels = new unsigned char[width*height*iplchannels];
            pixelsWidth = width;
            pixelsHeight = height;
        } else if(pixelsWidth != width || pixelsHeight != height)
	{
            // ROI changed, reallocate pixels for new size
            delete pixels;
            pixels = new unsigned char[width*height*iplchannels];
            pixelsWidth = width;
            pixelsHeight = height;
        }

        // copy from ROI to pixels
        for( int i = 0; i < height; i++ ) {
            memcpy( pixels + (i*width*iplchannels),
				   cvImage->imageData + ((i+roiY)*cvImage->widthStep) + roiX*iplchannels,
				   width*iplchannels );
        }
        bPixelsDirty = false;
    }
	return pixels;
}

// From RGBA
//--------------------------------------------------------------------------------
void ofxCvRgbaImage::convertToGrayscalePlanarImages(ofxCvGrayscaleImage& red,
													ofxCvGrayscaleImage& green,
													ofxCvGrayscaleImage& blue,
													ofxCvGrayscaleImage& alpha)
	{
	if( red.width == width && red.height == height &&
	   green.width == width && green.height == height &&
	   blue.width == width && blue.height == height &&
	   alpha.width == width && alpha.height == height )
    {
        cvCvtPixToPlane(cvImage,
						red.getCvImage(),
						green.getCvImage(),
						blue.getCvImage(),
						alpha.getCvImage());
	}
	else {
        ofLog(OF_LOG_ERROR, "in convertToGrayscalePlanarImages, images are different sizes");
	}
}


// From RGB
//--------------------------------------------------------------------------------
void ofxCvRgbaImage::convertToGrayscalePlanarImages(ofxCvGrayscaleImage& red,
													ofxCvGrayscaleImage& green,
													ofxCvGrayscaleImage& blue)
	{
	if( red.width == width && red.height == height &&
	   green.width == width && green.height == height &&
	   blue.width == width && blue.height == height )
    {
        cvCvtPixToPlane(cvImage,
						red.getCvImage(),
						green.getCvImage(),
						blue.getCvImage(),
						NULL);
	}
	else {
        ofLog(OF_LOG_ERROR, "in convertToGrayscalePlanarImages, images are different sizes");
	}
}



// Draw Image



// Image Filter Operations

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::contrastStretch() 
{
	ofLog(OF_LOG_WARNING, "in contrastStratch, not implemented for ofxCvRgbaImage");
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::convertToRange(float min, float max )
{
    rangeMap( cvImage, 0,255, min,max);
    flagImageChanged();
}



// Image Transformation Operations

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::resize( int w, int h ) 
{
	
    // note, one image copy operation could be ommitted by
    // reusing the temporal image storage
	
    IplImage* temp = cvCreateImage( cvSize(w,h), IPL_DEPTH_8U, iplchannels );
    cvResize( cvImage, temp );
    clear();
    allocate( w, h );
    cvCopy( temp, cvImage );
    cvReleaseImage( &temp );
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::scaleIntoMe( ofxCvImage& mom, int interpolationMethod )
{
    //for interpolation you can pass in:
    //CV_INTER_NN - nearest-neigbor interpolation,
    //CV_INTER_LINEAR - bilinear interpolation (used by default)
    //CV_INTER_AREA - resampling using pixel area relation. It is preferred method
    //                for image decimation that gives moire-free results. In case of
    //                zooming it is similar to CV_INTER_NN method.
    //CV_INTER_CUBIC - bicubic interpolation.
	
    if( mom.getCvImage()->nChannels == cvImage->nChannels &&
	   mom.getCvImage()->depth == cvImage->depth ) {
		
        if ((interpolationMethod != CV_INTER_NN) &&
            (interpolationMethod != CV_INTER_LINEAR) &&
            (interpolationMethod != CV_INTER_AREA) &&
            (interpolationMethod != CV_INTER_CUBIC) ){
            ofLog(OF_LOG_WARNING, "in scaleIntoMe, setting interpolationMethod to CV_INTER_NN");
    		interpolationMethod = CV_INTER_NN;
    	}
        cvResize( mom.getCvImage(), cvImage, interpolationMethod );
        flagImageChanged();
		
    }
	else {
        ofLog(OF_LOG_ERROR, "in scaleIntoMe, mom image type has to match");
    }
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::convertRgbToHsv()
{
    cvCvtColor( cvImage, cvImageTemp, CV_RGBA2RGB);
    cvCvtColor( cvImageTemp, cvImageTemp, CV_RGB2HSV);
    swapTemp();
    flagImageChanged();
}

//--------------------------------------------------------------------------------
void ofxCvRgbaImage::convertHsvToRgb()
{
    cvCvtColor( cvImage, cvImageTemp, CV_HSV2RGB);
    cvCvtColor( cvImageTemp, cvImageTemp, CV_RGB2RGBA);
    swapTemp();
    flagImageChanged();
}


#pragma once
#include "settings.h"

#include "VideoFilter.h"

// Input
#include "filters/PassthroughFilter.h"
#include "filters/GrayscalePassthroughFilter.h"

// Pre-processing
#include "filters/HomographyFilter.h"
#include "filters/DifferencingFilter.h"

// Thresholding
#include "filters/ThresholdingFilter.h"
#include "filters/SimpleThresholdingFilter.h"
#include "filters/AdaptiveThresholdingFilter.h"
#ifdef USE_OPENCV_TRUNK
#include "filters/AdaptiveSkinFilter.h"
#endif

// Background Subtraction
#include "filters/BackgroundSubtractionFilter.h"
#include "filters/ZivkovicBgFilter.h"
#include "filters/LiBgFilter.h"

// Low-level Features
#include "filters/ContourFindingFilter.h"
#include "filters/CannyEdgeFilter.h"
#include "filters/SobelFilter.h"
#ifdef USE_OPENCL
#include "filters/SobelFilterOcl.h"
#endif

// High-level Features
#include "filters/FiducialTrackingFilter.h"

// Algorithms
#include "filters/SparseOpticalFlowFilter.h"
#ifdef USE_OPENCV_TRUNK
#include "filters/DenseOpticalFlowFilter.h"
#endif

// Capturing
#include "filters/CaptureFilter.h"
//#include "filters/CalibrationFilter.h"
#ifdef USE_OPENCL
#include "filters/VoxelBufferFilter.h"
#endif


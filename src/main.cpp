#include "testApp.h"

#ifdef CPU_PROFILE
	#include <google/profiler.h>
#endif

#ifdef HEAP_PROFILE
	#include <google/heap-profiler.h>
#endif

testApp *myApp;
int main( )
{
	ofSetDataPathRoot("../Resources/");
#ifdef START_FULLSCREEN_SECONDARY
	ofSetupOpenGL(1280, 720, OF_FULLSCREEN);			// <-------- setup the GL context
#else
	ofSetupOpenGL(1280, 720, OF_WINDOW);
#endif
	myApp = new testApp;

#ifdef MANUAL_PROFILE
#ifdef CPU_PROFILE
	ProfilerStart("cpu_prof.out");
#endif
#ifdef HEAP_PROFILE
	HeapProfilerStart("heap_prof");
#endif
#endif

	ofRunApp(myApp);

#ifdef MANUAL_PROFILE
#ifdef CPU_PROFILE
	ProfilerStop();
#endif
#ifdef HEAP_PROFILE
	HeapProfilerStop();
#endif
#endif	
	delete myApp;
}

extern testApp *myApp;
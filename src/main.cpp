#include "testApp.h"

#define	CPU_PROFILE
#undef	HEAP_PROFILE
#undef	MANUAL_PROFILE

#ifdef CPU_PROFILE
	#include <google/profiler.h>
#endif

#ifdef HEAP_PROFILE
	#include <google/heap-profiler.h>
#endif

testApp *myApp;
int main( ){
	ofSetDataPathRoot("../Resources/");
	ofSetupOpenGL(1280, 720, OF_WINDOW);			// <-------- setup the GL context
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

#ifdef CPU_PROFILE
	ProfilerStop();
#endif
#ifdef HEAP_PROFILE
	HeapProfilerStop();
#endif
}

extern testApp *myApp;
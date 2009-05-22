#include "testApp.h"
#include <google/profiler.h>

testApp *myApp;
int main( ){
	ofSetupOpenGL(1280, 720, OF_WINDOW);			// <-------- setup the GL context
	myApp = new testApp;
	ProfilerStart("of.prof");
	ofRunApp(myApp);
	ProfilerStop();
}

extern testApp *myApp;
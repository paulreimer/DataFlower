#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"
#include <xapian.h>

class DataSystem : public ofxMSAInteractiveObject {
public:
	DataSystem(string lang="english");
	virtual ~DataSystem();

	void setup();

	void query(string q);
	
	void destroy();

	bool verbose;

private:
	Xapian::QueryParser qp;
	Xapian::Database db;
	Xapian::SimpleStopper stopper;
	Xapian::Stem stemmer;
};

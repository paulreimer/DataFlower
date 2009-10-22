#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"
#include <xapian.h>

class DataSystem : public ofxMSAInteractiveObject 
{
public:
	DataSystem(const std::string lang="english");
	virtual ~DataSystem();

	void setup();

	void query(const std::string q);
	
	void destroy();

	bool verbose;

private:
	Xapian::QueryParser qp;
	Xapian::Database db;
	const Xapian::SimpleStopper stopper;
	const Xapian::Stem stemmer;
};

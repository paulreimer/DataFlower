#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#ifdef USE_XAPIAN
#include <xapian.h>
#endif

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
#ifdef USE_XAPIAN
	Xapian::QueryParser qp;
	Xapian::Database db;
	const Xapian::SimpleStopper stopper;
	const Xapian::Stem stemmer;
#endif
};

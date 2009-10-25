#pragma once

#include "DataSystem.h"

#ifdef USE_DATA

static const char * sw[] = {
    "a", "about", "an", "and", "are", "as", "at",
	"be", "by",
	"en",
	"for", "from",
	"how",
	"i", "in", "is", "it",
	"of", "on", "or",
	"that", "the", "this", "to",
	"was", "what", "when", "where", "which", "who", "why", "will", "with"
};

DataSystem::DataSystem(const std::string lang) :
stemmer(lang),
stopper(sw, sw + sizeof(sw) / sizeof(sw[0]))
{
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("DataSystem::DataSystem()\n");
}

DataSystem::~DataSystem()
{
	if (verbose) printf("DataSystem::~DataSystem()\n");
	destroy();
}

void DataSystem::setup()
{
	try {
		// Parse the query string to produce a Xapian::Query object.
		qp.set_default_op(Xapian::Query::OP_OR);
		qp.set_stemmer(stemmer);
		qp.set_database(db);
		qp.set_stopper(&stopper);

		qp.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);

	} catch (const Xapian::Error &e)
	{
		printf("%s", e.get_description());
	}
}

void DataSystem::query(const std::string q)
{
	try {
		// Start an enquire session.
		Xapian::Enquire enquire(db);

		Xapian::Query query = qp.parse_query(q);
		printf("Parsed query is: %s", query.get_description().c_str());

		// Find the top 10 results for the query.
		enquire.set_query(query);
		Xapian::MSet matches = enquire.get_mset(0, 10);

		// Display the results.
		printf("%d results found.\n", matches.get_matches_estimated());
		printf("Matches 1-$d:\n", matches.size());

		for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i)
	{
			printf("%d: %d\% docid= [%s]\n\n",
				   i.get_rank() + 1,
				   i.get_percent(),
				   i.get_document().get_data().c_str());
		}
	} catch (const Xapian::Error &e)
	{
		printf("DataSystem, query error: %s", e.get_description());
	}
}

void DataSystem::destroy()
{
	if (verbose) printf("DataSystem::destroy()\n");
}

#endif

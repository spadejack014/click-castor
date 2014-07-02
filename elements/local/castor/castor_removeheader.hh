#ifndef CLICK_CASTOR_REMOVEHEADER_HH
#define CLICK_CASTOR_REMOVEHEADER_HH

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

/**
 * Removes the Castor header
 */
class CastorRemoveHeader: public Element {
public:
	CastorRemoveHeader();
	~CastorRemoveHeader();
		
	const char *class_name() const { return "CastorRemoveHeader"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return PUSH; }

	void push(int, Packet *);
};

CLICK_ENDDECLS

#endif


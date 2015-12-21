#ifndef CLICK_CASTOR_FLOW_HH
#define CLICK_CASTOR_FLOW_HH

#include <click/element.hh>
#include "../castor.hh"

CLICK_DECLS

class PacketLabel {
public:
	size_t num;
	FlowId fid;
	FlowAuth fauth;
	PacketId pid;
	AckAuth aauth;
};

class CastorFlow {
public:
	CastorFlow() {}
	virtual ~CastorFlow() {}

	/**
	 * Returns a fresh label or a dummy label if this flow is dead.
	 */
	virtual PacketLabel freshLabel() = 0;
	/**
	 * Returns true if this flow is still alive, i.e., if there exist more fresh labels.
	 * Returns false otherwise.
	 */
	virtual bool isAlive() const = 0;
};

CLICK_ENDDECLS

#endif

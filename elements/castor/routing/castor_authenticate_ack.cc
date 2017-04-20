/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <click/config.h>
#include <click/args.hh>
#include "castor_authenticate_ack.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorAuthenticateAck::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, this, errh)
		.read_mp("History", ElementCastArg("CastorHistory"), history)
		.read_or_set_p("Version", version, 1)
		.complete();
}

void CastorAuthenticateAck::push(int, Packet* p) {

	const PacketId& pid = CastorAnno::hash_anno(p);
	const NeighborId& src = CastorAnno::src_id_anno(p);
	int port = 0;

	if (!history->hasPkt(pid)) {
		port = 1; // never forwarded corresponding PKT -> discard
	} else if (history->hasAckFrom(pid, src)) {
		port = 2; // already received ACK from this neighbor -> discard
	} else if (version <= 1) {
		const NeighborId& routedTo = history->routedTo(pid);
		if (routedTo != src && routedTo != NeighborId::make_broadcast()) {
			port = 3; // received ACK from a neighbor we never forwarded the PKT to -> discard (standard Castor)
		}
	} else {
#if 1
		const NeighborId& routedTo = history->routedTo(pid);
		if (routedTo != src && routedTo != NeighborId::make_broadcast()) {
			port = 3; // received ACK from a neighbor we never forwarded the PKT to -> discard (standard Castor)
		}
#elif 0
		// Check whether the ACK was received from one of our PKT senders
		if (history->routedTo(pid) != NodeId::make_broadcast()) {
			for (size_t i = 1; i < history->getPkts(pid); i++) {
				const NodeId pktSender = history->getPktSenders(pid)[i];
				if (src == pktSender) {
					port = 4; // received ACK from a neighbor that has forwarded us the PKT -> discard (Castor improvement)
					break;
				}
			}
		}
#endif
		const NeighborId& firstPktSender = history->getPktSenders(pid)[0];
		if (src == firstPktSender) {
			port = 4; // received ACK from the neighbor that initially forwarded us the PKT -> discard (Castor improvement)
		}
	}

	output(port).push(p);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAuthenticateAck)

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
#include "neighbor_auth_check_icv.hh"
#include "../castor_anno.hh"
#include "../castor.hh"
#include "forwarder_list.hh"

CLICK_DECLS

int NeighborAuthCheckICV::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), crypto)
			.read_or_set_p("ENABLE", enable, true)
			.complete();
}

Packet* NeighborAuthCheckICV::simple_action(Packet *p) {
	if (!enable)
		return p;

	if (p->length() < sizeof(ForwarderList)) {
		checked_output_push(1, p);
		return 0;
	}

	const ForwarderList& fl = *reinterpret_cast<const ForwarderList*>(p->data());

	const SymmetricKey* key = crypto->getSharedKey(CastorAnno::src_id_anno(p));
	if (!key) {
		click_chatter("Could not find shared key for host %s. Discarding PKT...", CastorAnno::src_id_anno(p).unparse().c_str());
		checked_output_push(1, p);
		return 0;
	}

	int payload_length = p->length() - fl.nicv * icv_BYTES;
	if (payload_length < 0) {
		click_chatter("Malformed packet: packet is supposed to contain more ICVs than would fit in it. Discard.");
		checked_output_push(1, p);
		return 0;
	}
	// if any ICV is valid, proceed
	ICV calc;
	crypto->auth(calc, p->data(), payload_length, key->data());
	for (unsigned int i = 0; i < fl.nicv; i++) {
		if (calc == ICV(p->data() + payload_length + i * icv_BYTES))
			return p;
	}

	// no ICV match
	checked_output_push(1, p);
	return 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborAuthCheckICV)

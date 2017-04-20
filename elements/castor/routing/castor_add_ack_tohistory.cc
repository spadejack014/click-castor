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
#include "castor_add_ack_tohistory.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorAddAckToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("History", ElementCastArg("CastorHistory"), history)
			.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), flowtable)
			.complete();
}

Packet* CastorAddAckToHistory::simple_action(Packet *p) {
	const CastorAck& ack = *reinterpret_cast<const CastorAck*>(p->data());
	const PacketId& pid = CastorAnno::hash_anno(p);
	const NeighborId& from = CastorAnno::src_id_anno(p);
	if (history->hasAck(pid)) {
		history->addAckFor(pid, from);
	} else {
		history->addFirstAckFor(pid, from, ack.auth);
	}
	flowtable->get(history->getFlowId(pid)).set_ack(history->k(pid), from);
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddAckToHistory)

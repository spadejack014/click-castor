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
#include "samanagement.hh"

CLICK_DECLS

int SAManagement::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("NodeId", myNodeId)
			.read_mp("NeighborId", myNeighborId)
			.read_or_set_p("SYM_KEY_LENGTH", symmetricKeyLength, 32)
			.complete();
}

void SAManagement::add(const NodeId& node, const SecurityAssociation& sa) {
	sas_e2e[node].push_back(sa);
}

void SAManagement::add(const NeighborId& node, const SecurityAssociation& sa) {
	sas_h2h[node].push_back(sa);
}

const SecurityAssociation* SAManagement::get(const NodeId& node, SecurityAssociation::Type type) {
	if (sas_e2e.count(node) > 0) {
		for (const auto& sa : sas_e2e[node])
			if (sa.type == type)
				return &sa;
	}

	// If no entry currently exists, generate one
	if (type == SecurityAssociation::sharedsecret) {
		add(node, genereateSymmetricSA(node));
		return get(node, type);
	}

	return 0;
}

const SecurityAssociation* SAManagement::get(const NeighborId& node, SecurityAssociation::Type type) {
	if (sas_h2h.count(node) > 0) {
		for (const auto& sa : sas_h2h[node])
			if (sa.type == type)
				return &sa;
	}

	// If no entry currently exists, generate one
	if (type == SecurityAssociation::sharedsecret) {
		add(node, genereateSymmetricSA(node));
		return get(node, type);
	}

	return 0;
}

SecurityAssociation SAManagement::genereateSymmetricSA(const NodeId& node) {
	Vector<uint8_t> key;
	key.reserve(symmetricKeyLength);
	if (myNodeId.addr() < node.addr()) {
		for (unsigned int i = 0; i < sizeof(NodeId); i++)
			key.push_back(myNodeId.data()[i]);
		for (unsigned int i = 0; i < sizeof(NodeId); i++)
			key.push_back(node.data()[i]);
	} else {
		for (unsigned int i = 0; i < sizeof(NodeId); i++)
			key.push_back(node.data()[i]);
		for (unsigned int i = 0; i < sizeof(NodeId); i++)
			key.push_back(myNodeId.data()[i]);
	}
	while ((unsigned int) key.size() < symmetricKeyLength)
		key.push_back(0);
	return SecurityAssociation(SecurityAssociation::sharedsecret, key);
}

SecurityAssociation SAManagement::genereateSymmetricSA(const NeighborId& node) {
	Vector<uint8_t> key;
	key.reserve(symmetricKeyLength);
	if (myNeighborId.unparse() < node.unparse()) {
		for (unsigned int i = 0; i < sizeof(NeighborId); i++)
			key.push_back(myNeighborId.data()[i]);
		for (unsigned int i = 0; i < sizeof(NeighborId); i++)
			key.push_back(node.data()[i]);
	} else {
		for (unsigned int i = 0; i < sizeof(NeighborId); i++)
			key.push_back(node.data()[i]);
		for (unsigned int i = 0; i < sizeof(NeighborId); i++)
			key.push_back(myNeighborId.data()[i]);
	}
	while ((unsigned int) key.size() < symmetricKeyLength)
		key.push_back(0);
	return SecurityAssociation(SecurityAssociation::sharedsecret, key);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SAManagement)

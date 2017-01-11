#include <click/config.h>
#include "merkle_tree.hh"

CLICK_DECLS

MerkleTree::MerkleTree(const Hash in[], unsigned int length, const Crypto& crypto) : crypto(crypto) {
	if (!(length && !(length & (length - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", length);
		return;
	}
	_leaves = length;
	_height = log2(_leaves);
	_flat = new Hash[index(_height + 1)]; // +1 for root node

	unsigned int d = _height;
	// hash input values
	for (unsigned int k = 0; k < _leaves; k++) {
		crypto.hash(element(d, k), in[k]);
	}
	// create intermediate nodes up to the root
	for (; d > 0; d--) {
		for (unsigned int i = 0; i < nodes_at_depth(d); i += 2) {
			crypto.hash(element(d - 1, i >> 1),
					    element(d, i) + element(d, i + 1));
		}
	}
}

MerkleTree::MerkleTree(const Hash& root, unsigned int length, const Crypto& crypto) : crypto(crypto) {
	if (!(length && !(length & (length - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", length);
		return;
	}
	_leaves = length;
	_height = log2(_leaves);
	_flat = new Hash[index(_height + 1)](); // +1 -> for root node; () -> set nodes to zero
	_flat[0] = root;
}

MerkleTree::~MerkleTree() {
	delete [] _flat;
}

const Hash& MerkleTree::root() const {
	return _flat[0];
}

void MerkleTree::path_to_root(unsigned int k, Hash siblings[], unsigned int max) const {
	for (unsigned int i = index(_height, k), si = 0; i > 0 && si < max; i = parent(i), si++) {
		siblings[si] = _flat[sibling(i)];
	}
}

int MerkleTree::valid_leaf(unsigned int k, const Hash& in, const Hash siblings[], unsigned int n) const {
	// First hash the input
	Hash current;
	crypto.hash(current, in);
	unsigned int i = index(_height, k);
	for (unsigned int l = 0; i > 0 && l < n; i = parent(i), l++) {
		if (k & (1 << l)) { // sibling is left child
			crypto.hash(current, siblings[l] + current);
		} else { // sibling is right child
			crypto.hash(current, current + siblings[l]);
		}
	}
	if (_flat[i] == Hash())
		return -2;
	return (_flat[i] == current) ? 0 : -1;
}

void MerkleTree::add(unsigned int k, const Hash& in, const Hash siblings[], unsigned int n) {
	Hash current;
	crypto.hash(current, in);
	unsigned int i = index(_height, k);
	_flat[i] = current;
	for (unsigned int h = 0; i > 0 && h < n; i = parent(i), h++) {
		_flat[sibling(i)] = siblings[h];
		if (_flat[parent(i)] == Hash()) {
			if (k & (1 << h)) { // sibling is left child
				crypto.hash(_flat[parent(i)], _flat[sibling(i)] + _flat[i]);
			} else { // sibling is right child
				crypto.hash(_flat[parent(i)], _flat[i] + _flat[sibling(i)]);
			}
		} else {
			// we have valid tree elements from here on up
			break;
		}
	}
}

bool MerkleTree::validate(unsigned int k, const Hash& in, const Hash siblings[], unsigned int h, const Hash& root, const Crypto& crypto) {
	// First hash the input
	Hash current;
	crypto.hash(current, in);
	for(unsigned int s = 0; s < h; s++) {
		if (k & (1 << s)) { // sibling is left child
			crypto.hash(current, siblings[s] + current);
		} else { // sibling is right child
			crypto.hash(current, current + siblings[s]);
		}
	}
	return current == root;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(MerkleTree)

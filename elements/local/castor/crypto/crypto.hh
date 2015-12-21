#ifndef CLICK_CRYPTO_HH
#define CLICK_CRYPTO_HH
#include <click/element.hh>
#include "../node_id.hh"
#include "../hash.hh"
#include "samanagement.hh"
#include <sodium.h>

CLICK_DECLS

typedef Vector<uint8_t> SymmetricKey;

class Crypto: public Element {
public:
	const char *class_name() const { return "Crypto"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler *);

	/**
	 * Returns a new SymmetricKey instance or NULL if no corresponding key exists
	 */
	const SymmetricKey* getSharedKey(NodeId) const;
	template<unsigned int S>
	inline void encrypt(Buffer<S>& cipher, const Buffer<S>& plain, const SymmetricKey& key) const {
		assert(key.size() == crypto_stream_aes128ctr_KEYBYTES);
		crypto_stream_aes128ctr_xor(cipher.data(), plain.data(), plain.size(), nonce, key.begin());
	}
	template<unsigned int S>
	inline Buffer<S> encrypt(const Buffer<S>& plain, const SymmetricKey& key) const {
		Buffer<S> tmp;
		encrypt(tmp, plain, key);
		return tmp;
	}

	template<unsigned int S>
	inline void decrypt(Buffer<S>& plain, const Buffer<S>& cipher, const SymmetricKey& key) const {
		// encryption and decryption is the same for a stream cipher
		encrypt(plain, cipher, key);
	}
	template<unsigned int S>
	inline Buffer<S> decrypt(const Buffer<S>& cipher, const SymmetricKey& key) const {
		return encrypt(cipher, key);
	}

	template<unsigned int S>
	inline void random(Buffer<S>& buf) const {
		random(buf.data(), buf.size());
	}
	template<unsigned int S>
	inline Buffer<S> random() const {
		Buffer<S> buf;
		random(buf);
		return buf;
	}

	template<unsigned int S>
	inline void hash(Buffer<S>& out, const uint8_t* in, unsigned int n) const {
		truncated_hash(out.data(), out.size(), in, n);
	}
	template<unsigned int S>
	inline Buffer<S> hash(const uint8_t* in, unsigned int n) const {
		Buffer<S> tmp;
		hash(tmp, in, n);
		return tmp;
	}
	template<unsigned int S, unsigned int S2>
	inline void hash(Buffer<S>& out, const Buffer<S2>& in) const {
		truncated_hash(out.data(), out.size(), in.data(), in.size());
	}
	template<unsigned int S, unsigned int S2>
	inline Buffer<S> hash(const Buffer<S2>& in) const {
		Buffer<S> tmp;
		hash(tmp, in);
		return tmp;
	}
	template<unsigned int S>
	inline Buffer<S> hash(const Buffer<S>& in) const {
		Buffer<S> tmp;
		hash(tmp, in);
		return tmp;
	}
private:
	SAManagement* sam;
	uint8_t nonce[crypto_stream_aes128ctr_NONCEBYTES];

	void random(uint8_t* buf, unsigned int length) const;
	void hash(uint8_t* out, const uint8_t* in, unsigned int n) const;
	void truncated_hash(uint8_t* out, unsigned int outlen, const uint8_t* in, unsigned int n) const;
};

CLICK_ENDDECLS
#endif

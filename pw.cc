#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/objects.h>
#include <openssl/rand.h>

using namespace std;

#define CLOAK_KEY "/opt/cascade/vault/cloak_key"

int
openssl_enc(const string &plain, string &encrypted)
{
	string cmd = "echo " + plain;
	cmd += " | ";
	cmd += "openssl enc -e -a -aes256 -pass file:";
	cmd += CLOAK_KEY;
	cmd += " -md SHA512";
	cout << cmd << endl;

	FILE *pipe = popen(cmd.c_str(), "r");
	if (pipe == NULL) return -1;

	char buffer[128];
	string result;
	while (fgets(buffer, sizeof(buffer)-1, pipe) != NULL)
		result += buffer;
	pclose(pipe);

	encrypted = result;
	return 0;
}

int
openssl_dec(string &plain, const string &encrypted)
{
	string cmd = "echo " + encrypted;
	cmd += " | ";
	cmd += "openssl enc -d -a -aes256 -pass file:";
	cmd += CLOAK_KEY;
	cmd += " -md SHA512";
	cout << cmd << endl;

	FILE *pipe = popen(cmd.c_str(), "r");
	if (pipe == NULL) return -1;

	char buffer[128];
	string result;
	while(fgets(buffer, sizeof(buffer)-1, pipe) != NULL)
		result += buffer;
	pclose(pipe);

	plain = result;
	return 0;
}

/*
 * Essentially this command:
 * openssl enc -e/-d -a -aes256 -pass file:CLOAK_KEY -md sha512
 */
int
pw_enc_dec(string& plain, string& encrypted, int enc)
{
	static const char magic[] = "Salted__";
	char mbuf[sizeof magic-1];
	unsigned char key[EVP_MAX_KEY_LENGTH],iv[EVP_MAX_IV_LENGTH];
	unsigned char salt[PKCS5_SALT_LEN];
	unsigned char *sptr=NULL;
	BIO *mem=NULL, *b64=NULL, *aes=NULL, *chain=NULL, *pwdbio=NULL;
	const EVP_CIPHER *cipher = EVP_aes_256_cbc();
	const EVP_MD *dgst = EVP_sha512();
	EVP_CIPHER_CTX *ctx = NULL;
	int bytes;
	char pass[1024];
	char *str;
	long size;
	int ret = 0;

	if (enc)
		mem = BIO_new(BIO_s_mem());
	else
		mem = BIO_new_mem_buf((void *)encrypted.c_str(), encrypted.length());
	chain = mem;

	pwdbio = BIO_new_file(CLOAK_KEY, "r");
	BIO_gets(pwdbio, pass, 1024);
	BIO_free_all(pwdbio);
	str = strchr(pass, '\n');
	if (str) *str = 0;
	str = pass;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64,BIO_FLAGS_BASE64_NO_NL);
	chain = BIO_push(b64, chain);

	if (enc) {
		RAND_pseudo_bytes(salt, sizeof salt);
		BIO_write(chain, magic, sizeof magic-1);
		BIO_write(chain, (char *)salt, sizeof salt);
	} else {
		bytes = BIO_read(chain, mbuf, sizeof mbuf);
		bytes = BIO_read(chain, salt, sizeof salt);
		if (memcmp(mbuf, magic, sizeof magic-1)) printf("bad magic number %s", mbuf);
	}
	sptr = salt;

	EVP_BytesToKey(cipher,dgst,sptr, (unsigned char *)str, strlen(str),1,key,iv);

	aes = BIO_new(BIO_f_cipher());
	BIO_get_cipher_ctx(aes, &ctx);
	EVP_CipherInit_ex(ctx, cipher, NULL, NULL, NULL, enc);
	EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, enc);
	chain = BIO_push(aes, chain);

	if (enc) {
		BIO_write(chain, plain.c_str(), plain.length());
		BIO_flush(chain);

		size = BIO_get_mem_data(mem, &str);
		if (str[size-1] == '\n') size--;
		encrypted = string(str, size);
	} else {
		char message[128];
		memset(message, 0, 128);
		stringstream sout;
		while((bytes = BIO_read(chain, message, sizeof(message))) > 0)
			sout << message;
		plain = sout.str();
	}

	if (aes != NULL) BIO_free(aes);
	if (b64 != NULL) BIO_free(b64);
	if (mem != NULL) BIO_free(mem);

	return ret;
}

int
pw_cloak(const string& plain, string& encrypted)
{
	return pw_enc_dec((string &)plain, encrypted, 1);
}

int pw_reveal(string& plain, const string& encrypted)
{
	return pw_enc_dec(plain, (string&)encrypted, 0);
}

int main()
{
	const string orig = "admin";
	string plain, encrypted;

	cout << "*** test 1: pw_cloak -> pw_reveal" << endl;
	cout << "pw_cloak " << orig << endl;
	pw_cloak(orig, encrypted);
	cout << encrypted << endl;
	cout << "pw_reveal " << encrypted << endl;
	pw_reveal(plain, encrypted);
	cout << plain << endl;

	cout << "*** test 2: openssl enc -> pw_reveal" << endl;
	openssl_enc(orig, encrypted);
	cout << encrypted << endl;
	cout << "pw_reveal " << encrypted << endl;
	pw_reveal(plain, encrypted);
	cout << plain << endl;

	cout << "*** test 3: pw_cloak -> openssl enc" << endl;
	cout << "pw_cloak " << orig << endl;
	pw_cloak(orig, encrypted);
	cout << encrypted << endl;
	cout << "openssl dec " << encrypted << endl;
	openssl_dec(plain, encrypted);
	cout << plain << endl;

	return 0;
}

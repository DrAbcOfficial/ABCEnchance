#include <Windows.h>
#include <bcrypt.h>
#include "crypto.h"

#define MODULUS_LEN 258
const char modulus[] = "00e0b509f6259df8642dbc35662901477df22677ec152b5ff68ace615bb7b725152b3ab17a876aea8a5aa76d2e417629ec4ee341f56135fccf695280104e0312ecbda92557c93870114af6c9d05c4f7f0c3685b7a46bee255932575cce10b424d813cfe4875d3e82047b97ddef52741d546b8e289dc6935b3ece0462db0a22b8e7";
const string nonce = "0CoJUm6Qyw8W8jud";
#define PUBKEY_LEN 6
const char pubKey[] = "010001";

string trans(byte ch) {
	auto hex = [](byte i) -> char {
		if (i < 10) {
			return i + '0';
		}
		else {
			return i - 10 + 'a';
		}
		};
	char s[3] = { hex(ch >> 4), hex(ch & 0xF), 0 };
	return s;
}

string hexEncode(const string& s) {
	string ret;
	for (auto c : s) {
		ret += trans(c);
	}
	return ret;
}

string UrlEncode(const string& text) {
	string result;
	for (auto ch : text) {
		if (isalnum(ch) || ch == '.' || ch == '-' || ch == '_' || ch == '~') {
			result += ch;
		}
		else if (ch == ' ') {
			result += '+';
		}
		else {
			result += '%';
			result += trans(ch);
		}
	}
	return result;
}

string Md5Encode(const string& text) {
	BCRYPT_ALG_HANDLE hAlg = NULL;
	BCRYPT_HASH_HANDLE hHash = NULL;

	BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_MD5_ALGORITHM, NULL, 0);
	BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);
	BCryptHashData(hHash, (PUCHAR)text.c_str(), text.size(), 0);

	DWORD dwLen = 0;
	BCryptGetProperty(hHash, BCRYPT_HASH_LENGTH,
		(PUCHAR)&dwLen, sizeof(DWORD), &dwLen, 0);

	std::string hash(dwLen + 1, '\0');
	BCryptFinishHash(hHash, (PUCHAR)hash.c_str(), dwLen, 0);

	BCryptDestroyHash(hHash);
	BCryptCloseAlgorithmProvider(hAlg, 0);
	return hexEncode(hash);
}

string createSecretKey(int size) {
	const static char keys[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const static int keyslen = sizeof(keys) / sizeof(keys[0]);
	string key;
	for (int i = 0; i < size; ++i) {
		key += keys[rand() % keyslen];
	}
	return key;
}

static char base64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

string base64Encode(const string& text) {
	int len = text.length() / 3;
	string rst;
	unsigned data;
	string::const_iterator pos = text.cbegin();
	for (int i = 0; i < len; ++i) {
		data = (byte)*pos++;
		data = (data << 8) | (byte)*pos++;
		data = (data << 8) | (byte)*pos++;
		rst += base64Table[data >> 18];
		rst += base64Table[(data >> 12) & 63];
		rst += base64Table[(data >> 6) & 63];
		rst += base64Table[data & 63];
	}
	switch (text.length() % 3) {
	case 1:
		rst += base64Table[(byte)*pos >> 2];
		rst += base64Table[((byte)*pos << 4) & 63];
		rst += "==";
		break;
	case 2:
		data = (byte)*pos++;
		data = (data << 8) | (byte)*pos++;
		rst += base64Table[data >> 10];
		rst += base64Table[(data >> 4) & 63];
		rst += base64Table[(data << 2) & 63];
		rst += "=";
		break;
	}
	return rst;
}

string aesEncrypt(const string& text, const string& secKey) {
	const BYTE iv[] = "0102030405060708";
	BCRYPT_ALG_HANDLE hAlg = NULL;
	BCRYPT_KEY_HANDLE hKey = NULL;

	BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
	BCryptGenerateSymmetricKey(hAlg, &hKey, NULL, 0,
		(PUCHAR)secKey.c_str(), secKey.size(), 0);
	BCryptSetProperty(hKey, BCRYPT_CHAINING_MODE,
		(PUCHAR)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);

	DWORD dwLen = text.length();
#define AES_BLOCK_SIZE 16
	DWORD dwBufLen = dwLen + AES_BLOCK_SIZE;
	LPBYTE pbBuffer = new BYTE[dwBufLen];
	memset(pbBuffer, 0x00, dwBufLen);
	memcpy(pbBuffer, text.c_str(), dwLen);

	BCryptEncrypt(hKey, pbBuffer, dwLen, NULL,
		(PUCHAR)iv, AES_BLOCK_SIZE, pbBuffer, dwBufLen,
		&dwLen, 0);

	std::string cipher((char*)pbBuffer, dwLen);

	delete[] pbBuffer;
	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlg, 0);
	return base64Encode(cipher);
}

string zfill(const string& str, size_t size) {
	if (str.length() < size) {
		return string(size - str.length(), '0') + str;
	}
	else {
		return str;
	}
}

struct NEWBCRYPT_RSAKEY_BLOB : BCRYPT_RSAKEY_BLOB {
	BYTE PublicExponent[PUBKEY_LEN];
	BYTE Modulus[MODULUS_LEN];
};
string rsaEncrypt(const string& text) {
	BCRYPT_ALG_HANDLE hAlg;
	BCRYPT_KEY_HANDLE hKey;
	DWORD dwDataLen;
	DWORD dwBufferLen;
	PBYTE pbBuffer = NULL;
	PBYTE pbData = NULL;
	BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_RSA_ALGORITHM, NULL, 0);
	NEWBCRYPT_RSAKEY_BLOB blob;
	blob.Magic = BCRYPT_RSAPUBLIC_MAGIC;
	blob.BitLength = MODULUS_LEN * 8;
	blob.cbPublicExp = PUBKEY_LEN;
	blob.cbModulus = MODULUS_LEN;
	blob.cbPrime1 = 0;
	blob.cbPrime2 = 0;
	memcpy(blob.PublicExponent, pubKey, PUBKEY_LEN);
	memcpy(blob.Modulus, modulus, MODULUS_LEN);
	BCryptImportKeyPair(hAlg, NULL, BCRYPT_RSAPUBLIC_BLOB, &hKey, (PUCHAR)&blob, sizeof(blob), 0);
	dwDataLen = text.size();
	BCryptEncrypt(hKey, (PUCHAR)text.c_str(), dwDataLen, NULL, NULL, 0, NULL, 0, &dwBufferLen, BCRYPT_PAD_PKCS1);
	pbBuffer = (PBYTE)malloc(dwBufferLen);
	pbData = (PBYTE)malloc(dwDataLen);
	memcpy(pbData, text.c_str(), dwDataLen);
	BCryptEncrypt(hKey, pbData, dwDataLen, NULL, NULL, 0, pbBuffer, dwBufferLen, &dwBufferLen, BCRYPT_PAD_PKCS1);
	string cipher = reinterpret_cast<char*>(pbBuffer);
	BCryptDestroyKey(hKey);
	BCryptCloseAlgorithmProvider(hAlg, 0);

	free(pbBuffer);
	free(pbData);

	return zfill(base64Encode(cipher), 255);
}

void Encrypt(const string& text, string& encText, string& encSecKey) {
	string secKey = createSecretKey(16);
	encText = aesEncrypt(aesEncrypt(text, nonce), secKey);
	encSecKey = rsaEncrypt(secKey);
}
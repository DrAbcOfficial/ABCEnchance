#ifndef __CRYPTO_H_
#define __CRYPTO_H_

#include <string>
using std::string;

void Encrypt(const string& text, string& encText, string& encSecKey);
string Md5Encode(const string& text);
string UrlEncode(const string& text);

#endif
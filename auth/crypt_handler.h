#ifndef CRYPT_HANDLER_H
#define CRYPT_HANDLER_H

#pragma once

#include <random>
#include <unordered_map>

#include "aes.h"
#include "cryptlib.h"
#include "modes.h"
#include "filters.h"
#include "singleton.h"
#include "structures/session_info.h"

using namespace std;
using byte = uint8_t;
using param_type = unordered_map<string, string>;
using session_type = unordered_map<string, SESSION_INFO>;

using aesEnc = CryptoPP::AES::Encryption;
using aesDec = CryptoPP::AES::Decryption;

using cbcEnc = CryptoPP::CBC_Mode_ExternalCipher::Encryption;
using cbcDec = CryptoPP::CBC_Mode_ExternalCipher::Decryption;
using stFilter = CryptoPP::StreamTransformationFilter;
using SSink = CryptoPP::StringSink;

class CRYPT_HANDLER : public Singleton<CRYPT_HANDLER>
{
private:
    byte key[CryptoPP::AES::MAX_KEYLENGTH] = {0};
    byte iv[CryptoPP::AES::BLOCKSIZE] = {0};

public:
    CRYPT_HANDLER(){}

    bool static IsParenthesesOrDash(char c)
    {
        switch(c)
        {
            case '!':
            case '#':
            case '$':
            case '%':
            case '^':
            case '&':
            case '*':
            case '(':
            case ')':
            case '+':
            case '\'':
            case '\"':
            case ';':
            case '?':
            case '<':
            case '>':
            case ',':
            case '.':
            case '/':
                return true;
            default:
                return false;
        }
    }

    const string decrypt(const string& session)
    {
        const string& cipher = hex2stream(session);
        string dec;
        aesDec aes_dec(key, CryptoPP::AES::MAX_KEYLENGTH);
        cbcDec cbc_dec(aes_dec, iv);
        stFilter decryptor(cbc_dec, new SSink(dec));
        decryptor.Put(reinterpret_cast<const unsigned char*>(cipher.c_str()), cipher.size());
        decryptor.MessageEnd();
        return dec;
    }

    const param_type get_login_info(const string& body)
    {
        param_type map;

        istringstream iss(body);
        string keyval, key, val;
        while(getline(iss, keyval, '&'))
        {
            istringstream iss(keyval);
            if(getline(getline(iss, key, '='), val))
            {
                key.erase(std::remove_if(key.begin(), key.end(), IsParenthesesOrDash), key.end());
                boost::replace_all(val, "+", " ");
                val.erase(std::remove_if(val.begin(), val.end(), IsParenthesesOrDash), val.end());
                map.emplace(key, val);
            }
            else
            {
                map.emplace(key, "");
            }
        }
        return map;
    }

    const string create_session(const string& id)
    {
        string plain_session = create_cookie();
        plain_session.append("|");
        plain_session.append(id);
        plain_session.append(";");
        return stream2hex(crypt(plain_session));
    }

//    const string get_session(const string& id)
//    {
//        if(session.find(id) == session.end()) return "";
//        return session.at(id).get_session_value();
//    }

//    bool find_user(const string& id)
//    {
//        if (session.find(id) == session.end())
//        {
//            return false;
//        }
//        return true;
//    }

//    void remove_session(const string& id)
//    {
//        session.erase(id);
//    }

//    double get_session_sec(const string& id)
//    {
//        if (session.find(id) == session.end()) return 0;
//        return session.at(id).get_seconds();
//    }


private:

//    const string get_session_time(const string& key)
//    {
//        if (session.find(key) == session.end()) return "";
//        return session.at(key).get_time();
//    }

    const string create_cookie()
    {
        const string& default_chars = "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        mt19937_64 gen { std::random_device()() };
        uniform_int_distribution<size_t> dist { 0, default_chars.length() -1 };
        string ret;
        generate_n(std::back_inserter(ret), 16, [&] { return default_chars[dist(gen)]; });
        return ret;
    }

    const string hex2stream(const std::string hexstr)
    {
        string str;
        str.resize((hexstr.size() + 1) / 2);

        for (size_t i = 0, j = 0; i < str.size(); i++, j++)
        {
            str[i] = (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) << 4, j++;
            str[i] |= (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) & 0xF;
        }
        return str;
    }

    const string stream2hex(const std::string str, bool capital = false)
    {
        string hexstr;
        hexstr.resize(str.size() * 2);
        const size_t a = capital ? 'A' - 1 : 'a' - 1;

        for (size_t i = 0, c = str[0] & 0xFF; i < hexstr.size(); c = str[i / 2] & 0xFF)
        {
            hexstr[i++] = c > 0x9F ? (c / 16 - 9) | a : c / 16 | '0';
            hexstr[i++] = (c & 0xF) > 9 ? (c % 16 - 9) | a : c % 16 | '0';
        }
        return hexstr;
    }

    const string crypt(const string& plain_text)
    {
        string cipher;
        aesEnc aes_enc(key, CryptoPP::AES::MAX_KEYLENGTH);
        cbcEnc cbc_enc(aes_enc, iv);
        stFilter encryptor(cbc_enc, new SSink(cipher));
        encryptor.Put(reinterpret_cast<const unsigned char*>(plain_text.c_str() ), plain_text.length() + 1);
        encryptor.MessageEnd();
        return cipher;
    }

//    double get_time_sec()
//    {
//        using Clock = std::chrono::high_resolution_clock;
//        return std::chrono::duration<double>(Clock::now().time_since_epoch()).count();
//    }

//    const string get_time()
//    {
//        std::time_t rawtime;
//        std::time(&rawtime);
//        std::tm* timeinfo = std::localtime(&rawtime);

//        char buffer[80] = {0};
//        std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
//        return string(buffer);
//    }

};

#endif // CRYPT_HANDLER_H

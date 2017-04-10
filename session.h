#ifndef SESSION_H
#define SESSION_H

#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <random>

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

using namespace std;

namespace SECURITY {

    bool IsParenthesesOrDash(char c)
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

    namespace Auth {

        byte key[CryptoPP::AES::MAX_KEYLENGTH] = {0};
        byte iv[CryptoPP::AES::BLOCKSIZE] = {0};

        struct session_info
        {
            string session_id;
            string session_value;
            string time;
            double seconds;
        };

        unordered_map<string, session_info> session;

        double get_current_time_seconds()
        {
            using Clock = std::chrono::high_resolution_clock;
            return std::chrono::duration<double>(Clock::now().time_since_epoch()).count();
        }

        const string get_current_time()
        {
            std::time_t rawtime;
            std::time(&rawtime);
            std::tm* timeinfo = std::localtime(&rawtime);

            char buffer[80] = {0};
            std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
            return string(buffer);
        }

        bool find_user(const string& id)
        {
            if (session.find(id) == session.end())
            {
                return false;
            }
            return true;
        }

        void save_session(const string& id, const string& key, const string& value)
        {
            session.emplace(id, session_info{ key, value, get_current_time(), get_current_time_seconds() });
        }

        void remove_session(const string& id)
        {
            //const auto it = session.find(key);
            session.erase(id);
        }

        const string get_session(const string& id)
        {
            if (session.find(id) == session.end())
            {
                return "";
            }

            return session.at(id).session_value;
        }

        const string get_session_time(const string& key)
        {
            if (session.find(key) == session.end())
            {
                return "";
            }

            return session.at(key).time;
        }

        double get_session_second(const string& key)
        {
            if (session.find(key) == session.end())
            {
                return 0;
            }

            return session.at(key).seconds;
        }

        const unordered_map<string, string> get_login_info(const string& body)
        {
            unordered_map<string, string> map;

            istringstream iss(body);
            string keyval, key, val;
            while(getline(iss, keyval, '&'))
            {
                istringstream iss(keyval);
                if(getline(getline(iss, key, '='), val))
                {
                    key.erase(std::remove_if(key.begin(), key.end(), &IsParenthesesOrDash), key.end());
                    boost::replace_all(val, "+", " ");
                    val.erase(std::remove_if(val.begin(), val.end(), &IsParenthesesOrDash), val.end());
                    map.emplace(key, val);
                }
                else
                {
                    map.emplace(key, "");
                }
            }
            return map;
        }

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

        void hex2byte(const char *in, uint len, byte *out)
        {
             for (uint i = 0; i < len; i+=2)
             {
                   char c0 = in[i+0];
                   char c1 = in[i+1];
                   byte c = ( ((c0 & 0x40 ? (c0 & 0x20 ? c0-0x57 : c0-0x37) : c0-0x30)<<4) |
                            ((c1 & 0x40 ? (c1 & 0x20 ? c1-0x57 : c1-0x37) : c1-0x30)) );
                   out[i/2] = c;
             }
        }

        const string crypt(const string plain_text)
        {
            //char* rawKey="f4150d4a1ac5708c29e437749045a39a";
            //hex2byte(rawKey, strlen(rawKey), key);

            //char* rawIv="86afc43868fea6abd40fbf6d5ed50905";
            //hex2byte(rawIv, strlen(rawIv), iv);

            string cipher_text;
            try
            {
                CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::MAX_KEYLENGTH);
                CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);
                CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(cipher_text));
                stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plain_text.c_str() ), plain_text.length() + 1);
                stfEncryptor.MessageEnd();
            }
            catch(const CryptoPP::Exception& e)
            {
                cerr << e.what() << endl;
            }
            return cipher_text;
        }

        const string decrypt(const string& cipher_text)
        {
            string decrypted_text;
            try
            {
                CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::MAX_KEYLENGTH);
                CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
                CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decrypted_text));
                stfDecryptor.Put(reinterpret_cast<const unsigned char*>(cipher_text.c_str()), cipher_text.size());
                stfDecryptor.MessageEnd();
            }
            catch(const CryptoPP::Exception& e)
            {
                cerr << e.what() << endl;
            }

            return decrypted_text;
        }

        const string create_session(const string& id)
        {
            const string& cookie = create_cookie();
            string plain_session = cookie;
            plain_session.append("|");
            plain_session.append(id);
            plain_session.append(";");
            return stream2hex(crypt(plain_session));
        }

        const string decrypt_session(const string& _session)
        {
            return decrypt(hex2stream(_session));
        }

        bool is_valid_user(const string& id, const string& password)
        {

            if (id.compare("admin") != 0 || password.compare("admin") != 0)
            {
                return false;
            }
            return true;
        }

    }
}

#endif // SESSION_H

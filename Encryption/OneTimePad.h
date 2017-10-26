#ifndef ONETIMEPAD_H
#define ONETIMEPAD_H
#include <iostream>
#include <exception>
#include <sstream>
#include <memory>

#include <fstream>

#include "../make_unique.h"

#include "../Random.h"

typedef std::vector<char> OneTimePad;

class OTPEngine{
    public:

        /** Constructor
         * min: minimum number RNG can generate
         * max: maximum number RNG can generate
        **/
        OTPEngine(unsigned int min = 0,unsigned int max = 100);

        /** Generate new OneTimePad
         * length: length of OTP
        **/
        void reroll(int length);

        /** Decode text based on current OTP
         * str: text to decode
        **/
        std::string decode(const std::string &str);

        /** Encode text based on current OTP
         * str: text to encode
        **/
        std::string encode(const std::string &str);

        /** Encode file based on current OTP
         * str: path of file to encode
        **/
        int encodeFile(const std::string &path);

        /** Encode file based on current OTP
         * str: path of file to decode
        **/
        int decodeFile(const std::string &path);

        /** Return _string object **/
        const std::string &getString();

        /** Return current OTP **/
        const std::vector<char> &getCurrentValues();

        /** Save current pad to _pads map
         * name: name to save pad to
        **/
        int saveToCache(const std::string &name);

        /** Load current pad from _pads map
         * name: name from which to get pad
        **/
        int loadFromCache(const std::string &str);

        virtual ~OTPEngine();
    private:

        /** Update pad string to contain current OTP characters **/
        void updateString();

    private:

        std::shared_ptr<OneTimePad> _current;

        /** String that represents a OTP's values/characters in a readable format **/
        std::string _string = "";

        /** RNG to generate OTP values **/
        std::unique_ptr<Random<int>> _randomEngine;

        /** StringStream to convert integers to strings **/
        std::stringstream _convert;

        /** ofstream and ifstream for file decoding/encoding **/
        std::unique_ptr<std::ofstream> _write;
        std::unique_ptr<std::ifstream> _read;

        /** Cache to store saved OTPs **/
        std::map<std::string,std::shared_ptr<OneTimePad>> _pads;
};

#endif // ONETIMEPAD_H

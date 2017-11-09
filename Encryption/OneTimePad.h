#ifndef ONETIMEPAD_H
#define ONETIMEPAD_H
#include <iostream>
#include <iomanip>
#include <limits>

#include <exception>
#include <sstream>
#include <memory>

#include <fstream>

#include "../make_unique.h"
#include "../FileSystem.h"

#include "../Random.h"

typedef std::vector<int> OneTimePad;

class OTPEngine{
    public:

        /** Constructor
         * min: minimum number RNG can generate
         * max: maximum number RNG can generate
        **/
        OTPEngine(unsigned int min = 0,unsigned int max = 255);

        /** Generate new OneTimePad
         * length: length of OTP
        **/
        void reroll(int length);

        int rerollForFile(const std::string &path);

        /** decrypt text based on current OTP
         * str: text to decrypt
        **/
        std::string decrypt(const std::string &str);

        /** encrypt text based on current OTP
         * str: text to encrypt
        **/
        std::string encrypt(const std::string &str);

        /** encrypt file based on current OTP
         * str: path of file to encrypt
        **/
        int encryptFile(const std::string &path);

        /** encrypt file based on current OTP
         * str: path of file to decrypt
        **/
        int decryptFile(const std::string &path);


        int import(const std::string &content);

        int importFromFile(const std::string &path);

        int exportToFile(const std::string &path,bool overwrite = false);

        /** Return _string object **/
        const std::string &getString();

        /** Return current OTP **/
        const OneTimePad &getCurrentValues();

        /** Save current pad to _pads map
         * name: name to save pad to
        **/
        int saveToCache(const std::string &name);

        /** Load current pad from _pads map
         * name: name from which to get pad
        **/
        int loadFromCache(const std::string &str);

        virtual ~OTPEngine();
    protected:

        /** Update pad string to contain current OTP characters **/
        void updateString();

    protected:

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



class BasicOTPEngine : public OTPEngine{
    public:
        std::string decrypt(const std::string &str);
        std::string encrypt(const std::string &str);

        /** encrypt file based on current OTP
         * str: path of file to encrypt
        **/
        int encryptFile(const std::string &path);

        /** encrypt file based on current OTP
         * str: path of file to decrypt
        **/
        int decryptFile(const std::string &path);
};
#endif // ONETIMEPAD_H

#include "OneTimePad.h"

OTPEngine::OTPEngine(unsigned int min,unsigned int max){
    _randomEngine = make_unique<Random<int>>(min,max); // Initialize RNG
    _write = make_unique<std::ofstream>(); //Initialize ofstream
    _read = make_unique<std::ifstream>(); //Initialize ifstream
    _current = std::make_shared<OneTimePad>(); // Set the current pad to a new OneTimePad object
}

/** Generate a new OTP **/
void OTPEngine::reroll(int length){
    _current->clear(); // Erase current OneTimePad object

    for(int c = 0;c < length;++c){ // Do the following {length} times
        int currentValue = _randomEngine->next(); // Generate a random number,
        _current->push_back(currentValue); // and add it to the current OTP vector.
    }

    updateString(); // Update the _string object
}

/** Decode text via current OTP **/
std::string OTPEngine::decode(const std::string &str){

    std::string finalString = str; // Pull str (text to decode) object into finalString

    for(int i = 0;i < str.size();++i){ // Loop through the string
        finalString[i] = (char)str[i] - (*_current)[i]; // Subtract OTP values from text values
    }

    return finalString; // Return decoded text
}

/** Encode text based on current OTP **/
std::string OTPEngine::encode(const std::string &str){

    std::string finalString = str; //Copy str (text to decode) into finalString object, for later encoding

    for(int i = 0;i < str.size();++i){
        finalString[i] = (char)str[i] + (*_current)[i]; // Add (i) OTP value to the (i)th char, resulting in a new char
    }

    return finalString; // Return final encoded string
}

/** Encrypt file based on current OTP **/
int OTPEngine::encodeFile(const std::string &path){
    _read->open(path,std::ios::binary); // Open file for writing.

    if(!*_read){ //If opening the file failed,
        throw std::runtime_error("Could not open file \"" + path + "\" to encode."); //throw an exception.
    }

    std::string finalContents = ""; // String to store final (encrypted) contents of file.
    char currentChar; // Character to store current character being read by file stream.

    while(_read->get(currentChar)){ // While there are still characters of the file to read...
        finalContents += (char)(currentChar + (*_current)[_read->tellg()]); // Encrypt the current character and add it to the final string
    }

    int fileSize = _read->tellg(); // Get size of file in bytes.

    //_read->seekg(0,std::ios::cur);
    int finalCharAmount = _read->tellg(); // Variable to store the final amount of encrypted characters.

    _read->close(); // Close file-reading

    /* Writing new contents to file */
    _write->open(path); // Open file for writing (Consequentially erasing the file)
    *_write << finalContents; // Fill the empty file with the new contents
    _write->close(); // Close file-writing
    return 0;
}

/** Decrypt file based on current OTP **/
int OTPEngine::decodeFile(const std::string &path){
    _read->open(path,std::ios::binary); //Open file to decode.

    if(!*_read){ //If opening the file failed,
        throw std::invalid_argument("Could not open file \"" + path + "\" to decode."); //throw an exception.
    }

    std::string newContent = ""; // Variable to store new file content.
    char currentChar; // Character to store current character being read by file stream.
    unsigned int currentIndex = 0; // Current character in file.

    while(_read->get(currentChar)){ // While there's more of the file to read,
        newContent += (char)((int)currentChar - (*_current)[_read->tellg()]); // Decode current character and add it to final string
    }
    _read->close(); // Close file-reading

    _write->open(path); // Open file for writing
    *_write << newContent; // Write final string to file
    _write->close(); // Close file-writing
    return 0;
}

/** Save pad to _pads map **/
int OTPEngine::saveToCache(const std::string &name){

    if(_current != nullptr){ // If the current OTP object is valid
        _pads.insert(std::make_pair(name,std::make_shared<OneTimePad>(*_current))); // Save it to _pads map, tied (name) variable
        return 0;
    }
    return 1;
}

/** Load pad from _pads map **/
int OTPEngine::loadFromCache(const std::string &name){
    if(_pads.find(name) != _pads.end()){ // If a pad matching {name} exists in _pads map
        _current = _pads[name]; // Set it as the current map,
        updateString(); // and update the pad string
        return 0;
    }
        return 1;
}

/** Update _string object in OTPEngine **/
void OTPEngine::updateString(){

    OneTimePad::iterator iter; // Create new iterator

    _string.clear(); // Erase string

    for(iter = _current->begin();iter != _current->end();++iter){ // Loop through all char values in current OTP,
        _convert << +(*iter); // Convert values to integer (corresponding to value of char), and then add them to stringstream to be printable
        _string += (_convert.str() + " "); // Add current char to _string object, along with a space as a separator
        _convert.str(std::string()); // Clear stream of current string
    }
}

/** Returns string of characters in current pad **/
const std::string &OTPEngine::getString(){
    return _string;
}

const std::vector<char> &OTPEngine::getCurrentValues(){
    return *_current;
}

OTPEngine::~OTPEngine(){

}

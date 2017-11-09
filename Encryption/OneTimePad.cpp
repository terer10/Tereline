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

int OTPEngine::rerollForFile(const std::string &path){
    int size = 0;

    _read->open(path,std::ios::binary); // Open file for reading

    if(!*_read || !_read->is_open() || _read->bad()){
        throw std::runtime_error("Cannot open " + path);
    }
    _read->seekg(0,std::ios::end); // Size
    size = _read->tellg();

    _read->close(); // Close file

    reroll(size);
    return size;
}

std::string OTPEngine::encrypt(const std::string &text){
    std::string finalText = text; // Feed finalText variable plain text variable

    for(int c = 0;c <= text.size();++c){ // Loop through text string
        finalText[c] = (int)((finalText[c]) ^ ((*_current)[c] )); // and encrypt every char
    }

    updateString(); // Update _string variable
    return finalText;
}

std::string OTPEngine::decrypt(const std::string &text){
    std::string finalText = text; // Feed finalText variable plain text variable

    for(int c = 0;c <= text.size();++c){ // Loop through text string
        finalText[c] = (int)(finalText[c]) ^ ((*_current)[c] % 255); // and encrypt every char
    }
    updateString(); // Update the _string variable
    return finalText;
}

/** Encrypt file based on current OTP **/
int OTPEngine::encryptFile(const std::string &path){
    std::stringstream stream;

    _read->open(path,std::ios::binary); // Open file for writing.

    if(!*_read){ //If opening the file failed,
        throw std::runtime_error("Could not open file \"" + path + "\" to encrypt."); //throw an exception.
    }

    std::string finalContents = ""; // String to store final (encrypted) contents of file.
    char currentChar; // Character to store current character being read by file stream.

    unsigned int currentIndex = 0; // Current position in file (Used to get the right index of char in _current vector)

    while(_read->get(currentChar)){ // While there are still characters of the file to read...
        char curchar = static_cast<char>(static_cast<int>(currentChar) ^ (((*_current)[currentIndex])% 255)); // Encrypt the current character and add it to the final string

        stream << curchar; // Output current char to stream

        ++currentIndex; // Update index
    }

    _read->close(); // Close file-reading

    /* Writing new contents to file */
    _write->open(path,std::ios::binary); // Open file for writing (Consequentially erasing the file)
    *_write << stream.rdbuf(); // Fill the empty file with the new contents
    _write->close(); // Close file-writing
    return 0;
}

/** Decrypt file based on current OTP **/
int OTPEngine::decryptFile(const std::string &path){
    std::stringstream stream;

    _read->open(path,std::ios::binary); //Open file to decrypt.

    if(!*_read){ //If opening the file failed,
        throw std::invalid_argument("Could not open file \"" + path + "\" to decrypt."); //throw an exception.
    }

    std::string finalContent = ""; // Variable to store new file content.
    char currentChar; // Character to store current character being read by file stream.
    unsigned int currentIndex = 0; // Current index/position in file (For _current vector)

    while(_read->get(currentChar)){ // While there's more of the file to read,
        char curchar = static_cast<char>(static_cast<int>(currentChar) ^ (((*_current)[currentIndex]) % 255)); // decrypt current character and add it to final string
        stream << curchar; // Add chracter to stringstream
        ++currentIndex; // Update currentIndex
    }
    _read->close(); // Close file-reading

    _write->open(path,std::ios::binary); // Open file for writing
    *_write << stream.rdbuf(); // Write final string to file
    _write->close(); // Close file-writing
    return 0;
}

int OTPEngine::import(const std::string &content){
    _current->clear();
    OneTimePad::iterator iter;
    int currentInt;
    std::stringstream stream;
    stream.str(content);
    while(stream >> currentInt){
        _current->push_back(currentInt);
    }
}

int OTPEngine::importFromFile(const std::string &path){
    _read->open(path); // Open file for reading

    if(!*_read){ // Throw an error if the file couldn't be opened
        throw std::runtime_error("Could not open file \"" + path + "\" to import."); //throw an exception.
    }

    int currentValue; // Variable to store the current integer when searching through file
    OneTimePad temp; // Temporary OneTimePad in order to store values taken from file

    while(*_read >> currentValue){ // Suck in values
        temp.push_back(currentValue); // and put them into the temp OneTimePad
    }

    if(!_read->eof()){ // If the whole file was not read (Meaning, a non-int character was reached)
        _read->close(); // close the file,
        throw std::runtime_error("Did not reach end of file for importing! (Possible non-integer character in OTP file.)"); // and throw an exception
    }

    _current = std::make_shared<OneTimePad>(temp); // Assign the temporary OTP variable, to the _current variable
    updateString(); // Update the OTP-String

    _read->close(); //Close the file
}

int OTPEngine::exportToFile(const std::string &path,bool overwrite){
    if(!overwrite){ //If not authorized to overwrite files,
        if(File::exists(path)){ // if the file already exists,
            throw std::runtime_error("File already exists; overwrite mode is false."); // and throw an exception because of the file's existence.
        }
    }

    _write->open(path); // Open file for writing

    if(!*_write){
        throw std::runtime_error("Could not open/create file."); // Throw an exception if the file couldn't be opened
    }

    OneTimePad::iterator iter;
    for(iter = _current->begin();iter != _current->end();++iter){
        *_write << (*iter) << " ";
    }

    _write->close(); // Close file
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

const OneTimePad &OTPEngine::getCurrentValues(){
    return *_current;
}

OTPEngine::~OTPEngine(){

}


/** decrypt text via current OTP **/
std::string BasicOTPEngine::decrypt(const std::string &str){

    std::string finalString = str; // Pull str (text to decrypt) object into finalString

    for(unsigned int i = 0;i < str.size();++i){ // Loop through the string
        finalString[i] = (char)str[i] - (*_current)[i]; // Subtract OTP values from text values
    }

    return finalString; // Return decrypted text
}

/** encrypt text based on current OTP **/
std::string BasicOTPEngine::encrypt(const std::string &str){

    std::string finalString = str; //Copy str (text to decrypt) into finalString object, for later encoding

    for(unsigned int i = 0;i < str.size();++i){
        finalString[i] = (char)str[i] + (*_current)[i]; // Add (i) OTP value to the (i)th char, resulting in a new char
    }

    return finalString; // Return final encrypted string
}


/** Encrypt file based on current OTP **/
int BasicOTPEngine::encryptFile(const std::string &path){
    _read->open(path,std::ios::binary); // Open file for writing.

    if(!*_read){ //If opening the file failed,
        throw std::runtime_error("Could not open file \"" + path + "\" to encrypt."); //throw an exception.
    }

    std::string finalContents = ""; // String to store final (encrypted) contents of file.
    char currentChar; // Character to store current character being read by file stream.

    while(_read->get(currentChar)){ // While there are still characters of the file to read...
        finalContents += (char)(currentChar + (*_current)[_read->tellg()]); // Encrypt the current character and add it to the final string
    }

    _read->close(); // Close file-reading

    /* Writing new contents to file */
    _write->open(path,std::ios::binary); // Open file for writing (Consequentially erasing the file)
    *_write << finalContents; // Fill the empty file with the new contents
    _write->close(); // Close file-writing
    return 0;
}

/** Decrypt file based on current OTP **/
int BasicOTPEngine::decryptFile(const std::string &path){
    _read->open(path,std::ios::binary); //Open file to decrypt.

    if(!*_read){ //If opening the file failed,
        throw std::invalid_argument("Could not open file \"" + path + "\" to decrypt."); //throw an exception.
    }

    std::string newContent = ""; // Variable to store new file content.
    char currentChar; // Character to store current character being read by file stream.

    while(_read->get(currentChar)){ // While there's more of the file to read,
        newContent += (char)((int)currentChar - (*_current)[_read->tellg()]); // decrypt current character and add it to final string
    }
    _read->close(); // Close file-reading

    _write->open(path,std::ios::binary); // Open file for writing
    *_write << newContent; // Write final string to file
    _write->close(); // Close file-writing
    return 0;
}


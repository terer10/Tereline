#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <iostream>
#include <fstream>

#ifdef _WIN32
    #include <dirent.h>
#endif // _WIN32
#include <sys/stat.h>
#include <sys/types.h>

class File{
    public:
    ///Get name of file from full path
    static std::string getNameFromDirectory(const std::string &dir2){
        std::string name = "";
        for(int i = dir2.find_last_of('/') + 1;i < dir2.length();++i){
            name+=dir2[i];
        }
        return name;
    }

    ///Create a directory
    static void createDirectory(const std::string &str){
        #ifdef _WIN32
            mkdir(str.c_str());
        #else
            mkdir(str.c_str(),0666);
        #endif
    }

    ///Copy a single file from one place to another
    static void copyFile(const std::string &dir1,const std::string &dir2){
        std::ifstream read(dir1);
        std::ofstream write(dir2);
        //Take all the data from the first file and paste it to the last file
        write << read.rdbuf();
        std::cout << "========================" << std::endl;
        std::cout << dir1 << "copied to" << dir2 << std::endl;
        std::cout << "========================\n" << std::endl;
    }

    ///Copy a whole folder from one place to another
    static void copyFolder(const std::string &dir1, const std::string &dir2){
        DIR *dir = new DIR;
        struct dirent *ent = new struct dirent;
        //If the program is actually able to open the directory
        if((dir = opendir(dir1.c_str())) != NULL){
            //While reading a file that is not at the end of the file
            while((ent = readdir(dir)) != NULL){
                //Grab the name of the file and attach it to the end to create
                //a full path for the file to be copied, then copy the file
                copyFile(ent->d_name,dir2 + getNameFromDirectory(ent->d_name));
            }
            //Close directory once it reaches the end
            closedir(dir);
        }
    }

    static bool exists(const std::string &path){
        struct stat buf;
        return (stat(path.c_str(),&buf) == 0);
    }

    static unsigned int getSize(const std::string &path,bool binary = false){
        std::ifstream read;
        if(binary){
            read.open(path,std::ios::binary);
        }else{
            read.open(path);
        }
        if(!read){
            read.close();
            throw std::runtime_error("Invalid filename.");
        }
        read.seekg(0,std::ios::beg);
        unsigned int beg = read.tellg();
        read.seekg(0,std::ios::end);
        unsigned int endd = read.tellg();

        read.close();
        return (endd - beg);
    }
};

#endif // FILESYSTEM_H

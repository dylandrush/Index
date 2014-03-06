/*
 * EECS 3540 Project 2 - Index
 * Author: Dylan Drake Rush
 * Date: February 16, 2014
 *
 */

#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

struct wordEntry
{
    std::string word;
    std::vector<int> location; 
};

int FileSize(std::string fileName);

std::vector<wordEntry> IndexFile(std::string fileName);

int main (int argc, char* argv[])
{
    int threadAmount;
    int sizePerThread;
    int fileSize;
    std::string fileName;
    std::ifstream inFile(argv[2]);
    std::vector<wordEntry> indexedFile;

    if (argc == 3)
    {
        if (isdigit(*argv[1]))
        {
            threadAmount = atoi(argv[1]);
        }
        else
        {
            std::cerr << "Try a valid thread amount..." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (inFile)
        {
            fileName = argv[2];
            inFile.close();
        }
        else
        {
            std::cerr << "Try a vaild file..." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    else
    {        
        std::cerr << "Run the program with the number of desired threads and a "
                  << "file to index...\nFor example: ./Index 4 file.txt"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    fileSize = FileSize(fileName);
    sizePerThread = fileSize/threadAmount;

    std::cout << "The file is " << fileSize << " bytes." << std::endl;
    std::cout << "There will be " << sizePerThread
              << " bytes per thread." << std::endl;

    indexedFile = IndexFile(fileName);    

    for (int i = 0; i < indexedFile.size(); i++)
    {
        std::cout << indexedFile.at(i).word << ":\t";
        for (int j = 0; j < indexedFile.at(i).location.size(); j++)
        {
            std::cout << indexedFile.at(i).location.at(j) << "\t";
        }
        std::cout << std::endl;
    }

}

int FileSize(std::string fileName)
{
    std::ifstream file;
    std::streampos begin, end;
    file.open(fileName.c_str());
    begin = file.tellg();
    file.seekg (0, std::ios::end);
    end = file.tellg();
    file.close();
    return (end-begin);
}

std::vector<wordEntry> IndexFile(std::string fileName)
{
    std::ifstream inFile;
    std::vector<wordEntry> index;
    std::string word;
    char lastChar, firstChar;
    inFile.open(fileName.c_str());
//  TODO limit this loop to the amount of bytes per thread and somewhere around
//       here I need to determin if i am at the beinging of a word or what if i
//       start in the middle of a word.
    int location;
    while (inFile >> word)
    {
        location = (int)inFile.tellg() - (int)word.length();
        int i = 30;
        // This is a really really bad way to do this, but ohh well...
        // Just keep looping through an arbitrary amount of times to make sure
        // that the word has no leading or trailing non alpha characters
        do {
            if (word.length() > 1)
            {
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                firstChar = word[0];
                lastChar = word[word.length() - 1];
               
                if (firstChar < 97) 
                { 
                    word.erase(0, 1);
                }
                if (firstChar > 122)
                {
                    word.erase(0, 1);
                }
    
                if (lastChar < 97) 
                { 
                    word.erase(word.length() - 1);
                }
                if (lastChar > 122)
                {
                    word.erase(word.length() - 1);
                }
            }
            if (word.length() == 1)
            {
                firstChar = word[0];
                if (firstChar < 97)
                {
                    word = "";
                }
                if (firstChar > 122)
                {
                    word = "";
                }
            }
            i--;
        } while (i != 0);

        if (word.length() > 0)
        {
            bool previouslyIndexed = false;
            for (int i = 0; i < index.size(); i++)
            {
                if (index.at(i).word == word)
                {
                    index.at(i).location.push_back(location);
                    previouslyIndexed = true;
                }
            }
       
            if (previouslyIndexed == false)
            {
                index.push_back(wordEntry());
                index.back().word = word;
                index.back().location.push_back(location);
            }
        }
    }
    return index;
}

/***********************************************/
/*                COMPILE NOTES                */
/* g++ Index.cpp -o Index -pthread -std=c++11  */
/***********************************************/

/*
 * EECS 3540 Project 2 - Index
 * Author: Dylan Drake Rush
 * Date: February 16, 2014
 *
 * Description:
 *  This program will index a file.  It will print out all unique words in a 
 *  file in alphabetical order along with the location(s) of the words in 
 *  chronological order.  This program utilizes c++11 std::thread and 
 *  std::mutex.
 *
 * Bugs:
 *  - No checks on thread amount -- causes problems when too many threads
 */

#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

/* Each entry into the index will have a word and a location(s) */
struct wordEntry
{
    std::string word;
    std::vector<int> location; 
};

/* Prototypes */
int FileSize(std::string fileName);
void IndexFile(std::string fileName, int offset, int id);
bool compareWordEntry (wordEntry a, wordEntry b) { return (a.word < b.word); }

/* Globals */
std::vector<wordEntry> index;
std::mutex indexMutex;
int threadAmount;

int main (int argc, char* argv[])
{
    int sizePerThread;
    int fileSize;
    std::string fileName;
    std::ifstream inFile(argv[2]);
    
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

/*    std::cout << "The file is " << fileSize << " bytes." << std::endl;
      std::cout << "There will be " << sizePerThread
                << " bytes per thread." << std::endl;
*/

    std::thread threadID[threadAmount];
    
    // Start making threads!
    for (int i = 0; i < threadAmount; ++i)
    {
        threadID[i] = std::thread(IndexFile, fileName, sizePerThread, i);
    }

    // Wait for all of the threads to finish
    for (int i = 0; i < threadAmount; ++i)
    {
        threadID[i].join();
    }
    
    std::sort(index.begin(), index.end(), compareWordEntry);
    for (int i = 0; i < index.size(); i++)
    {
        std::sort(index.at(i).location.begin(), index.at(i).location.end());
    }

    for (int i = 0; i < index.size(); ++i)
    {
        std::cout << index.at(i).word << ":\t";
        if (index.at(i).word.length() < 7)
        {
            std::cout << "\t";
        }
        for (int j = 0; j < index.at(i).location.size(); ++j)
        {
            std::cout << index.at(i).location.at(j) << "\t";
        }
        std::cout << std::endl;
    }
}

/* FileSize returns the size in bytes of a file */ 
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

/* IndexFile will write all of the words, and their byte location in a file
 * into a vector.  It will only index the word once.  If a word shows up a
 * second time, only the position of the word is added into the index.
 * Punctuation, parentheses, etc. are stripped from each end of a word.  
 * Non-words, numbers and punctuation only words will not show up in the index.
 * Contractions and other words with apostrophes embeded in them will show up
 * in the index as unique words.
 */  
void IndexFile(std::string fileName, int offset, int id)
{

    std::ifstream inFile;
    inFile.open(fileName);

    std::string word;
    char testChar;
    char lastChar, firstChar;
    int seekAmount = offset * id;
    int searchLimit = (id + 1) * offset;
    int location;

    if ((id + 1) == threadAmount)
    {
        inFile.seekg(0, inFile.end);
        searchLimit = (int)inFile.tellg();
    }

    inFile.seekg(0, inFile.beg);
    inFile.seekg(seekAmount);
    
    // If we are not the first thread, we might be in the middle of a word.
    if (seekAmount != 0)
    {
        inFile.seekg(0, inFile.beg);
        inFile.seekg((int)seekAmount - 1); // Move back 1 char.
        inFile.get(testChar);
        inFile.unget();
        inFile.seekg(0, inFile.beg);
        inFile.seekg(seekAmount);
        
        // If the previous char is not a space, the first word is garbage.    
        if (testChar != ' '  && testChar != '\n')
        {
            inFile >> word;
        }
    }
    
    while (inFile >> word)
    {
        location = (int)inFile.tellg() - (int)word.length() + 1;
        if (location < searchLimit)
        {
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
                --i;
            } while (i != 0);
            
            // Going to now add the word to the index.  Time to lock things up.
            indexMutex.lock(); 
            if (word.length() > 0)
            {
                bool previouslyIndexed = false;
                for (int i = 0; i < index.size(); ++i)
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
            indexMutex.unlock(); // Word added; unlock mutex.
        }
    }
}

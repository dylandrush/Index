/*
 * EECS 3540 Project 2 - Index
 * Author: Dylan Drake Rush
 * Date: February 16, 2014
 *
 */

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int FileSize(std::string fileName);

int main (int argc, char* argv[])
{
    int threadAmount;
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
    fileSize = FileSize(fileName);l
    sizePerThread = fileSize/threadAmount;

    std::cout << "The file is " << fileSize << " bytes." << std::endl;
    std::cout << "There will be " << sizePerThread << " bytes per thread." << std::endl;
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
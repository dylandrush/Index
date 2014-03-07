Index
=====

Creates an index of all the words in a document.
Poorly makes use of multithreading.

Compile with g++ Index.cpp -o Index -pthread -std=c++11

Run with ./Index [number_of_threads] [name_of_file]

/*
    homework 9

    hash.h 
    this file contains all the declarations of the functions that will be implemented
    in the hash.cpp file. This includes fuctions for both the HashKey class and the 
    HashTable class. These functions are all described in the hash.cpp file
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#ifndef HASH_H
#define HASH_H

class HashKey {

    public:
        HashKey(std::string key_, int value);

        std::string getKey();
        void addValue(int val);
        std::vector<int> getValues();


    private:
        std::string key;    //the kmer
        std::vector<int> values;    //a vector of the positions in the genome sequence 
                                    //where the kmer is found
};


class HashTable {
    public:
        HashTable(int t_size, float occupancy_);
        std::vector<int> find(std::string key);
        void insert(std::string key, int value);
        ~HashTable();

        bool check_occupancy();
        void resize();

        int hash_function(std::string kmer);
    
    private:
        HashKey** table;    //the table to hold the HashKeys
        int table_size;     //the size of the table (or minimum size)  
        int key_entries;    //number of unique key entries in the table
        float occupancy;    //the ratio that tells us when we need to resize the table
        std::vector<int> values;    //the vector of values
};
#endif
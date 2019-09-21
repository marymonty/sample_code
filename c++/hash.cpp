/*
    homemwork 9

    hash.cpp

    implements all functions laid out in the hash.h file
    including those for the HashKey class and the HashTable class.
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <math.h>
#include "hash.h"


//
//      Below is an implementation of the HashKey class
//

/*
    constructor for a HashKey

        creates a new HashKey 
        arguments:
            key_: a string representing a k-mer of length k 
            value: an int representing the location of the k-mer in the complete genome sequence
        no return 
*/
HashKey::HashKey(std::string key_, int value) {
    key = key_;
    values.push_back(value);
}


/*
    getKey()

        member function will return the key of a specified HashKey
        no arguments
        returns the key associated with that HashKey
*/
std::string HashKey::getKey() {
    return this->key;
}

/*
    addValue()

        a modifier, will add a value to the vector of values associated with a specific HashKey
        arguments:
            val: an int of the location the k-mer shows up in the genome file
        no return
*/
void HashKey::addValue(int val) {
    this->values.push_back(val);
}


/*
    getValues()

        member function that returns the vector of values of a specified HashKey
        no arguments
        returns the vector of ints of the values (the locations of the k-mer)
*/
std::vector<int> HashKey::getValues() {
    return this->values;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//      Below is an implementation of the HashTable class
//
//

/*
    HashTable constructor

        creates a new HashTable
        argumetns:
            t_size: an int, the table size, the minimum size that the table needs to Below
            occupancy: a float, the maximum ratio that the table can have before it needs to be resized
        no return
*/
HashTable::HashTable(int t_size, float occupancy_) {
    this->table_size = t_size;  //sets table size to given value
    this->key_entries = 0;  //key entries are the number of unique keys in the table, initially set to zero
    this->occupancy = occupancy_;   //sets occupancy to given value
    this->table = new HashKey* [t_size];    //creates a new table of pointers of HashKeys of the table size
    for (int i = 0; i < t_size; i++) {  //fills in the table with NULL values
        this->table[i] = NULL;
    }
}


/*
    find()

        function that searches for a given key in the HashTable
        arguemtns:
            key: a string of the kmer we are searching the table for
        return:
            an std::vector<int> which is the vector of values of positions where the k-mer
            was found in the original genome sequence
        should be an O(1) function
        for more detail look at insert, these two are very similar
*/
std::vector<int> HashTable::find(std::string key) {
    int index = (hash_function(key) % this->table_size);
    std::vector<int> empty_vec;
    while (this->table[index] != NULL && this->table[index]->getKey() != key) {
        index = (index+1) % this->table_size;
    }
    if (this->table[index] != NULL && this->table[index]->getKey() == key) {
        return this->table[index]->getValues();
    }
    else {
        return empty_vec;
    }
}


/*
    insert()

         insert gets an index and attempts to insert the new HashKey into the HashTable
         checks if a resize is neccesary
         arguments:
            key: a string that is the key of the kmer of size indicated
        no return
*/
void HashTable::insert(std::string key, int value) {
    //index is an int of where the key should go in the hash table, this is found by
    //useing the hash function to get an int which is modded by the size of the table 
    //so that the index I am trying to insert the key into is within the table bounds.
    int index = (hash_function(key) % this->table_size);
    //if the index in the table is not NULL, therefore it has a key already in it, 
    //then i check to see if it is the same key, if so I can just add the new position 
    //it was found in the genome text to that key's values vector, however if the key
    //is not the same I linear probed through the table to find the next open index
    while (this->table[index] != NULL && this->table[index]->getKey() != key) {
        index = (index+1) % this->table_size;
    }
    if (this->table[index] != NULL && this->table[index]->getKey() == key) {
        this->table[index]->addValue(value);
    }
    else {
        //if it is a NULL, I can create a new HashKey and insert it into that free index
        this->table[index] = new HashKey(key, value);
        this->key_entries += 1;
    }
    //if the array is over occupied I must resize it 
    if(!(check_occupancy())) {
        resize();
    }
}


/*
    check_occupancy()

        checks if the table is too full
        no arguments
        returns a bool of whether the table has space (true) or if it is over crowded (false)
*/
bool HashTable::check_occupancy() {
    return ((float(this->key_entries)/float(this->table_size)) < this->occupancy);
}


/*
    resize()

        if the table is over crowded I must double the size in this function
        and copy over all the HashKeys from the smaller table into the bigger table
        by reinserting them all into the HashTable
        no arguments
        no returns
*/
void HashTable::resize() {
    int old_size = this->table_size;
    this->table_size *= 2;  //new size is twice as big as the old size
    HashKey** temp_table = this->table;
    this->table = new HashKey*[this->table_size];   //create a new big hash table
    for (int i = 0; i < this->table_size; i++) {    //fills new hash table with NULL
        this->table[i] = NULL;
    }
    this->key_entries = 0;
    for (int index = 0; index < old_size-1; index++) {  //go through the old table
        if (temp_table[index] != NULL) {    //there is a HashKey in this index
            HashKey* temp_key;
            HashKey* new_key = temp_table[index];
            if (new_key->getValues().size() > 1) {  //if the key has multiple values
                                                    //i must go through each value to reinsert it
                for (unsigned int j = 0; j < new_key->getValues().size(); j++) {
                    insert(new_key->getKey(), new_key->getValues()[j]);
                }
                temp_key = new_key;
                new_key = temp_table[index+1];
                delete temp_key;    //delete the hashKey in the old table
            }
            else {  //there was only one value in the values vector so insert that 
                insert(new_key->getKey(), new_key->getValues()[0]);
            }
        }
    }
    delete [] temp_table;   //delete the old table
}



//the deletion function
HashTable::~HashTable() {
    for (int i = 0; i < this->table_size; i++) {
        if (this->table[i] != NULL) {
            delete this->table[i];
        }
    }
    delete [] this->table;
}




/*
    hash_function

        computes a numerical value for the kmer string and returns that value
        arguments:
            kmer: a string of what we are putting into the table
        returns an int representing the numerical value for the kmer
*/
int HashTable::hash_function(std::string kmer) {
    int letter_val;
    int total = 0;
    for (unsigned int i = 0; i < kmer.length(); i++) {
        //assign a value to each letter
        if (kmer[i] == 'A') {
            letter_val = 0;
        }
        else if (kmer[i] == 'C') {
            letter_val = 1;
        }
        else if (kmer[i] == 'G') {
            letter_val = 2;
        }
        else if (kmer[i] == 'T') {
            letter_val = 3;
        }
        //add up all the letter values to create a total for the kmer
        total += (letter_val * (pow(4, i)));
    }
    return total;
}
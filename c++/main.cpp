/*
    homework 9 miniBLAST

    this will read in from an input file
    have a specified list of input commands
    uses hash tables and hash functions to look for queries
    outputs to an output file


    main.cpp
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include "hash.h"



/*
    main method
    will go through the input file to read what commands are being called
*/
int main(int argc, char* argv[]) {

    std::string command;
    std::string filename = "";
    std::string closeFile = "";
    int table_size = 1000;
    float occupancy = 0.5;
    int k = 0;
    std::vector<int> mismatches;
    int m = 0;
    std::vector<std::string> query_vec;
    std::string q = "";
    while (std::cin >> command) {
        /*
            genome filename 
            Read a genome sequence from filename. 
            The genome file consists of lines DNA characters.
        */
        if (command == "genome") {
            std::cin >> filename;
        }
        

        /*
            table_size N
            this is an optional command. 
            N is an integer. 
            It is the initial hash table size. 
            If it does not appear in the command file, 
            the initial table size should be set to 100.
        */
        if (command == "table_size") {
            std::cin >> table_size;
        }


        /*
            occupancy f 
            this is an optional command. 
            f is a float. 
            When the occupancy goes above this level, the table should be resized. 
            If it does not appear in the command file, 
            the initial level should be set to 0.5.
        */
        if (command == "occupancy") {
            std::cin >> occupancy;
        }


        /*
            kmer k 
            k is an integer. 
            The genome should be indexed with kmers of length k
        */
        if (command == "kmer") {
            std::cin >> k;
        }


        /*
            query m query_vec
            Search the genome for a match to query string allowing for m mismatches.
        */
        if (command == "query") {
            std::cin >> m;
            mismatches.push_back(m);
            std::cin >> q;
            query_vec.push_back(q);
        }


        /*
        	write_to closeFile
        	closeFile is a string that is the filename to output to
        */
        if (command == "write_to") {
        	std::cin >> closeFile;
        }


        /*
            quit
            exit the program
        */
        if (command == "quit"){
            break;
        }
    }

    std::string genome_text;
    std::string genome_line;
    std::ifstream genome_file(filename.c_str()); 
    if (!genome_file.good()) {  //bad filename
        std::cerr << "Can't open " << filename << " to read.\n";
        return 1;
    }

    while (genome_file >> genome_line) {    //collecting the text from the file
        genome_text += genome_line;
    }
    HashTable hash_table = HashTable(table_size, occupancy);
    std::string kmer;
    //remember k is the size of the kmer
    for (unsigned int i = 0; i < genome_text.length()-k; i++) {
        kmer = genome_text.substr(i, k);
        hash_table.insert(kmer, i);
    }

    std::ofstream out_str(closeFile.c_str()); 
	if (!out_str.good()) {
	  std::cerr << "Can't open " << closeFile << " to write.\n";
	  exit(1);
	}

    //find and print
    std::vector<int> found_index_vec;
    std::string q_substring = "";
    std::string current_qeury = "";
    for (unsigned int query_index = 0; query_index < query_vec.size(); query_index++) {
        current_qeury = query_vec[query_index];
        std::string k_length_query = current_qeury.substr(0, k);
        out_str << "Query: " << current_qeury << std::endl;

        found_index_vec = hash_table.find(k_length_query);

        if (found_index_vec.size() == 0) {
            out_str << "No Match" << std::endl;
        }
        else {
            bool already_printed = false;
            for (unsigned int index = 0; index < found_index_vec.size(); index++) {
                q_substring = genome_text.substr(found_index_vec[index], current_qeury.length());
                int mismatch_count = 0;
                int allowed_mismatches = mismatches[query_index];   //mismatches between the genome and query string
                bool different = false;
                unsigned int print_count = 0;
                for (unsigned int found_query_index = 0; found_query_index < current_qeury.length(); found_query_index++) {
                    if (current_qeury[found_query_index] != q_substring[found_query_index]) {
                        mismatch_count++;
                    }
                    if (mismatch_count > allowed_mismatches) {
                        different = true;
                        continue;
                    }
                }
                if (!different) {
                    print_count++;
                    out_str << found_index_vec[index] << " " << mismatch_count << " " << q_substring << std::endl;
                    already_printed = true;
                }
                if (different) {
                    if (!already_printed && print_count < found_index_vec.size() && index == found_index_vec.size()-1) {   
                        out_str << "No Match" << std::endl;
                    
                    }                    
                }
            }
        }
    }
    return 0;
}

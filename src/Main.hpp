/**
* Main.hpp
*
* A simple C++ software to retrieve files 
* from the Protein Data Bank.
*
* Written by Olivier Béquignon
* Published under the MIT License
*/
#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>
#include <vector>

int Revision = BUILD_REVISION;

bool mode_set, help_mode, search_mode, get_mode, compressed_files, large_files, mmcif_files, pdbml_files, obsolete_files;
std::vector<std::string> pdb_codes;

enum PdbType { Standard = 0, Large = 1, Obsolete = 2, Unvalid = 3 };
enum PdbDownloadType { PDB = 0, mmCIF = 1, PDBML = 2, PDB_gz = 3, mmCIF_gz = 4, PDBML_gz = 5 };

int main(int argc, char* argv[]);

void parse_arguments(std::vector<std::string> argv);
void show_short_usage(std::string name);
void show_usage(std::string name);

PdbDownloadType get_download_type();
void display_pdb_type(std::vector<std::string> pdb_codes);
void display_information(std::string category_title, std::vector<std::string> pdb_codes, std::string help);

std::vector<PdbType> pdb_type(std::vector<std::string> pdb_codes);
std::vector<bool> pdb_exist(std::vector<std::string> pdb_codes);
std::string get_obsolete_ids();
std::string get_large_ids();

void download_pdb(std::string pdb_code, bool gzipped);
bool download_pdb(std::string pdb_code, PdbType pdbType, PdbDownloadType downloadType, bool gzipped, bool largePdbBundle);
bool file_exists(std::string filename);
void unzip(std::string filename);
#endif

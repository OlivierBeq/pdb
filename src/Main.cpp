#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "HTTPDownloader.hpp"


void show_usage(std::string name){
	std::cout << "\nUsage: " << name << " help | search | get [-c] <pdb_code1> <pdb_code2> ...\n" << std::endl;
	std::cout << " Arguments:" << std::endl;
	std::cout << "    help\tDisplays this help" << std::endl;
	std::cout << "    search\tDetermine if supplied pdb codes are valid" << std::endl;
	std::cout << "    get\t\tDownload pdb files corresponding to supplied pdb codes\n" << std::endl;
	std::cout << "    -c\t\tDownload gzipped pdb files (ignored if 'search' mode)\n" << std::endl;
}

std::string replace_substring(std::string originalString, std::string stringToReplace, std::string replacementString){
	size_t index = 0;
	while (true){
		// Locate the substring to replace.
		index = originalString.find(stringToReplace, index);
		if (index == std::string::npos) break;

		// Make the replacement.
		originalString = originalString.replace(index, stringToReplace.length(), replacementString);

		// Advance index so the next iteration does not pick it up as well.
		index += stringToReplace.length();
	}
	return originalString;
}

std::string remove_char(std::string originalString, std::string charsToRemove){
	for (int i = 0; i < charsToRemove.length(); ++i){
		originalString.erase( std::remove(originalString.begin(), originalString.end(), charsToRemove.at(i)), originalString.end());
	}
	return originalString;
}

std::string to_upper(std::string originalString){
	std::transform(originalString.begin(), originalString.end(), originalString.begin(), ::toupper);

	return originalString;
}

std::string to_lower(std::string originalString){
	std::transform(originalString.begin(), originalString.end(), originalString.begin(), ::tolower);

	return originalString;

}

bool pdb_exists(std::string pdb_code){
	std::string url = "https://www.rcsb.org/pdb/rest/customReport.xml?pdbids=";
	url += pdb_code;
	url += "&customReportColumns=residueCount&format=csv";

	HTTPDownloader downloader;
	std::string content = downloader.download(url);

	content = replace_substring(content, "<br />", "\n");
	content = replace_substring(content, "structureId,residueCount\n", "");
	content = to_upper(content);

	pdb_code = to_upper(pdb_code);

	size_t index = content.find(pdb_code);
	return index < std::string::npos;
}

void download_pdb(std::string pdb_code, bool gzipped){
	std::string filename;
	filename += pdb_code;
	filename += ".pdb";

	if (gzipped){
		filename += ".gz";
	}

	std::string url;
	if (!gzipped){
		url = "https://files.rcsb.org/view/";
		url += filename;
		HTTPDownloader downloader;
		std::string content = downloader.download(url);

		std::ofstream out(filename);
		out << content;
		out.close();
		return;
	}
	else {
		url = "ftp://ftp.rcsb.org/pub/pdb/data/structures/all/pdb/pdb";
		url += to_lower(pdb_code);
		url += ".ent.gz";
		std::string command = "wget -O " + filename + " --quiet " + url;
		system((const char*)command.c_str());
		return;
	}
}

int main(int argc, char* argv[]){
	if (argc < 3){
		show_usage(argv[0]);
		return 1;
	}
	if (argc > 1){
		std::string first_arg;
		first_arg += argv[1];
		first_arg = remove_char(first_arg, "-/");
		if (first_arg == "help" || first_arg == "h" || first_arg == "?"){
			show_usage(argv[0]);
			return 0;
		}
		else{
			if (first_arg == "get"){
				std::string second_arg;
				second_arg += argv[2];
				second_arg = remove_char(second_arg, "-/");
				bool compressed;
				int start_index;
				if (second_arg == "c"){
					compressed = true;
					start_index = 3;
				}
				else{
					compressed = false;
					start_index = 2;
				}
				std::vector<std::string> unvalid;
				std::vector<std::string> valid;
				int downloaded = 0;
				for (int i = start_index; i < argc; ++i){
					bool is_valid = pdb_exists(argv[i]);
					if (is_valid){
						download_pdb(to_upper(argv[i]), compressed);
						valid.push_back(to_upper(argv[i]));
						++downloaded;
					}
					else {
						unvalid.push_back(argv[i]);
					}
				}
				std::cout << "\nDownloaded files: " << downloaded << std::endl;
				for (int i = 0; i < valid.size(); ++i){
					std::cout << valid.at(i) << " ";
					if ((i+1) % 7 == 0) std::cout << "" << std::endl;
				}
				std::cout << "" << std::endl;
				std::cout	<< "\nUnvalid PDB identifiers:\n";
				for (int i = 0; i < unvalid.size(); ++i){
					std::cout << unvalid.at(i) << " ";
					if ((i+1) % 7 == 0) std::cout << "" << std::endl;
				}
				std::cout << "" << std::endl;
			}
			else if (first_arg == "search"){
				std::vector<std::string> valid;
				std::vector<std::string> unvalid;
				for (int i = 2; i < argc; ++i){
					bool res = pdb_exists(argv[i]);
					if (res) valid.push_back(to_upper(argv[i]));
					else unvalid.push_back(to_upper(argv[i]));
				}
				std::cout	<< "\nValid PDB identifiers:\n";
				for (int i = 0; i < valid.size(); ++i){
					std::cout << valid.at(i) << " ";
					if ((i+1)  % 7 == 0) std::cout << "" << std::endl;
				}
				std::cout << "" << std::endl;
				std::cout	<< "\nUnvalid PDB identifiers:\n";
				for (int i = 0; i < unvalid.size(); ++i){
					std::cout << unvalid.at(i) << " ";
					if ((i+1) % 7 == 0) std::cout << "" << std::endl;
				}
				std::cout << "" << std::endl;
			}
		}
	}
	else{
		show_usage(argv[1]);
		return 1;
	}
}

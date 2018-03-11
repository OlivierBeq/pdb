/**
* Main.cpp
*
* A simple C++ software to retrieve files
* from the Protein Data Bank.
*
* Written by Olivier Béquignon
* Published under the MIT License
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <sys/stat.h>
#include "Main.hpp"
#include "StringMagic.hpp"
#include "HTTPDownloader.hpp"

// TODO: When files exists but compressed unzip and keep gzip file.

int main(int argc, char* argv[]) {
    if (argc < 2) {
        show_short_usage(argv[0]);
        return 1;
    }

    std::vector<std::string> arguments;
    for (int i = 1; i < argc; ++i) {
        arguments.push_back((std::string)argv[i]);
    }

    parse_arguments(arguments);
    if (!mode_set) {
        show_short_usage(argv[0]);
        return 1;
    }
    else if (help_mode) {
        show_usage(argv[0]);
        return 0;
    }
    else if (search_mode) {
        if (pdb_codes.size() == 0) {
            show_short_usage(argv[0]);
            return 1;
        }
        display_pdb_type(pdb_codes);
    }
    else if (get_mode) {
        if (pdb_codes.size() == 0) {
            show_short_usage(argv[0]);
            return 1;
        }
        std::vector<PdbType> pdb_types = pdb_type(pdb_codes);
        PdbDownloadType downloadType = get_download_type();
        std::vector<std::string> downloads;
        std::vector<std::string> already_download;
        std::vector<std::string> obsoletes;
        std::vector<std::string> larges;
        std::vector<std::string> unvalids;
        for (int i = 0; i < pdb_codes.size(); ++i) {
            PdbType pdbType = pdb_types.at(i);
            if (pdbType != Unvalid) {
                bool downloaded = download_pdb(pdb_codes.at(i), pdbType, downloadType, compressed_files, large_files);
                if (pdbType == Obsolete && !downloaded) obsoletes.push_back(pdb_codes.at(i));
                else if (pdbType == Large && !downloaded) larges.push_back(pdb_codes.at(i));
                else if (downloaded) downloads.push_back(pdb_codes.at(i));
                else already_download.push_back(pdb_codes.at(i));
            }
            else unvalids.push_back(pdb_codes.at(i));
        }
        display_information("Files already in folder:", already_download, "");
        display_information("Downloaded PDB:", downloads, "");
        display_information("Not downloaded large files (> 99999 atoms and/or > 62 chains):", larges, "\nUse the -large option to download as TAR files each\none containing a collection of best effort/minimal\nfiles in the PDB file format representing the entire\nstructure.");
        display_information("Not downloaded obsolete files:", obsoletes, "\nUse the -obsolete option to force download of obsolete files.");
        display_information("Unvalid PDB identifiers:", unvalids, "");
    }
}


void parse_arguments(std::vector<std::string> argv) {
    int next_arg = 0;

    // Identify mode
    std::string mode = remove_char(argv.at(next_arg), "-/");
    if (mode == "help" || mode == "h" || mode == "?") {
        mode_set = true;
        help_mode = true;
        ++next_arg;
    }
    else if (mode == "search") {
        mode_set = true;
        search_mode = true;
        ++next_arg;
    }
    else if (mode == "get") {
        mode_set = true;
        get_mode = true;
        ++next_arg;
    }
    if (argv.size() < 2) return; // Stop if not enough arguments

    // Identify switches
    std::string compressed = remove_char(argv.at(next_arg), "-/");
    if (compressed == "compressed" || compressed == "c") {
        compressed_files = true;
        ++next_arg;
    }

    std::string large_mmcif_pdbml = remove_char(argv.at(next_arg), "-/");
    if (large_mmcif_pdbml == "large" || large_mmcif_pdbml == "l") {
        large_files = true;
        ++next_arg;
    }
    else if (large_mmcif_pdbml == "mmcif" || large_mmcif_pdbml == "m") {
        mmcif_files = true;
        ++next_arg;
    }
    else if (large_mmcif_pdbml == "pdbml" || large_mmcif_pdbml == "p") {
        pdbml_files = true;
        ++next_arg;
    }

    // Identification of the switch corresponding to obsolete files
    std::string obsolete = remove_char(argv.at(next_arg), "-/");
    if (obsolete == "obsolete" || obsolete == "o") {
        obsolete_files = true;
        ++next_arg;
    }

    // Identification of PDB codes
    for (int i = next_arg; i < argv.size(); ++i) {
        pdb_codes.push_back(to_upper(argv.at(i)));
    }
}

void show_short_usage(std::string name) {
    std::cout << std::endl;
    std::cout << "Usage: " << name << " help" << std::endl;
    std::cout << "       " << name << " search <pdb_code1> <pdb_code2> ..." << std::endl;
    std::cout << "       " << name << " get [-c] [-large | -mmcif | -pdbml] [-obsolete] <pdb_code1> <pdb_code2> ..." << std::endl;
    std::cout << std::endl;
}

void show_usage(std::string name) {
    std::cout << std::endl;
    std::cout << "Usage:   " << name << " help" << std::endl;
    std::cout << "         " << name << " search <pdb_code1> <pdb_code2> ..." << std::endl;
    std::cout << "         " << name << " get [-c] [-large | -mmcif | -pdbml] [-obsolete] <pdb_code1> <pdb_code2> ..." << std::endl;
    std::cout << std::endl;
	std::cout << " Arguments:  (May be preceeded either by - or -- or /)" << std::endl;
	std::cout << "    -help\tDisplays this help" << std::endl;
    std::cout << "      -h  -?\n" << std::endl;
	std::cout << "    -search\tDetermine if supplied pdb codes are valid" << std::endl;
    std::cout << "      -s\n" << std::endl;
    std::cout << "    -get\tDownload pdb files corresponding to supplied pdb codes" << std::endl;
    std::cout << "      -g" << std::endl;
    std::cout << std::endl;
    std::cout << "  >>>> The folowwing arguments are ignored in 'search' mode <<<<" << std::endl;
    std::cout << std::endl;
	std::cout << "    -compressed\tDownload gzipped pdb files" << std::endl;
    std::cout << "      -c\n" << std::endl;
    std::cout << "    -large\tAllow structures with > 99,999 atoms or > 62 chain to be downloaded" << std::endl;
    std::cout << "      -l\tas a tar.gz bundle of pdb files\n" << std::endl;
    std::cout << "    -mmcif\tDownload mmCIF files instead of pdb files" << std::endl;
    std::cout << "      -m\n" << std::endl;
    std::cout << "    -pdbml\tDownload PDBML files instead of pdb files" << std::endl;
    std::cout << "      -p\n" << std::endl;
    std::cout << "    -obsolete\tAllow obsolete structures to be downloaded" << std::endl;
    std::cout << "      -o\n" << std::endl;
}

void display_pdb_type(std::vector<std::string> pdb_codes) {
    std::vector<PdbType> pdb_types = pdb_type(pdb_codes);
    std::vector<std::string> valid;
    std::vector<std::string> large;
    std::vector<std::string> obsolete;
    std::vector<std::string> unvalid;
    for (int i = 0; i < pdb_types.size(); ++i) {
        if (pdb_types.at(i) == Standard) {
            valid.push_back(pdb_codes.at(i));
        }
        else if (pdb_types.at(i) == Large) {
            large.push_back(pdb_codes.at(i));
        }
        else if (pdb_types.at(i) == Obsolete) {
            obsolete.push_back(pdb_codes.at(i));
        }
        else if (pdb_types.at(i) == Unvalid) {
            unvalid.push_back(pdb_codes.at(i));
        }
    }

    display_information("Unvalid PDB identifiers:", unvalid, "");
    display_information("Valid PDB identifiers:", valid, "");
    display_information("Large PDB identifiers (> 99999 atoms and/or > 62 chains):", large, "\nUse the -large option to download as TAR files each\none containing a collection of best effort/minimal\nfiles in the PDB file format representing the entire\nstructure.");
    display_information("Obsolete PDB identifiers:", obsolete, "\nUse the -obsolete option to force download of obsolete files.");
    std::cout << std::endl;
}

void display_information(std::string category_title, std::vector<std::string> pdb_codes, std::string help) {
    if (pdb_codes.size() == 0) return;

    std::cout << std::endl;
    std::cout << category_title << std::endl;
    for (int i = 0; i < pdb_codes.size(); ++i) {
        std::cout << pdb_codes.at(i) << " ";
        if ((i + 1) % 11 == 0) std::cout << "" << std::endl;
    }
    std::cout << ((help.length() != 0) ? "\n" : "") << help << std::endl;
}

PdbDownloadType get_download_type() {
    if (!compressed_files && !mmcif_files && !pdbml_files) { return PDB; }
    else if (compressed_files && !mmcif_files && !pdbml_files) { return PDB_gz; }
    else if (!compressed_files && mmcif_files) { return mmCIF; }
    else if (compressed_files && mmcif_files) { return mmCIF_gz; }
    else if (!compressed_files && pdbml_files) { return PDBML; }
    else if (compressed_files && pdbml_files) { return PDBML_gz; }
}


std::vector<PdbType> pdb_type(std::vector<std::string> pdb_codes) {
    std::string obsolete = get_obsolete_ids();
    std::string large = get_large_ids();
    std::vector<bool> exist = pdb_exist(pdb_codes);

    std::vector<PdbType> types;
    for (int i = 0; i < pdb_codes.size(); ++i) {
        if (exist.at(i) == false) {
            types.push_back(Unvalid);
        }
        else if (obsolete.find(pdb_codes.at(i)) != std::string::npos) {
            types.push_back(Obsolete);
        }
        else if (large.find(pdb_codes.at(i)) != std::string::npos) {
            types.push_back(Large);
        }
        else {
            types.push_back(Standard);
        }
    }
    return types;
}

std::vector<bool> pdb_exist(std::vector<std::string> pdb_codes) {
    std::string url = "https://www.rcsb.org/pdb/rest/customReport.xml?pdbids=";
    for (int i = 0; i < pdb_codes.size() -1 ; ++i) {
        url += pdb_codes.at(i) + ",";
    }
    url += pdb_codes.at(pdb_codes.size() - 1) + "&customReportColumns=residueCount&format=csv";

    HTTPDownloader downloader;
    std::string content = downloader.download(url);
    content = to_upper(replace_substring(replace_substring(content, "<br />", "\n"), "structureId,residueCount\n", ""));

    std::vector<bool> pdb_existence;
    for (int i = 0; i < pdb_codes.size(); ++i) {
        size_t index = content.find(to_upper(pdb_codes.at(i)));
        pdb_existence.push_back(index < std::string::npos);
    }
    return pdb_existence;
}

std::string get_obsolete_ids() {
    HTTPDownloader downloader;
    std::string content = downloader.download("http://www.rcsb.org/pdb/rest/getObsolete");
    content = replace_substring(content, "<obsolete>", "");
    content = replace_substring(content, "<PDB structureId=\"", "");
    content = replace_substring(content, "\"/>", "");
    content = replace_substring(content, "</obsolete>", "");
    content = replace_substring(content, "\n", " ");
    return to_upper(content);
}

std::string get_large_ids() {
    HTTPDownloader downloader;
    std::string content = downloader.download("ftp://ftp.wwpdb.org/pub/pdb/compatible/pdb_bundle/pdb_bundle_index.txt");
    content = replace_substring(content, "\n", " ");
    return to_upper(content);
}


void download_pdb(std::string pdb_code, bool gzipped) {
	std::string filename = pdb_code + (gzipped ? "pdb.gz" : ".pdb");

	std::string url;
	if (!gzipped) {
		url = "https://files.rcsb.org/view/" + filename;
        std::string command = "wget -O " + filename + " --quiet " + url;
        system((const char*)command.c_str());
		return;
	}
	else {
		url = "ftp://ftp.rcsb.org/pub/pdb/data/structures/all/pdb/pdb" + to_lower(pdb_code) + ".ent.gz";
		std::string command = "wget -O " + filename + " --quiet " + url;
		system((const char*)command.c_str());
		return;
	}
}

bool download_pdb(std::string pdb_code, PdbType pdbType, PdbDownloadType downloadType, bool gzipped, bool largePdbBundle) {
    
    std::string filename, url = "ftp://ftp.wwpdb.org/pub/pdb";
    if ((pdbType == Large && !largePdbBundle) || (pdbType == Obsolete && !obsolete_files)) return false;
    else if (pdbType == Large && largePdbBundle) {
        filename = to_lower(pdb_code) + "-pdb-bundle.tar.gz";
        url += "/compatible/pdb_bundle/" + to_lower(pdb_code.substr(1, 2)) + "/" + to_lower(pdb_code) + "/" + filename;
    }
    else {
        // URL start
        if (pdbType == Obsolete) {
            url += "/data/structures/obsolete/";
        }
        else {
            url += "/data/structures/divided/";
        }
        // URL part depending on file type
        if (downloadType == PDB || downloadType == PDB_gz) {  // Obsolete PDB
            url += "pdb/" + to_lower(pdb_code.substr(1, 2)) + "/pdb" + to_lower(pdb_code) + ".ent.gz";
            filename = to_upper(pdb_code) + ".pdb.gz";
        }
        else if (downloadType == mmCIF || downloadType == mmCIF_gz) {  // Obsolete mmCIF
            url += "mmCIF/" + to_lower(pdb_code.substr(1, 2)) + "/" + to_lower(pdb_code) + ".cif.gz";
            filename = to_upper(pdb_code) + ".cif.gz";
        }
        else if (downloadType == PDBML || downloadType == PDBML_gz) {  // Obsolete PDBML
            url += "XML/" + to_lower(pdb_code.substr(1, 2)) + "/" + to_lower(pdb_code) + ".xml.gz";
            filename = to_upper(pdb_code) + ".xml.gz";
        }
    }

    bool uncompressed = (pdbType != Large && (downloadType == PDB || downloadType == mmCIF || downloadType == PDBML));
    
    if (file_exists(uncompressed ? replace_substring(filename,".gz","") : filename)) return false;

    //Download
    std::string command = "wget -O " + filename + " --quiet " + url;
    system((const char*)command.c_str());

    // Unzip file
    if (uncompressed) {
        unzip(filename);
    }
    return true;
}

bool file_exists(std::string filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void unzip(std::string filename) {
    std::string command = "gunzip -f -q " + filename;
    system((const char*)command.c_str());
}
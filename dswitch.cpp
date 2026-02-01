#include "command_parser.hpp"
#include "directory_entry.hpp"
#include "directory_entry_table.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>

static std::string TAG_FILE_NAME      = "tags";
static std::string PREV_TAG_NAME_FILE = "prev";
static std::string DSWITCHER_HOME     = ".dswitcher";

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

static std::string getHomeDirectory() {
    char* buffer = new char[MAX_PATH];

    _dupenv_s(&buffer, nullptr, "USERPROFILE");

    if (buffer == nullptr) {
        char* drive = new char[MAX_PATH];
        char* path  = new char[MAX_PATH];

        _dupenv_s(&drive, nullptr, "HOMEDRIVE");
        _dupenv_s(&path , nullptr, "HOMEPATH");

        if (drive && path) {
            std::string home = std::string(drive) + path;

            delete[] drive;
            delete[] path;

            return home;
        }

        delete[] drive;
        delete[] path;
    }

    std::string home = std::string(buffer);
    delete buffer;
    return home;
}

#else

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

static std::string getHomeDirectory() {
    return std::string(getpwuid(getuid())->pw_dir);
}

#endif 

static std::string getFileNameImpl(std::string& fileName) {
    std::string homeDir = getHomeDirectory();

    if (homeDir.empty()) {
        throw std::logic_error("Unknown home directory.");
    } else {
        char separator = std::filesystem::path::preferred_separator;

        return homeDir
            + separator 
            + DSWITCHER_HOME
            + separator 
            + fileName;
    }
}

static std::string getTagsFileName() {
    return getFileNameImpl(TAG_FILE_NAME);
}

static std::string getPrevTagFileName() {
    return getFileNameImpl(PREV_TAG_NAME_FILE);
}

/*
static const std::string OPTION_LIST_TAGS             = "-l";
static const std::string OPTION_LIST_FULL             = "-L";
static const std::string OPTION_LIST_SORTED_TAGS      = "-s";
static const std::string OPTION_LIST_SORTED_FULL      = "-S";
static const std::string OPTION_LIST_DIRS_ONLY        = "-o";
static const std::string OPTION_LIST_DIRS_ONLY_SORTED = "-O";
static const std::string OPTION_LIST_DIRS_TAGS        = "-d";
static const std::string OPTION_LIST_SORTED_DIRS_TAGS = "-D";
static const std::string OPTION_REVERSE_ORDER         = "-r";
static const std::string OPTION_ADD_ENTRY             = "-a";
static const std::string OPTION_REMOVE_ENTRIES        = "-x";*/

static void handlePreviousSwitch() {
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string cwdName = cwd.string();
    std::string prevFileName = getPrevTagFileName();

    // Make sure the file exists:
    std::ofstream ofs_init(prevFileName, std::ios::app); 
    ofs_init.close();

    std::ifstream ifs(prevFileName, std::ios_base::in);
    std::string prevDirectory;

    if (!ifs) {
        throw std::logic_error("Could not open the prev file for reading.");
    }

    ifs >> prevDirectory;
    ifs.close();

    std::ofstream ofs(prevFileName, std::ios_base::trunc);

    if (!ofs) {
        throw std::logic_error("Could not open the prev file for writing.");
    }

    ofs << cwdName;
    ofs.close();

    std::cout << "cd " << prevDirectory;
}

using namespace io::github::coderodde::dswitch;

static std::string expandTilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }

    std::string home = getHomeDirectory();

    if (home.size() == 1) {
        return home;
    }

    char separator = std::filesystem::path::preferred_separator;

    if (path[0] == '~') {
        return home + separator + path.substr(2);
    }

    return path;
}

static void listTags(const DirectoryEntryTable& table) {
    for (size_t i = 0; i < table.size(); ++i) {
        const DirectoryEntry& entry = table.getEntry(i);
        std::cout << entry.getTagName() << "\n";
    }
}

static std::size_t computeMaxTagLength(
    const DirectoryEntryTable& table) {

    std::size_t max_length = 0;

    for (size_t i = 0; i < table.size(); ++i) {

        const DirectoryEntry& entry = table.getEntry(i);
        std::size_t length = entry.getTagName().size();
        max_length = max(max_length, length);
    }

    return max_length;
}

static std::string getTagFormatString(
    const DirectoryEntryTable& table) {
    std::size_t max_length = table.getLongestTagLength();

    return "%-" + std::to_string(max_length) + "s -> %s\n";
}

static void listFull(const DirectoryEntryTable& table) {
    const std::size_t tag_len = table.getLongestTagLength();

    for (size_t i = 0; i < table.size(); ++i) {
        const DirectoryEntry& entry = table.getEntry(i);

        std::cout << std::left 
                  << std::setw(tag_len) 
                  << entry.getTagName()
                  << " -> "
                  << entry.getTagDirectory() 
                  << "\n";
    }
}

int main(int argc, char* argv[]) {
    CommandParser commandParser(argc, argv);

    std::string tableFileName = getTagsFileName();
    std::ifstream ifs(tableFileName);

    DirectoryEntryTable table;

    ifs >> table;

    if (argc == 1) {
        handlePreviousSwitch();
    } else if (argc == 2) {
        std::string opt = argv[1];

        if (opt == OPTION_LIST_TAGS) {
            listTags(table);
        } else if (opt == OPTION_LIST_FULL) {
            listFull(table);
        }

        DirectoryEntry* entry = table.findEntryByTagName(opt);

        if (entry != nullptr) {
            std::string dirName = entry->getTagDirectory();                 
            dirName = expandTilde(dirName);
            std::cout << dirName << "\n";
        } else {
            std::cerr << "[ERROR] Tag file is empty.\n";
            return EXIT_FAILURE;
        }
    }

    DirectoryEntry entry;
    return EXIT_SUCCESS;
}

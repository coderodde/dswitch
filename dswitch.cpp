#include "directory_entry.hpp"
#include "directory_entry_table.hpp"
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
static const std::string OPTION_REMOVE_ENTRIES        = "-x";

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

int main(int argc, char* argv[]) {
    std::string tableFileName = getTagsFileName();
    std::ifstream ifs(tableFileName);

    DirectoryEntryTable table;

    ifs >> table;

    if (argc == 1) {
        handlePreviousSwitch();
    } else if (argc == 2) {
        std::string opt = argv[1];

        DirectoryEntry* entry = table.findEntryByTagName(opt);

        if (entry != nullptr) {
            std::filesystem::path dirPath = entry->getTagDirectory();
            if (std::filesystem::exists(dirPath) &&
                std::filesystem::is_directory(dirPath)) {
                std::cout << "cd " << dirPath.string();
            } else {
                std::cerr << "Error: Directory does not exist: "
                          << dirPath.string() << "\n";
                return EXIT_FAILURE;
            }
        } else {
            std::cerr << "Error: No such tag: " << opt << "\n";
            return EXIT_FAILURE;
        }
    }

    DirectoryEntry entry;
    return EXIT_SUCCESS;
}

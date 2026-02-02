#include "directory_entry.hpp"
#include "directory_entry_table.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>

using io::github::coderodde::dswitch::DirectoryEntry;
using io::github::coderodde::dswitch::DirectoryEntryTable;

static const std::string TAG_FILE_NAME      = "tags";
static const std::string PREV_TAG_NAME_FILE = "prev";
static const std::string COMMAND_FILE_NAME  = ".ds_command";
static const std::string DSWITCHER_HOME     = ".dswitcher";

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <string>

static std::string getHomeDirectory() {
    char* buf = nullptr;
    size_t len = 0;

    if (_dupenv_s(&buf, &len, "USERPROFILE") == 0 && buf) {
        std::string home(buf);
        free(buf);
        return home;
    }

    char* drive = nullptr;
    char* path = nullptr;

    _dupenv_s(&drive, &len, "HOMEDRIVE");
    _dupenv_s(&path,  &len, "HOMEPATH");

    std::string home;
    if (drive && path) home = std::string(drive) + std::string(path);

    if (drive) free(drive);
    if (path)  free(path);

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

static std::string getFileNameImpl(const std::string& fileName) {
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

static void writeCommandFile(const std::string& text) {
    const std::string fileName = getFileNameImpl(COMMAND_FILE_NAME);
    std::ofstream out(fileName, std::ios::trunc);

    if (!out) {
        throw std::logic_error("Could not write to the commamnd file.");
    }

    out << text;
    out.close();
}

static std::string getTagsFileName() {
    return getFileNameImpl(TAG_FILE_NAME);
}

static std::string getPrevTagFileName() {
    return getFileNameImpl(PREV_TAG_NAME_FILE);
}

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

    writeCommandFile("cd " + prevDirectory);
}

static std::string expandTilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }

    std::string home = getHomeDirectory();

    if (home.empty()) {
        return path;
    }

    if (path == "~") {
        return home;
    }

    // handle "~/..."
    if (path.size() >= 2 && path[0] == '~' 
                         && (path[1] == '/' || path[1] == '\\')) {

        return home + path.substr(1); // keeps the slash
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
        max_length = std::max(max_length, length);
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

static void trySwitchByTag(DirectoryEntryTable& table,
                           const std::string tag)  {
    DirectoryEntry* p_entry = table.findEntryByTagName(tag);

    if (p_entry == nullptr) {
        throw std::logic_error("Tag file does not contain any entry.");
    }

    const std::string path = p_entry->getTagDirectory();
    writeCommandFile("cd " + path);
}

int main(int argc, char* argv[]) {

    std::string tableFileName = getTagsFileName();
    std::ifstream ifs(tableFileName);

    DirectoryEntryTable table;

    ifs >> table;

    if (argc == 1) {
        handlePreviousSwitch();
    } else if (argc == 2) {
        std::string opt = argv[1];

        if (opt == "-L") {
            table.printTagsAndDirs();
        } else if (opt == "-l") {
            table.printTags();
        } else if (opt == "-S") {
            table.sortByTag();
            table.printTagsAndDirs();
        } else if (opt == "-s") {
            table.sortByTag();
            table.printTags();
        } else if (opt == "-D") {
            table.sortByDirectory();
            table.printDirsAndTags();
        } else if (opt == "-d") {
            table.printDirsAndTags();
        } else {
            trySwitchByTag(table, opt);
        }
    } else if (argc == 4) {
        // Should be the -a command (... -a tag dir):
        std::string opt = argv[1];

        if (opt == "-a") {
            // Once here, add an entry:
            table.addEntry(argv[2], argv[3]);
            std::ofstream ofs(getTagsFileName(), std::ios::trunc);
            ofs << table;
        } else if (opt == "-x") {
            // Once here (argc == 4), we are removing two tags:
            table.removeEntry(argv[2]);
            table.removeEntry(argv[3]);
            std::ofstream ofs(getTagsFileName(), std::ios::trunc);
            ofs << table;
        } else {
            std::cerr << "[ERROR] Invalid command line.\n";
            return EXIT_FAILURE;
        }
    } else {
        std::string opt = argv[1];

        if (opt == "-x") {
            for (int i = 2; i < argc; ++i) {
                table.removeEntry(argv[i]);
            }

            std::ofstream ofs(getPrevTagFileName(), std::ios::trunc);
            ofs << table;
        } else {
            std::cerr << "[ERROR] Invalid command line.\n";
        }
    }

    return EXIT_SUCCESS;
}

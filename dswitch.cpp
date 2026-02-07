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

#ifdef _WIN32
static const std::string COMMAND_FILE_NAME  = "ds_command.bat";
#else
static const std::string COMMAND_FILE_NAME = "ds_command.sh";
#endif

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

static void handlePreviousSwitch() {
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string cwdName = cwd.string();
    std::string prevFileName = getPrevTagFileName();

    std::ifstream ifs(prevFileName);
    std::string previoudDirectory;

    ifs >> previoudDirectory;
    ifs.close();

    previoudDirectory = expandTilde(previoudDirectory);

    std::ofstream ofs(prevFileName, std::ios::trunc);
    ofs << cwdName;
    ofs.close();

    writeCommandFile("cd " + previoudDirectory + "\n");
}

static void updatePreviousDirectory() {
    std::filesystem::path cwd = std::filesystem::current_path();
    std::string cwdName = cwd.string();
    std::string prevFileName = getPrevTagFileName();
    std::ofstream ofs(prevFileName, std::ios::trunc);
    ofs << cwdName;
    ofs.close();
}

static void listTags(const DirectoryEntryTable& table) {
    for (size_t i = 0; i < table.size(); ++i) {
        const DirectoryEntry& entry = table.getEntry(i);
        std::cout << entry.getTagName() << "\n";
    }
}

static void trySwitchByTag(DirectoryEntryTable& table,
                           const std::string tag)  {
    DirectoryEntry* p_entry = table.findEntryByTagName(tag);

    if (p_entry == nullptr) {
        throw std::logic_error("Tag file does not contain any entry.");
    }

    std::string path = p_entry->getTagDirectory();
    path = expandTilde(path);
    writeCommandFile("cd " + path + "\n");
    updatePreviousDirectory();
}

static void printHelp() {
    std::cout << "ds [-l | -L | -s | -S | -d | -D]\n" 
              << "    -l          list tags\n"
              << "    _L          list tags and directories\n"
              << "    -s          list tags in alphabetic order\n"
              << "    -sr or -rs  same as -s but in reverse order\n"
              << "    -S          list tags and directories sorted by tags\n"
              << "    -Sr or -rS  same as -S but in reverse order\n"
              << "    -d          list directories first and then tags\n"
              << "    -D          list directoires and tags sorted by directories\n"
              << "    -Dr or -rD  same as -D but in reverse order\n"
              << "\n"
              << "ds -a TAG DIR\n"
              << "    associate TAG with DIR\n"
              << "\n"
              << "ds -x TAG1 TAG2 ...\n"
              << "    remove all tags TAG1 TAG2 ...\n" 
              << "\n"
              << "ds TAG\n"
              << "    switch to directory closest to TAG\n"
              << "\n"
              << "ds\n"
              << "   (no arguments) jump to the previous directory\n";
}       

int main(int argc, char* argv[]) try {

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
        } else if (opt == "-Sr" || opt == "-rS") {
            table.sortByTag();
            table.reverse();
            table.printTagsAndDirs();
        } else if (opt == "-s") {
            table.sortByTag();
            table.printTags();
        } else if (opt == "-sr" || opt == "-rs") {
            table.sortByTag();
            table.reverse();
            table.printTags();
        } else if (opt == "-D") {
            table.sortByDirectory();
            table.printDirsAndTags();
        } else if (opt == "-Dr" || opt == "-rD") {
            table.sortByDirectory();
            table.reverse();
            table.printDirsAndTags();
        } else if (opt == "-d") {
            table.printDirsAndTags();
        } else if (opt == "-h") {
            printHelp();
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

            std::ofstream ofs(getTagsFileName(), std::ios::trunc);
            ofs << table;
        } else {
            std::cerr << "[ERROR] Invalid command line.\n";
        }
    }

    return EXIT_SUCCESS;
} catch (std::logic_error& err) {
    std::cerr << "[ERROR] " << err.what() << "\n";
}

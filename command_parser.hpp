#ifndef IO_GITHUB_CODERODDE_DSWITCH_COMMAND_PARSER_H_
#define IO_GITHUB_CODERODDE_DSWITCH_COMMAND_PARSER_H_

#include "directory_entry.hpp"
#include <string>
#include <vector>

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


class CommandParser {
public:
    CommandParser(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string flag = argv[i];;

            if (flag == OPTION_LIST_TAGS) {
                list_tags = true;
            } else if (flag == OPTION_LIST_FULL) {
                list_tags = true;
                list_dirs = true;
            } else if (flag == OPTION_LIST_SORTED_TAGS) {
                list_tags     = true;
                sorted_by_tag = true;
            } else if (flag == OPTION_LIST_SORTED_FULL) {
                list_tags = true;
                list_dirs = true;
                sorted_by_tag  = true;
            }
            else if (flag == OPTION_LIST_DIRS_TAGS) {
                list_tags = true;
                list_dirs = true;
                dir_first = true;
            } else if (flag == OPTION_LIST_SORTED_DIRS_TAGS) {
                list_tags = true;
                list_dirs = true;
                dir_first = true;
                sorted_by_dir  = true;
            } else if (flag == OPTION_LIST_DIRS_ONLY) {
                list_dirs = true;
            } else if (flag == OPTION_LIST_DIRS_ONLY_SORTED) {
                list_dirs     = true;
                sorted_by_dir = true;
            } else if (flag == OPTION_REVERSE_ORDER) {
                reverse = true;
            } else if (flag == OPTION_ADD_ENTRY) {
                if (argc != 4) {
                    throw std::logic_error("-a flag must have two arguments.");
                }

                add = true;
                tag = argv[2];
                dir = argv[3];
            } else if (flag == OPTION_REMOVE_ENTRIES) {
                if (argc < 3) {
                    throw std::logic_error(
                        "-x flag requires at least one parameter tag.");
                }

                remove = true;

                for (int i = 2; i < argc; ++i) {
                    removal_tags.emplace_back(std::string(argv[i]));
                }
            }
        }
    }

    bool requestedNewEntry() const {
        return add;
    }

    bool requestedEntryRemoval() const {
        return remove;
    }

    DirectoryEntry getNewEntry() const {
        DirectoryEntry entry(tag, dir);
        return entry;
    }

    const std::vector<std::string> getRemovalTagNames() const {
        return removal_tags;
    }

private:
    bool reverse           = false;
    bool list_tags         = false;
    bool list_dirs         = false;
    bool sorted_by_tag     = false;
    bool sorted_by_dir     = false;
    bool dir_first         = false;
    bool add               = false;
    bool remove            = false;

    std::string tag;
    std::string dir;
    std::vector<std::string> removal_tags;
};

#endif // IO_GITHUB_CODERODDE_DSWITCH_COMMAND_PARSER_H_
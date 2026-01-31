#ifndef IO_GITHUB_CODEROBOTS_DIRECTORY_ENTRY_TABLE_HPP_
#define IO_GITHUB_CODEROBOTS_DIRECTORY_ENTRY_TABLE_HPP_

#include "directory_entry.hpp"
#include <algorithm>
#include <fstream>
#include <vector>

namespace io::github::coderodde::dswitch {

class DirectoryEntryTable {
 public:
    DirectoryEntryTable()  = default;
    ~DirectoryEntryTable() = default;

    void addEntry(const DirectoryEntry& entry) {
        entries.push_back(entry);
    }

    const DirectoryEntry& getEntry(size_t index) const {
        return entries.at(index);
    }

    size_t size() const {
        return entries.size();
    }

    void sortByTag() {
        std::sort(entries.begin(),
                  entries.end(), 
                  [](const DirectoryEntry& a,
                     const DirectoryEntry& b) {
                      return a.getTagName() < b.getTagName();
                  });
    }

    void sortByDirectory() {
        std::sort(entries.begin(),
                  entries.end(), 
                  [](const DirectoryEntry& a, 
                     const DirectoryEntry& b) {
                      return a.getTagDirectory() < b.getTagDirectory();
                  });
    }

    void reverse() {
        std::reverse(entries.begin(), 
                     entries.end());
    }

    friend std::ofstream& operator<<(std::ofstream& os, 
                                     const DirectoryEntryTable& table) {

        for (const auto& entry : table.entries) {
            os << entry.getTagName() 
               << " " 
               << entry.getTagDirectory()
               << "\n";
        }

        return os;
    }

    friend std::ifstream& operator>>(std::ifstream& is, 
                                     DirectoryEntryTable& table) {
        DirectoryEntry entry;

        while (is >> entry) {
            table.addEntry(entry);
        }

        return is;
    }

 private:
    std::vector<DirectoryEntry> entries;
};

} // namespace io::github::coderodde::dswitch

#endif
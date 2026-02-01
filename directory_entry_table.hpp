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

    bool addEntry(const std::string& tagName,
                  const std::string& tagDirectory) {

        // Try update:
        for (auto& e : entries) {
            if (e.getTagName() == tagName) {
                e.setTagDirectory(tagDirectory);
                return false;
            }
        }

        // Once here, the entry does not exist, create:
        DirectoryEntry entry;
        entry.setTagName(tagName);
        entry.setTagDirectory(tagDirectory);
        entries.emplace_back(entry);
        return true;
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

    std::size_t getLongestTagLength() const {
        std::size_t len = 0;

        for (std::size_t i = 0; i < entries.size(); ++i) {
            len = std::max(len, entries.at(i).getTagName().length());
        }

        return len;
    }

    std::size_t getLongestDirectoryLength() const {
        std::size_t len = 0;

        for (std::size_t i = 0; i < entries.size(); ++i) {
            len = std::max(len, entries.at(i).getTagDirectory().length());
        }

        return len;
    }

    DirectoryEntry* findEntryByTagName(const std::string& tagName) {
        DirectoryEntry target;
        target.setTagName(tagName);

        std::size_t maximum_distance = SIZE_MAX;
        DirectoryEntry* closest_entry = nullptr;

        for (auto& entry : entries) {
            std::size_t distance = 
                entry.computeLevenshteinDistance(target);

            if (distance == 0) {
                return &entry;
            }

            if (maximum_distance > distance) {
                maximum_distance = distance;
                closest_entry = &entry;
            }
        }
        
        return closest_entry;
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

    void addEntry(const DirectoryEntry& entry) {
         addEntry(entry.getTagName(),
                  entry.getTagDirectory());
    }

    std::vector<DirectoryEntry> entries;
};

} // namespace io::github::coderodde::dswitch

#endif
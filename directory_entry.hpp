#ifndef IO_GITHUB_CODERODDE_DIRECTORY_ENTRY_HPP_
#define IO_GITHUB_CODERODDE_DIRECTORY_ENTRY_HPP_

#include <fstream>
#include <string>

namespace io::github::coderodde::dswitch {

class DirectoryEntry {
 public:
    DirectoryEntry() = default;
    ~DirectoryEntry() = default;

    std::string getTagName() const {
        return tag_name;
    }

    void setTagName(std::string name) {
        tag_name = name;
    }

    std::string getTagDirectory() const {
        return tag_dir;
    }

    void setTagDirectory(std::string dir) {
        tag_dir = dir;
    }

    friend std::ofstream& operator<<(std::ofstream& os, 
                                     const DirectoryEntry& entry);

    friend std::ifstream& operator>>(std::ifstream& is, 
                                     DirectoryEntry& entry) {
        std::string line;

        if (std::getline(is, line)) {
            size_t delimiter_pos = line.find(' ');

            if (delimiter_pos != std::string::npos) {
                entry.tag_name = line.substr(0, delimiter_pos);
                entry.tag_dir  = line.substr(delimiter_pos + 1);
            }
        }

        return is;
    }
    private:
        std::string tag_name;
        std::string tag_dir;
};

std::ofstream& operator<<(std::ofstream& os, 
                         const DirectoryEntry& entry) {
                        
    os << entry.tag_name << ":" << entry.tag_dir;
    return os;
}

} // namespace io::github::coderodde
#endif// IO_GITHUB_CODERODDE_DIRECTORY_ENTRY_HPP_
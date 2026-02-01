#ifndef IO_GITHUB_CODERODDE_DIRECTORY_ENTRY_HPP_
#define IO_GITHUB_CODERODDE_DIRECTORY_ENTRY_HPP_

#include <fstream>
#include <string>
#include <vector>

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

    std::size_t computeLevenshteinDistance(
        const DirectoryEntry& other) const {
        const std::string& s1 = tag_name;
        const std::string& s2 = other.tag_name;

        const std::size_t m = s1.size();
        const std::size_t n = s2.size();

        std::vector<std::vector<std::size_t>> dp(m + 1, 
                    std::vector<std::size_t>(n + 1));

        for (std::size_t i = 0; i <= m; ++i) {
            dp[i][0] = i;
        }

        for (std::size_t j = 1; j <= n; ++j) {
            dp[0][j] = j;
        }

        for (std::size_t i = 1; i <= m; ++i) {
            for (std::size_t j = 1; j <= n; ++j) {

                if (s1[i - 1] == s2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    dp[i][j] = std::min({dp[i - 1][j] + 1,    // Deletion
                                         dp[i][j - 1] + 1,    // Insertion
                                         dp[i - 1][j - 1] + 1 // Substitution
                                         });
                }
            }
        }

        return dp[m][n];
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

#include <string>
#include <vector>

class KMP {
private:
    std::vector<int> computePrefixFunction(const std::string& pattern) {
        std::vector<int> pi(pattern.length(), 0);
        int k = 0;
        
        for (int i = 1; i < pattern.length(); i++) {
            while (k > 0 && pattern[k] != pattern[i]) {
                k = pi[k - 1];
            }
            if (pattern[k] == pattern[i]) {
                k++;
            }
            pi[i] = k;
        }
        return pi;
    }

public:
    std::vector<int> search(const std::string& text, const std::string& pattern) {
        std::vector<int> result;
        std::vector<int> pi = computePrefixFunction(pattern);
        int k = 0;

        for (int i = 0; i < text.length(); i++) {
            while (k > 0 && pattern[k] != text[i]) {
                k = pi[k - 1];
            }
            if (pattern[k] == text[i]) {
                k++;
            }
            if (k == pattern.length()) {
                result.push_back(i - pattern.length() + 1);
                k = pi[k - 1];
            }
        }
        return result;
    }

    bool matchCount(const std::string& text, const std::string& pattern, int requiredCount) {
        std::vector<int> matches = search(text, pattern);
        return matches.size() >= requiredCount;
    }
};

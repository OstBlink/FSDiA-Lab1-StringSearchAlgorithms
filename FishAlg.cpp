
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>

class AhoCorasick {
private:
    struct Node {
        std::map<char, int> next;
        int link;
        int parent;
        char symbol;
        std::vector<int> pattern_ids;
        std::map<char, int> go;
        
        Node(int p = -1, char c = '$') : link(-1), parent(p), symbol(c) {}
    };

    std::vector<Node> trie;
    std::vector<std::string> patterns;

public:
    AhoCorasick() {
        trie.emplace_back();
    }

    void addPattern(const std::string& pattern, int id) {
        int node = 0;
        for (char c : pattern) {
            if (!trie[node].next.count(c)) {
                trie[node].next[c] = trie.size();
                trie.emplace_back(node, c);
            }
            node = trie[node].next[c];
        }
        trie[node].pattern_ids.push_back(id);
        patterns.push_back(pattern);
    }

private:
    int getLink(int node) {
        if (trie[node].link == -1) {
            if (node == 0 || trie[node].parent == 0) {
                trie[node].link = 0;
            } else {
                trie[node].link = go(getLink(trie[node].parent), trie[node].symbol);
            }
        }
        return trie[node].link;
    }

    int go(int node, char c) {
        if (!trie[node].go.count(c)) {
            if (trie[node].next.count(c)) {
                trie[node].go[c] = trie[node].next[c];
            } else {
                trie[node].go[c] = node == 0 ? 0 : go(getLink(node), c);
            }
        }
        return trie[node].go[c];
    }

public:
    std::vector<std::pair<int, int>> search(const std::string& text) {
        std::vector<std::pair<int, int>> results;
        int currentState = 0;

        for (int i = 0; i < text.length(); ++i) {
            currentState = go(currentState, text[i]);
            int checkState = currentState;
            
            while (checkState > 0) {
                if (!trie[checkState].pattern_ids.empty()) {
                    for (int patternId : trie[checkState].pattern_ids) {
                        int pos = i - patterns[patternId].length() + 1;
                        results.emplace_back(pos, patternId);
                    }
                }
                checkState = getLink(checkState);
            }
        }
        return results;
    }

    void reset() {
        trie.clear();
        trie.emplace_back();
        patterns.clear();
    }
};

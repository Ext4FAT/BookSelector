/*************************************************************************
	> File Name: Trie.hpp
	> Author: zyy
	> Mail: zyy34472@gmail.com
	> Created Time: Thu 21 Jan 2016 03:32:21 PM CST
 ************************************************************************/

#include <iostream>
#include <map>

class TrieNode {
public:
    // Initialize your data structure here.
    char c;
    TrieNode* child[128];
    //std::map<char, TrieNode*> child;
    TrieNode(char character) 
    {
        c = character;
    }
};

class Trie {
public:
    //Construction
    Trie() 
    {
        root = new TrieNode('\0');
        end = new TrieNode('\0');
    }
    
    // Inserts a word into the trie.
    void insert(std::string word); 

    // Returns if the word is in the trie.
    bool search(std::string word); 

    // Returns if there is any word in the trie that starts with the given prefix.
    bool startsWith(std::string prefix);

private:
    TrieNode* root;
    TrieNode* end;
};

// [Usage]
// 	Trie trie;
// 	trie.insert("somestring");
// 	trie.search("key");

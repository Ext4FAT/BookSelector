/*************************************************************************
	> File Name: Trie.cpp
	> Author: zyy
	> Mail: zyy34472@gmail.com
	> Created Time: Thu 21 Jan 2016 03:45:17 PM CST
 ************************************************************************/

#include "Trie.hpp"

#define _SEARCH_TRIE_(_W_)	\
	TrieNode *tmp = root, *child; \
	for (auto w: _W_) { \
		child = tmp->child[w]; \
		if (!child) return false; \
		tmp = child; \
	}	

//Trie
void Trie::insert(std::string word)
{
	TrieNode *tmp = root, *child;
	for (auto w: word) {
		child = tmp->child[w];
		if (!child) {
			child = new TrieNode(w);       
			tmp->child[w] = child;
		}
		tmp = child;
	}
	tmp->child[0] = end;
}

bool Trie::search(std::string word) 
{
	_SEARCH_TRIE_(word);
	return tmp->child[0] == end;
}

bool Trie::startsWith(std::string prefix)
{
	_SEARCH_TRIE_(prefix);
	return true;
}

#include <vector>
using namespace std;

#ifndef CREATESUDOKUPUZZLE_H
#define CREATESUDOKUPUZZLE_H

struct ListNode {
    int val, row;
    ListNode* right;
    ListNode* left;
    ListNode* down;
    ListNode* up;
    ListNode* header;
    ListNode(): val(1), row(-1), left(nullptr), right(nullptr), up(nullptr), down(nullptr), header(nullptr) {}
    ListNode(int x, int y): val(x), row(y), left(nullptr), right(nullptr), up(nullptr), down(nullptr), header(nullptr) {}
};

void unlink(ListNode* node);

void relink(ListNode* node);

ListNode* get_matrix(int size);

template <typename T>
bool solve(ListNode* head, vector<ListNode*>& solution, int size, T rng, int& solutions);

template <typename T>
vector<ListNode*> puzzlify(ListNode* head, vector<ListNode*> scrambled, int size, T rng, int difficulty);

template<typename T>
bool remove_selected_nodes(ListNode* head, vector<ListNode*> nodes, int size, T rng);

vector<vector<int>> get_puzzle();
// int main();

#endif
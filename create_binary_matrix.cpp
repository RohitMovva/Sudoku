#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <chrono>
using namespace std;
 
struct LinkedListNode {
    int val, row;
    LinkedListNode* right;
    LinkedListNode* left;
    LinkedListNode* down;
    LinkedListNode* up;
    LinkedListNode* header;
    LinkedListNode() : val(1), row(-1), left(nullptr), right(nullptr), up(nullptr), down(nullptr), header(nullptr) {}
    LinkedListNode(int x, int y) : val(x), row(y), left(nullptr), right(nullptr), up(nullptr), down(nullptr), header(nullptr) {}
};
void unlink(LinkedListNode* node){
    node = node->header;
    node->left->right = node->right;
    node->right->left = node->left;
    LinkedListNode* iter = node->down;
    while (iter != node){
        LinkedListNode* rowiter = iter->right;
        while (rowiter != iter){
            rowiter->up->down = rowiter->down;
            rowiter->down->up = rowiter->up;
            rowiter->header->val--;
            rowiter = rowiter->right;
        }
        iter = iter->down;
    }
}

void relink(LinkedListNode* node){
    node = node->header;
    LinkedListNode* iter = node->up;
    while (iter != node){
        LinkedListNode* rowiter = iter->left;
        while (rowiter != iter){
            rowiter->up->down = rowiter;
            rowiter->down->up = rowiter;
            rowiter->header->val++;
            rowiter = rowiter->left;
        }
        iter = iter->up;
    }
    node->left->right = node;
    node->right->left = node;
}

LinkedListNode* get_matrix(int size){
    int rows = size*size*size;
    int cols = 4*size*size;
    int subsect = sqrt(size);
    vector<vector<int>> matrix(rows+1, vector<int>(cols, 0));
    fill(matrix[0].begin(), matrix[0].end(), size);
    for (int r = 0; r < rows; r++){
        matrix[r+1][r/size] = 1;
        matrix[r+1][r%size + size*(r/(size*size)) + size*size] = 1; // rows
        matrix[r+1][r%(size*size) + 2*(size*size)] = 1; // columns
        matrix[r+1][r%(size) + size*(r/(size*subsect)%subsect) + subsect*size*(r/(size*size*subsect)) + 3*(size*size)] = 1; // squares
    }
    LinkedListNode* head = new LinkedListNode(4*size*size, 0);
    vector<LinkedListNode*> stuff = {};
    for (int i = 0; i < rows+1; i++){
        LinkedListNode* temp = nullptr;
        for (auto& j: matrix[i]){
            if (j == 0) continue;
            if (temp == nullptr){
                temp = new LinkedListNode(j, i);
                stuff.push_back(temp);
                if (head->right == nullptr){
                    head->right = temp;
                    temp->left = head;
                    stuff.back() = head;
                }
                continue;
            }
            temp->right = new LinkedListNode(j, i);
            temp->right->left = temp;
            temp = temp->right;
        }
        stuff[stuff.size()-1]->left = temp;
        temp->right = stuff[stuff.size()-1];
    }
    stuff[0] = stuff[0]->right;
    for (int i = 0; i < cols; i++){
        LinkedListNode* currhead = stuff[0]; // new LinkedListNode(matrix[0][i])
        stuff[0] = stuff[0]->right;
        LinkedListNode* temp = currhead;
        for (int j = 1; j < rows+1; j++){
            if (matrix[j][i] == 0){continue;}
            temp->down = stuff[j];
            temp->header = currhead;
            stuff[j]->up = temp;
            temp = stuff[j];
            stuff[j] = stuff[j]->right;
        }
        temp->down = currhead;
        temp->header = currhead;
        currhead->up = temp;

    }
    return head;
}

template <typename T>

bool solve(LinkedListNode* head, vector<LinkedListNode*>& solution, int size, T rng, int& solutions){
    if (!head || !head->right) {
        return true;
    }
    int min = INT32_MAX;
    LinkedListNode* currcol;
    LinkedListNode* iter = head->right;
    vector<LinkedListNode*> rows, cols;
    while (iter != head){
        if (iter->val < min){
            min = iter->val;
            currcol = iter;
        }
        iter = iter->right;
    }
    if (min == 0){
        return false;
    } else if (min == INT32_MAX){
        solutions++;
        return true;
    }
    iter = currcol->down;
    vector<LinkedListNode*> vals = {};
    while (iter != currcol){
        vals.push_back(iter);
        iter = iter->down;
    }
    shuffle(vals.begin(), vals.end(), rng);
    for (auto& i: vals){
        iter = i;
        solution.push_back(i);
        vector<LinkedListNode*> deleted;
        while (iter->right != i){
            unlink(iter);
            deleted.push_back(iter);
            iter = iter->right;
        }
        unlink(iter);
        deleted.push_back(iter);
        if (solve(head, solution, size, rng, solutions)){
            if (solutions >= 2){
                for (int j = deleted.size()-1; j >= 0; j--){
                    relink(deleted[j]);
                }
                return true;
            }
        }
        for (int j = deleted.size()-1; j >= 0; j--){
            relink(deleted[j]);
        }
        solution.pop_back();
    }
    return false;
}

template <typename T>

vector<LinkedListNode*> puzzlify(LinkedListNode* head, vector<LinkedListNode*> scrambled, int size, T rng){
    for (auto& i: scrambled){
        LinkedListNode* temp = scrambled.back();
        scrambled.pop_back();
        if (!remove_selected_nodes(head, scrambled, size, rng)){
            if (scrambled.size() > 30){
                scrambled.insert(scrambled.begin(), temp);
            } else {
                scrambled.push_back(temp);
                return scrambled;
            }
        }
    }
    return scrambled;
}

template<typename T>

bool remove_selected_nodes(LinkedListNode* head, vector<LinkedListNode*> nodes, int size, T rng){
    vector<LinkedListNode*> deleted;
    vector<LinkedListNode*> solution;
    for (auto& i: nodes){
        solution.push_back(i);
        LinkedListNode* rowiter = i;
        while (rowiter->right != i){
            unlink(rowiter);
            deleted.push_back(rowiter);
            rowiter = rowiter->right;
        }
        unlink(rowiter);
        deleted.push_back(rowiter);
    }
    
    int solutions = 0;
    if (solve(head, solution, size, rng, solutions)){
        if (solutions >= 2){
            for (int i = deleted.size()-1; i >= 0; i--){
                relink(deleted[i]);
            }
            return false;
        }
    }
    for (int i = deleted.size()-1; i >= 0; i--){
        relink(deleted[i]);
    }
    return true;
}

void pretty_print(vector<vector<int>> board, int size){
    for (int i = 0; i < board.size(); i++){
        if ((i)%(int(sqrt(size))) == 0){
            for (int j = 0; j < size; j++){
                if (j == 0){
                    if (i == 0){
                        cout << "┌--";
                    } else {
                        cout << "├--";
                    }
                } else if (j == size-1){
                    if (i == 0){
                        cout << "-┐";
                    } else {
                        cout << "-┤";
                    }
                }
                else if ((j+1)%(int(sqrt(size))) == 0){
                    if (i == 0){
                        cout << "-┬";
                    }
                    else {
                        cout << "-+";
                    }
                } else {
                    cout << "--";
                }
            }
            cout << "\n";
        }
        for (int j = 0; j < board[i].size(); j++){
            if (j == 0){
                cout << "|";
            }
            if (board[i][j] == 0){
                cout << " ";
            }
            else {
                cout << board[i][j];
            }
            if ((j+1)%(int(sqrt(size))) == 0){
                cout << "|";
            } else {
                cout << " ";
            }
        }
        cout << "\n";
    }
    for (int i = 0; i < size; i++){
        // cout << ((i+1)%size) << " ASDF\n";
        if (i == 0){
            cout << "└-";
        }
        else if (i == size-1){
            cout << "--┘";
        } 
        else if ((i)%int(sqrt(size)) == 0){
            cout << "┴-";
        } else {
            cout << "--";
        }
    }
    cout << "\n";
}

int main(){
    auto start = chrono::high_resolution_clock::now();
    int size = 9;
    auto rng = std::mt19937 {std::random_device{}()};
    LinkedListNode* head = get_matrix(size);
    vector<LinkedListNode*> ans;
    int solutions;
    bool a = solve(head, ans, size, rng, solutions);
    shuffle(ans.begin(), ans.end(), rng);
    ans = puzzlify(head, ans, size, rng);
    sort(ans.begin(), ans.end());
    vector<int> finans;
    for (auto& i: ans){
        finans.push_back(i->row);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "\n\n" << ans.size() << "\n\n";
    vector<vector<int>> puzzle(size, vector<int>(size, 0));
    sort(finans.begin(), finans.end(), greater<int>());
    for (auto& i: finans){
        puzzle[(((i-1)/size)/size)][(((i-1)/size))%size] = (i-1)%size+1;
    }
    // Normal print
    // for (int i = 0; i < size; i++){
    //     for (int j = 0; j < size; j++){
    //         if (!finans.empty() && (finans.back()-1)/size == i*size+j){
    //             cout << (finans.back()-1)%size+1 << " ";
    //             finans.pop_back();
    //         } else {
    //             cout << "0 ";
    //         }
    //     }
    //     cout << "\n";
    // }
    pretty_print(puzzle, size);
    cout << "Took " << duration.count() << " milliseconds to execute\n";
    return 0;
}
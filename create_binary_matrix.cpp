#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <unordered_map>
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

bool solve(LinkedListNode* head, vector<LinkedListNode*>& solution, int size, T rng, int& solutions){ // vector<int>
    if (!head || !head->right) return true;
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
    vector<int> ans_nodes;
    for (auto& i: scrambled){
        // ans_nodes.push_back(i);
        LinkedListNode* temp = scrambled.back();
        scrambled.pop_back();
        if (!remove_selected_nodes(head, scrambled, size, rng)){
            // ans_nodes.pop_back();
            scrambled.push_back(temp);
            return scrambled;
        }
    }
    return scrambled;
    // return ans_nodes;
}

template<typename T>

bool remove_selected_nodes(LinkedListNode* head, vector<LinkedListNode*> nodes, int size, T rng){
    // sort(nodes.begin(), nodes.end(), greater<int>());
    vector<LinkedListNode*> deleted;
    // vector<LinkedListNode*> todelete;
    // for (int i = 0; i < nodes.size(); i++){
    //     LinkedListNode *iter = head->right;
    //     for (int j = 0; j < nodes[i]; j++){
    //         iter = iter->down;
    //     }
    //     todelete.push_back(iter);
    // }
    for (auto& i: nodes){
        LinkedListNode* rowiter = i;
        while (rowiter->right != i){
            unlink(rowiter);
            deleted.push_back(rowiter);
            rowiter = rowiter->right;
        }
        unlink(rowiter);
        deleted.push_back(rowiter);
    }
    // for (int i = 0; i < nodes.size(); i++){
    //     LinkedListNode* rowiter, *iter = head->right;
    //     for (int j = 0; j < nodes[i]; j++){
    //         iter = iter->down;
    //     }
    //     // iter = i;
    //     // solution.push_back(i->row);
    //     rowiter = iter;
    //     while (rowiter->right != iter){
    //         unlink(rowiter);
    //         deleted.push_back(rowiter);
    //         rowiter = rowiter->right;
    //     }
    //     unlink(rowiter);
    //     deleted.push_back(rowiter);
    //     // for (int j = deleted.size()-1; j >= 0; j--){
    //     //     relink(deleted[j]);
            
    //     // }
    //     // solution.pop_back();
    // }
    vector<LinkedListNode*> solution;
    int solutions = 0;
    if (solve(head, solution, size, rng, solutions)){
        if (solutions >= 2){
            for (auto& i: deleted){
                relink(i);
            }
            return false;
        }
    }
    for (auto& i: deleted){
        relink(i);
    }
    return true;
}

vector<int> uniform_distribution(vector<vector<int>> board, int size){
    vector<int> col_constraints(size);
    vector<int> curr_constrains = col_constraints;
    iota(col_constraints.begin(), col_constraints.end(), 1);
    for (int i = 0; i < size; i++){
        int choice = rand() % curr_constrains.size();
        // choice = size*(choice/size);
        int num = curr_constrains[choice];
        col_constraints.erase(find(col_constraints.begin(), col_constraints.end(), choice));
        choice = choice - choice%size;
        for (int i = )
        curr_constrains.erase(find(col_constraints.begin(), col_constraints.end(), choice), find(col_constraints.begin(), col_constraints.end(), choice)+sqrt(size)-1);
        if (i%(int(sqrt(size))) == 0){
            curr_constrains = col_constraints;
        }
    }
}

int main(){
    int size = 4;
    // cin >> size;
    auto rng = std::mt19937 {std::random_device{}()};
    LinkedListNode* head = get_matrix(size);
    vector<LinkedListNode*> ans;
    int solutions;
    bool a = solve(head, ans, size, rng, solutions); // LinkedListNode* iter, *bad_node
    sort(ans.begin(), ans.end());
    cout << ans.size() << " sizeof\n";
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            cout << (ans[size*i+j]->row-1)%size + 1 << " ";
        }
        cout << "\n";
    }
    shuffle(ans.begin(), ans.end(), rng);
    // head = get_matrix(size);

    ans = puzzlify(head, ans, size, rng);
    sort(ans.begin(), ans.end());
    cout << ans.size() << " sizeof\n";
    vector<int> finans;
    for (auto& i: ans){
        finans.push_back(i->row);
    }
    sort(finans.begin(), finans.end(), greater<int>());
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            // cout << finans.back() << " " << finans.back()/size << " " << i*size+j << "\n";
            if (!finans.empty() && (finans.back()-1)/size == i*size+j){
                cout << (finans.back()-1)%size+1 << " ";
                finans.pop_back();
            } else {
                cout << "0 ";
            }
        }
        cout << "\n";
    }
    // for (int i = 0; i < size*size; i++){
    //     // cout << finans.back() << " " << (finans.back()-1)/size << "\n";
    //     if (i%size == 0){
    //         cout << "\n";
    //     }
    //     if (i == (finans.back()-1)/size){
    //         cout << (finans.back()-1)%size+1 << " "; // %size + 1
    //         finans.pop_back();
    //     } else {
    //         cout << "0 ";
    //     }
    // }
    // cout << "\n";
}
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <cctype>
#include <direct.h>   // for _getcwd()
using namespace std;
// STRUCT: Singly Linked List Node
struct Node {
    string word;
    Node* next;
};
// CLASS: Binary Search Tree (BST)
class BST {
private:
    struct TreeNode {
        string word;
        TreeNode* left;
        TreeNode* right;
    };

    TreeNode* root;

    TreeNode* insert(TreeNode* node, const string& word) {
        if (!node) {
            TreeNode* newNode = new TreeNode();
            newNode->word = word;
            newNode->left = newNode->right = nullptr;
            return newNode;
        }
        if (word < node->word)
            node->left = insert(node->left, word);
        else if (word > node->word)
            node->right = insert(node->right, word);
        return node;
    }

    bool search(TreeNode* node, const string& word) {
        if (!node) return false;
        if (node->word == word) return true;
        if (word < node->word) return search(node->left, word);
        return search(node->right, word);
    }

public:
    BST() { root = nullptr; }

    void insert(const string& word) {
        root = insert(root, word);
    }

    bool search(const string& word) {
        return search(root, word);
    }
};
//  singly Linked List FUNCTIONS
void insertWord(Node*& head, const string& word) {
    Node* newNode = new Node();
    newNode->word = word;
    newNode->next = nullptr;

    if (!head) {
        head = newNode;
        return;
    }

    Node* temp = head;
    while (temp->next != nullptr)
        temp = temp->next;

    temp->next = newNode;
}

void bubbleSortList(Node* head) {
    if (!head) return;

    bool swapped;
    do {
        swapped = false;
        Node* current = head;
        while (current->next != nullptr) {
            if (current->word > current->next->word) {
                swap(current->word, current->next->word);
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
}

bool searchWord(Node* head, const string& word) {
    Node* current = head;
    while (current != nullptr) {
        if (current->word == word)
            return true;
        current = current->next;
    }
    return false;
}
// Load Dictionary ? Linked List + BST
void loadDictionary(Node*& head, BST& bst, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open dictionary file.\n";
        return;
    }

    string word;
    while (getline(file, word)) {
        for (char& c : word) c = tolower(c);
        insertWord(head, word);
        bst.insert(word);     // <-- BST INSERT HERE
    }

    bubbleSortList(head);

    cout << "Dictionary loaded into Linked List + BST.\n";
} 
//stack
// SPELLING CHECK (Uses BOTH Linked List & BST searches)
void generateSuggestions(Node* head, const string& word, stack<string>& suggestions) {

    // 1. Deletion (already existed)
    
    for (size_t i = 0; i < word.length(); i++) {
        string candidate = word.substr(0, i) + word.substr(i + 1);
        if (searchWord(head, candidate))
            suggestions.push(candidate);
    }

    // 2. Insertion (add a-z at every position)
    
    for (size_t i = 0; i <= word.length(); i++) {
        for (char c = 'a'; c <= 'z'; c++) {
            string candidate = word.substr(0, i) + c + word.substr(i);
            if (searchWord(head, candidate))
                suggestions.push(candidate);
        }
    }

    // 3. Replacement (replace each char with a-z)
    
    for (size_t i = 0; i < word.length(); i++) {
        for (char c = 'a'; c <= 'z'; c++) {
            if (c == word[i]) continue; // skip if same
            string candidate = word.substr(0, i) + c + word.substr(i + 1);
            if (searchWord(head, candidate))
                suggestions.push(candidate);
        }
    }
}

//queue
void checkSpelling(Node* head, BST& bst) {
    cout << "Enter a sentence to check: ";
    string input;
    getline(cin, input);

    queue<string> wordQueue;
    string word = "";

    for (char c : input) {
        if (isspace(c)) {
            if (!word.empty()) {
                for (char& ch : word) ch = tolower(ch);
                wordQueue.push(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        for (char& ch : word) ch = tolower(ch);
        wordQueue.push(word);
    }

    while (!wordQueue.empty()) {
        string w = wordQueue.front();
        wordQueue.pop();

        // Check using BST (fast)
        if (bst.search(w)) {
            cout << "'" << w << "' is spelled correctly (BST).\n";
        }
        else {
            cout << "'" << w << "' is MISSPELLED.\n";
            stack<string> suggestions;

            generateSuggestions(head, w, suggestions);

            if (suggestions.empty())
                cout << "  No suggestions.\n";
            else {
                while (!suggestions.empty()) {
                    cout << "  " << suggestions.top() << endl;
                    suggestions.pop();
                }
            }
        }
    }
}

void freeList(Node* head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}
// MAIN PROGRAM
int main() {
	
	char cwd[512];
    cout << "Program is running from: " << _getcwd(cwd, 512) << endl;

    Node* dictionaryHead = nullptr;
    BST dictionaryBST;

    int choice;
    do {
        cout << "\n--- Spelling Checker Menu ---\n";
        cout << "1. Load Dictionary\n";
        cout << "2. Check Spelling\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                loadDictionary(dictionaryHead, dictionaryBST, "dictionary.txt");
                break;

            case 2:
                if (!dictionaryHead)
                    cout << "Load dictionary first.\n";
                else
                    checkSpelling(dictionaryHead, dictionaryBST);
                break;

            case 3:
                cout << "Goodbye!\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 3);

    freeList(dictionaryHead);
    return 0;
}
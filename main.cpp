#include <windows.h>
#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

struct Node 
{
    char data;
    bool end;
    vector<Node*> children;
};

Node* create(char c) 
{
    Node* newNode = new Node;
    newNode->data = c;
    newNode->end = false;
    return newNode;
}

Node* child_locator(Node* current, char c) 
{
    for (int i = 0; i < current->children.size(); i++) 
    { 
        Node* temp1 = current->children[i];
        if (temp1->data == c) 
        {
            return temp1;
        }
    }
    return NULL;
}

void insert(Node* root, string s) // Time Complexity: O(L), L = Length of the string
{
    Node* current = root;
    if (s.length() == 0) 
    {
        current->end = true; // an empty word
        return;
    }
    for (int i = 0; i < s.length(); i++) 
    {
        Node* child = child_locator(current, s[i]);
        if (child != NULL) 
        {
            current = child;
        }
        else 
        {
            Node* tmp = create(s[i]);
            current->children.push_back(tmp);
            current = tmp;
        }
        if (i == s.length() - 1)
            current->end = true;
    }
}

bool word_search(Node* root, string s) // Time Complexity: O(L), L = Length of the string
{
    Node* current = root;
    while (current != NULL) 
    {
        for (int i = 0; i < s.length(); i++) 
        {
            Node* temp1 = child_locator(current, s[i]);
            if (temp1 == NULL)
                return false;
            current = temp1;
        }
        if (current->end)
            return true;
        else
            return false;
    }
    return false;
}

void TRIE_Traversal(Node* current, char* s, vector<string>& res, bool& loop) // Time Complexity: O(k*h),
{                                                                            //  k = number of suggestions
    char k[100] = { 0 };                                                     // h = Height of the tree
    char aa[2] = { 0 };
    if (loop) 
    {
        if (current != NULL) 
        {
            if (current->end == true) 
            {
                res.push_back(s);
                if (res.size() >= 100)
                    loop = false;
            }
            vector<Node*> child = current->children;
            for (int j = 0; j < child.size() && loop; j++) 
            {
                strcpy(k, s);
                aa[0] = child[j]->data;
                aa[1] = '\0';
                strcat(k, aa);
                if (loop)
                {
                    TRIE_Traversal(child[j], k, res, loop);
                }
                    
            }
        }
    }
}

void auto_start(Node* root, string s, vector<string>& res) 
{
    Node* current = root;
    for (int i = 0; i < s.length(); i++) 
    {
        Node* tmp = child_locator(current, s[i]);
        if (tmp == NULL)
            return;
        current = tmp;                 
    }
    char c[100];
    strcpy(c, s.c_str());
    bool loop = true;
    TRIE_Traversal(current, c, res, loop);
    return;
}

void Dictionary_loading(Node* root, string fn) // Time Complexity: O(n*m)
{                                              // n = Number of words in the dictionary
    ifstream dictionary;                       // m = Average lenght of all the words in the dictionary
    dictionary.open(fn.c_str());
    if (!dictionary.is_open()) 
    {
        MessageBox(NULL, "Could not open dictionary file", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    while (!dictionary.eof()) 
    {
        char s[100];
        dictionary >> s;
        insert(root, s);
    }
    dictionary.close();
    return;
}

HWND hInput, hList, hSpellInput;
Node* root;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static vector<string> listOfWords;
    static bool loop = true;
    
    switch (uMsg) {
        case WM_CREATE:
            CreateWindow("BUTTON", "Auto Complete", WS_VISIBLE | WS_CHILD, 10, 10, 150, 30, hwnd, (HMENU)1, NULL, NULL);
            CreateWindow("BUTTON", "Spell Check", WS_VISIBLE | WS_CHILD, 10, 50, 150, 30, hwnd, (HMENU)2, NULL, NULL);
            CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD, 10, 90, 150, 30, hwnd, (HMENU)3, NULL, NULL);
            hInput = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 170, 10, 200, 30, hwnd, NULL, NULL, NULL);
            hSpellInput = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 170, 50, 200, 30, hwnd, NULL, NULL, NULL);
            hList = CreateWindow("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 170, 90, 200, 100, hwnd, NULL, NULL, NULL);
            break;
        
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1: // Auto Complete
                {
                    char buffer[100];
                    GetWindowText(hInput, buffer, 100);
                    string prefix(buffer);
                    listOfWords.clear();
                    auto_start(root, prefix, listOfWords);
                    SendMessage(hList, LB_RESETCONTENT, 0, 0);
                    for (const auto& word : listOfWords) {
                        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)word.c_str());
                    }
                    break;
                }
                
                case 2: // Spell Check
                {
                    char buffer[100];
                    GetWindowText(hSpellInput, buffer, 100);
                    string word(buffer);
                    bool result = word_search(root, word);
                    MessageBox(hwnd, result ? "The entered word is correct!" : "The entered word is incorrect!", "Spell Check", MB_OK);
                    break;
                }
                
                case 3: // Exit
                    PostQuitMessage(0);
                    break;
            }
            break;
        
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Auto Complete and Spell Check",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 250,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    root = create(' ');
    Dictionary_loading(root, "dictionary.txt");

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete root;
    return 0;
}

#include<iostream>
#include<fstream>
#include<string.h>
#include<unordered_map>
#include<vector>
#include<stack>

using namespace std;

static int nodeId = 1;

class Node {
    public:
        int id;
        bool isFinal;
        unordered_map<char,vector<Node*>> next;

        Node()
        {
            id = nodeId;
            isFinal = false;
            nodeId++;
        }

        void AddTransition(char inp, Node* n)
        {
            next[inp].push_back(n);
        }
};

class NFA {
    public:
    Node* start;
    Node* end;
    unordered_map<int, Node*> nodes;

    NFA()
    {
        start = new Node();
        nodes[start->id] = start;
        end = start;
    }

    void AddTransition(int id, char inp, Node* n)
    {
        nodes[n->id] = n;
        nodes[id]->AddTransition(inp, n);
        end = n;
    }

    // Add an epsilon transition from end to start
    void AddPlusTransition()
    {
        nodes[end->id]->AddTransition('e', start);
    }

    // Add an epsilon transition from start to end and another from end to start
    void AddStarTransition()
    {
        nodes[start->id]->AddTransition('e', end);
        nodes[end->id]->AddTransition('e', start);
    }

    // Merge this nfa and the provided NFA
    // Add a transition from the end of this nfa to the start of the other nfa
    void Merge(NFA* nfa)
    {
        nodes[end->id]->AddTransition('e', nfa->nodes[nfa->start->id]);
        for(unordered_map<int, Node*>::iterator it = nfa->nodes.begin(); it != nfa->nodes.end(); ++it)
        {
            nodes[it->first] = it->second;
        }
        end = nfa->end;
        // TODO: Delete reference to nfa
    }

    void PrintNFA()
    {
        cout << "Start: " << start->id << " " << "End: " << end->id << endl;

        for(unordered_map<int, Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            cout << it->first << endl;
            for(unordered_map<char,vector<Node*>>::iterator it1 = it->second->next.begin(); it1 != it->second->next.end(); ++it1)
            {
                cout << it1->first << ": ";
                for(Node* n : it1->second)
                {
                    cout << n->id << " ";
                }
                cout << endl;
            }
            cout << endl;
            
        }
    }
};

int main()
{
    ifstream inputFile("input.txt");
    string r,w, temp;
    inputFile >> r >> w;

    stack<NFA*> nfaStack;
    NFA* currentNFA = NULL;

    for(char c: r)
    {
        switch(c)
        {
            case '(': 
                if(currentNFA != NULL)
                    nfaStack.push(currentNFA);
                currentNFA = new NFA();

            break;

            case ')':
                if(currentNFA != NULL)
                {
                    if(nfaStack.size() == 0)
                        break;

                    nfaStack.top()->Merge(currentNFA);
                    currentNFA = nfaStack.top();
                    nfaStack.pop();
                }
                else
                {
                    cout << "Invalid Regex" << endl;
                    return -1;
                }
            break;

            case '+':
                currentNFA->AddPlusTransition();
            break;

            case '*':
                currentNFA->AddStarTransition();
            break;

            default:
                currentNFA->AddTransition(currentNFA->end->id, c, new Node());
        }
    }
    currentNFA->PrintNFA();
} 
#include<iostream>
#include<fstream>
#include<string.h>
#include<unordered_map>
#include<vector>
#include<stack>
#include<queue>

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
    bool hasOrNode;

    NFA()
    {
        start = new Node();
        nodes[start->id] = start;
        end = start;
        hasOrNode = false;
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
        // If the is being ORed
        if(nfa->hasOrNode)
        {
            Node* commonNode = new Node();
            // Add a new node to the end of the given NFA
            nfa->AddTransition(nfa->end->id, 'e', commonNode);
            // Add an epsilon transtiton from the start of the current NFA to the start of the given NFA
            nodes[start->id]->AddTransition('e', nfa->start);
            // Add an epsilon transition from the end of the current NFA to the new end of the given NFA
            nodes[end->id]->AddTransition('e', nfa->end);
        }

        // Add epsilon transition from end of current NFA to start of given NFA if the NFA is not being ORed
        if(!nfa->hasOrNode)
            nodes[end->id]->AddTransition('e', nfa->nodes[nfa->start->id]);
        // Add the nodes of given NFA to current NFA's list
        for(unordered_map<int, Node*>::iterator it = nfa->nodes.begin(); it != nfa->nodes.end(); ++it)
        {
            nodes[it->first] = it->second;
        }

        end = nfa->end;
        
        // Delete reference to nfa
        delete nfa;
    }

    bool Traverse(string input)
    {
        int depth = 0;
        queue<Node*> nodeQueue;
        nodeQueue.p
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

NFA* GenerateNFAWithEpsilon(string regex)
{
    stack<NFA*> nfaStack;
    NFA* currentNFA = NULL;
    bool shouldOr = false;

    for(char c: regex)
    {
        switch(c)
        {
            // Create a new independent NFA when '(' is encountered
            case '(': 
                if(currentNFA != NULL)
                    nfaStack.push(currentNFA);
                currentNFA = new NFA();
                if(shouldOr)
                {
                    shouldOr = false;
                    currentNFA->hasOrNode = true;
                }

            break;
            // Merge the NFA on top of the stack with the current NFA when ')' is encountered
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
                    return NULL;
                }
            break;
            // Add a Plus Transition to the current NFA when '*' is encountered
            case '+':
                currentNFA->AddPlusTransition();
            break;
            // Add a Star Transition to the current NFA when '-' is encountered
            case '*':
                currentNFA->AddStarTransition();
            break;
            // Mark the next NFA as being ORed
            case '|':
                shouldOr = true;
                break;
            // When any other char is encountered it must be a literal so add a transition to the current NFA
            default:
                currentNFA->AddTransition(currentNFA->end->id, c, new Node());
        }
    }
    return currentNFA;
}

int main()
{
    // Get the input from file
    ifstream inputFile("input.txt");
    string regex,w, temp;
    inputFile >> regex >> w;

    // Generate the NFA for the regex
    NFA* regexNFA = GenerateNFAWithEpsilon(regex);
    if(regexNFA == NULL)
    {
        cout << "Invalid Regex" << endl;
        return -1;
    }

    regexNFA->PrintNFA();
}
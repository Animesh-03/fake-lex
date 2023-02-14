#include<iostream>
#include<fstream>
#include<string.h>
#include<unordered_map>
#include<vector>
#include<stack>
#include<queue>
#include<set>

using namespace std;

static int nodeId = 1;

class Node;

set<Node*> Union(set<Node*> a, set<Node*> b);

class Node {
    public:
        int id;
        bool isFinal;
        unordered_map<char,vector<Node*>> next;
        set<Node*> epClosure;

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

        int AddClosureToQueue(deque<Node*>& q, char c)
        {
            int n = 0;
            for(Node* closure : next[c])
            {
                q.push_front(closure);
                n++;
            }
            return n;
        }

        int AddEpsilonClosureToQueue(deque<Node*>& q)
        {
            int num = 0;
            for(Node* n : epClosure)
            {
                q.push_back(n);
                num++;
            }
            return num;
        }

        // No idea why this works
        set<Node*> FindEpsilonClosure(Node* orig, set<Node*>& origSet)
        {            
            for(Node* node : next['e'])
            {
                if(epClosure.find(node) != epClosure.end())
                    continue;
                epClosure.insert(node);
                epClosure = Union(node->FindEpsilonClosure(orig, origSet), epClosure);
            }
            return epClosure;
        }
};

set<Node*> Union(set<Node*> a, set<Node*> b)
{
    set<Node*> u = a;
    u.insert(b.begin(), b.end());
    return u;
}

bool QueueHasFinalState(deque<Node*> q)
{
    // cout << "\nPrinting Final Reachability\n";
    int size = q.size();
    while(size--)
    {
        // cout << q.front()->id << " ";
        if(q.front()->isFinal)
            return true;

        for(Node* n : q.front()->epClosure)
        {
            // cout << n->id << " ";
            if(n->isFinal)
                return true;
        }
        
        auto front = q.front();
        q.pop_front();
        q.push_back(front);
    }
    return false;
}

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

    void FindClosures()
    {
        for(unordered_map<int, Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            nodes[it->first]->FindEpsilonClosure(it->second, it->second->epClosure);
        }
    }

    bool Traverse(string input)
    {
        int depth = 0, i = 0;
        deque<Node*> nodeQueue, nodeQueue1;
        nodeQueue.push_back(start);

        while(i < input.length() && depth < 1000 && !nodeQueue.empty())
        {
            int size = nodeQueue.size();
            // First get all the epsilon closures and store them in q1
            while(size--)
            {
                Node* first = nodeQueue.front();
                nodeQueue.pop_front();
                int n = first->AddEpsilonClosureToQueue(nodeQueue1);
                // Then for each node in q1 use a symbol in the string to get the next nodes
                while(n--)
                {
                    Node* second = nodeQueue1.front();
                    nodeQueue1.pop_front();
                    second->AddClosureToQueue(nodeQueue, input[i]);
                }
                depth++;
            }
            i++;
        }
        // If the nodes remaining in the queue has a final node or if one of the nodes has an epsilon closure containing final state then return true
        if(QueueHasFinalState(nodeQueue))
            return true;
        else
            return false;
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

    void PrintEpsilonClosure()
    {
        for(unordered_map<int, Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            cout << it->first << ": ";
            for(Node* n: it->second->epClosure)
            {
                cout << n->id << " ";
            }
            cout << endl << endl;
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
    currentNFA->end->isFinal = true;
    currentNFA->FindClosures();
    return currentNFA;
}

void Simulate(NFA* nfa, string w)
{
    int left = 0, right = w.length();
    string ans = "";
    
    while(left < w.length())
    {
        string s = w.substr(left, right - left);
        if(nfa->Traverse(s))
        {
            ans += "$" + s;
            left = right;
            right = w.length();
        }
        else
        {
            right--;
        }

        if(left > right)
        {
            right = w.length();
            ans += "@";
            ans += w[left];
            left++;
        }
    }

    cout << ans << "#" << endl;
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

    // Test the input against the NFA
    cout << regexNFA->Traverse("abab") << endl;

    Simulate(regexNFA, w);
}
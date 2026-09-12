#include <iostream>
using namespace std;
class Stack
{
    public:
        void push(int x);
        int pop();
        bool isEmpty();
        Stack(); // Конструктор
        ~Stack();   // Деструктор

    private:
        struct Node
        {
            int value;
            Node* next;
        }
        struct Node* top;

    void Stack::Push(int val)
    {
        Node* n = new Node;
        n->value = val;
        n->next = top;
        top = n; 
    }

    int Stack::pop()
    {
        if(isEmpty()) {return -1;}
        int temp = top->val;
        Node*pv = top;
        top = top->next;
        delete pv;
        return temp;
    }

    bool Stack::isEmpty()
    {
        return top == NULL;
    }
    void Stack::Stack()
    {
        top == NULL;
    }
    void Stack::~Stack()
    {
        while(top)
        {
            Node*pv = top;
            top = top->next;
            delete pv;
        }   
        
    }
}
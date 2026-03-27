// Generic implementation of Stack

#include<iostream>
#include<string.h>

using namespace std;

template<class T>
struct Stacknode
{
    T data;
    struct Stacknode<T> *next;
};

template<class T>
class Stack
{
    private:
        struct Stacknode<T> *first;
        int iCount;

    public:
        Stack();
        void Push(T);
        T Pop();
        void Display();
        int Count();
};

template<class T>
Stack<T> :: Stack()
{
    first = NULL;
    iCount = 0;
}

template<class T>
void Stack<T> :: Push(T no) // InsertFirst
{
    struct Stacknode<T> * newn = NULL;

    newn = new Stacknode<T>;

    newn->data = no;
    newn->next = NULL;

    newn->next = first;
    first = newn;

    iCount++;
}

template<class T>
T Stack<T> :: Pop()         // DeleteFirst
{
    struct Stacknode<T> *temp = NULL;
    T value;

    if(first == NULL)
    {
        cout<<"Unable to pop as stack is empty\n";
        return NULL;
    }
    else
    {
        temp = first;
        value = temp->data;

        first = first -> next;
        delete temp;

        iCount--;

        return value;
    }
}

template<class T>
void Stack<T> :: Display()
{
    struct Stacknode<T> *temp = NULL;

    temp = first;

    while(temp != NULL)
    {
        cout<<"| "<<temp->data<<" |-";
        temp = temp->next;
    }
    cout<<"\n";
}

template<class T>
int Stack<T> :: Count()
{
    return iCount;
}

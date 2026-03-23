#include<iostream>
using namespace std;

template<class T>
struct node
{
    T data;
    node<T>* next;
};

template<class T>
class SinglyLL
{
private:
    node<T>* first;
    int iCount;

public:
    SinglyLL()
    {
        first = NULL;
        iCount = 0;
    }

    void InsertFirst(T no)
    {
        node<T>* newn = new node<T>;
        newn->data = no;
        newn->next = NULL;

        if(first == NULL)
        {
            first = newn;
        }
        else
        {
            newn->next = first;
            first = newn;
        }
        iCount++;
    }

    void Display()
    {
        node<T>* temp = first;
        while(temp != NULL)
        {
            cout << temp->data << " -> ";
            temp = temp->next;
        }
        cout << "NULL\n";
    }

    int Count()
    {
        return iCount;
    }
};

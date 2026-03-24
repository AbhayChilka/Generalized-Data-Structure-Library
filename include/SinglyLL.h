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
    }

    void InsertLast(T no)
    {
        node<T>* newn = new node<T>;
        newn->data = no;
        newn->next = NULL;

        node<T>* temp = NULL;

        if(first == NULL)
        {
            first = newn;
        }
        else
        {   
            temp = first;
    
            while(temp->next != NULL)
            {
                temp = temp ->next;
            }
            temp -> next = newn;
        }
    }

    void DeleteFirst()
    {
        node<T>* temp = NULL;

        if(first == NULL)
        {
            return;
        }
        else if(first->next == NULL)
        {
            delete first;

            first = NULL;   
        }
        else
        {
            temp = first;
            first = first -> next;
         
            delete temp;
        }
    }

    void DeleteLast()
    {
        node<T>* temp = NULL;

        if(first == NULL)
        {
            return;
        }
        else if(first->next == NULL)
        {
            delete first;

            first = NULL;   
        }
        else
        {
            temp = first;
           
            while(temp->next->next != NULL)
            {
                temp = temp -> next;
            }

            delete(temp->next);
            temp->next = NULL;
        }
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
        int iCnt = 0;
        node<T>* temp = first;   

        while(temp != NULL)
        {
            iCnt++;
            temp = temp->next; 
        }
        return iCnt;    
    }
};


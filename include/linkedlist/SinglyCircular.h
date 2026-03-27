#include<iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////
//
//  Class :         SinglyCircular
//  Description :   Generic implementation of Singly Circular
//
///////////////////////////////////////////////////////////////////////

template <class T>
struct SinglyCLL
{
    T data;
    struct SinglyCLL<T> *next;
};

template <class T>
class SinglyCircular
{
    private:
        struct SinglyCLL<T> * first;
        struct SinglyCLL<T> * last;
        int iCount;

    public:
        SinglyCircular();

        void Display();
        int Count();

        void InsertFirst(T no);
        void InsertLast(T no);
        void InsertAtPos(T no, int pos);

        void DeleteFirst();
        void DeleteLast();
        void DeleteAtPos(int pos);
};

template <class T>
SinglyCircular<T>::SinglyCircular()
{
    cout<<"Inside constructor of SinglyCircular\n";

    this->first = NULL;
    this->last = NULL;
    this->iCount = 0;
}

template <class T>
void SinglyCircular<T>::InsertFirst(T no)
{
    struct SinglyCLL<T> * newn = NULL;

    newn = new struct SinglyCLL<T>;

    newn->data = no;
    newn->next = NULL;

    if(first == NULL && last == NULL)   // if(iCount == 0)
    {
        first = newn;
        last = newn;

        last->next = first;
    }
    else
    {
        newn->next = first;
        first = newn;

        last->next = first;
    }

    iCount++;
}

template <class T>
void SinglyCircular<T>::InsertLast(T no)
{
    struct SinglyCLL<T> * newn = NULL;

    newn = new struct SinglyCLL<T>;

    newn->data = no;
    newn->next = NULL;

    if(first == NULL && last == NULL)   // if(iCount == 0)
    {
        first = newn;
        last = newn;

        last->next = first;
    }
    else
    {
        last->next = newn;
        last = newn;

        last->next = first;
    }

    iCount++;
}

template <class T>
void SinglyCircular<T>::Display()
{
    if(first == NULL && last == NULL)
    {
        return;
    }

    struct SinglyCLL<T> * temp = first;

    do
    {
        cout<<"| "<<temp->data<<" | -> ";
        temp = temp -> next;
    } while (temp != last ->next);
    
    cout<<"\n";
}

template <class T>
int SinglyCircular<T>::Count()
{
    return iCount;
}

template <class T>
void SinglyCircular<T>::DeleteFirst()
{
    if(first == NULL && last == NULL)
    {
        return;
    }
    else if(first == last)
    {
        delete first;

        first = NULL;
        last = NULL;
    }
    else
    {
        first = first -> next;
        delete last->next;

        last->next = first;
    }
    iCount--;
}

template <class T>
void SinglyCircular<T>::DeleteLast()
{
    struct SinglyCLL<T> * temp = NULL;

    if(first == NULL && last == NULL)
    {
        return;
    }
    else if(first == last)
    {
        delete first;

        first = NULL;
        last = NULL;
    }
    else
    {
        temp = first;

        while(temp -> next != last)
        {
            temp = temp -> next;
        }

        delete last;
        last = temp;

        last->next = first;
    }
    iCount--;
}

template <class T>
void SinglyCircular<T>::InsertAtPos(T no, int pos)
{
    struct SinglyCLL<T> * temp = NULL;
    struct SinglyCLL<T> * newn = NULL;

    int i = 0;

    if(pos < 1 || pos > iCount+1)
    {
        cout<<"Invalid position\n";
        return;
    }

    if(pos == 1)
    {
        InsertFirst(no);
    }
    else if(pos == iCount+1)
    {
        InsertLast(no);
    }
    else
    {
        newn = new struct SinglyCLL<T>;

        newn->data = no;
        newn->next = NULL;

        temp = first;

        for(i = 1; i < pos -1; i++)
        {
            temp = temp -> next;
        }

        newn->next = temp -> next;
        temp->next = newn;

        iCount++;
    }
}

template <class T>
void SinglyCircular<T>::DeleteAtPos(int pos)
{
    struct SinglyCLL<T> * temp = NULL;
    struct SinglyCLL<T> * target = NULL;
    
    int i = 0;
    
    if(pos < 1 || pos > iCount)
    {
        cout<<"Invalid position\n";
        return;
    }

    if(pos == 1)
    {
        DeleteFirst();
    }
    else if(pos == iCount)
    {
        DeleteLast();
    }
    else
    {
        temp = first;

        for(i = 1; i < pos-1; i++)
        {
            temp = temp -> next;
        }

        target = temp -> next;

        temp->next = target->next;
        delete target;

        iCount--;
    }
}


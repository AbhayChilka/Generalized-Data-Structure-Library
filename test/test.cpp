#include "SinglyLL.h"

int main()
{
    int cnt = 0;

    SinglyLL<int> obj;

    obj.InsertFirst(20);
    obj.InsertFirst(10);

    obj.Display();

    cnt = obj.Count();
    cout<< "Total elemets are : "<<cnt<<endl;

    obj.InsertLast(30);
    obj.Display();

    cnt = obj.Count();
    cout<< "Total elemets are : "<<cnt<<endl;

    obj.DeleteFirst();
    obj.Display();

    cnt = obj.Count();
    cout<< "Total elemets are : "<<cnt<<endl;

    obj.DeleteLast();
    obj.Display();

    cnt = obj.Count();
    cout<< "Total elemets are : "<<cnt<<endl;

    return 0;
}

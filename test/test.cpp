#include "SinglyLL.h"

int main()
{
    SinglyLL<int> obj;

    obj.InsertFirst(10);
    obj.InsertFirst(20);

    obj.Display();

    return 0;
}
#include <iostream>
#include "stack/Stack.h"

using namespace std;

int main()
{
    Stack<int> s;

    int a = 10, b = 20, c = 30;

    s.Push(&a);
    s.Push(&b);
    s.Push(&c);

    s.DisplayStack();

    int top;
    if (s.Peek(&top))
    {
        cout << "Top element: " << top << endl;
    }

    s.Pop();
    s.DisplayStack();

    cout << "Count: " << s.Count() << endl;

    s.FreeStack();

    return 0;
}
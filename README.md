# Generic Data Structures in C++

A header-only C++ library of fundamental data structures implemented using
templated classes. Each structure is fully generic — it works with any
copyable type `T` (int, float, struct, etc.).

---

## Structures Included

| File | Class | Type |
|------|-------|------|
| `Singly_linkedList.h` | `Singly_linkedList<T>` | Singly Linear Linked List |
| `Doubly_linkedList.h` | `Doubly_linkedList<T>` | Doubly Linear Linked List |
| `SinglyCircular_linkedList.h` | `SinglyCircular_linkedList<T>` | Singly Circular Linked List |
| `DoublyCircular_linkedList.h` | `DoublyCircular_linkedList<T>` | Doubly Circular Linked List |
| `Stack.h` | `Stack<T>` | Stack (LIFO) via Linked List |
| `Queue.h` | `Queue<T>` | Queue (FIFO) via Linked List |

---

## API Overview

### Singly Linear Linked List — `Singly_linkedList<T>`
| Method | Description |
|--------|-------------|
| `AddToGenListFirst(T*)` | Prepend node at front |
| `AddToGenListLast(T*)` | Append node at back |
| `InsertAtPosition(int, T*)` | Insert at 1-based position |
| `DeleteFromGenListFirst()` | Remove front node |
| `DeleteFromGenListLast()` | Remove last node |
| `DeleteAtPosition(int)` | Remove node at position |
| `DisplayGenList()` | Print list forward |
| `CountGenListNode()` | Count total nodes |
| `SearchFirstOccurance(T)` | Position of first match (-1 if not found) |
| `SearchAllOccurance(T)` | Count of all matches |
| `FreeGenList()` | Release all nodes |
| `ReverseList()` | Reverse the list in-place |

### Doubly Linear Linked List — `Doubly_linkedList<T>`
| Method | Description |
|--------|-------------|
| `AddToListFirst(T*)` | Prepend node at front |
| `AddToListLast(T*)` | Append node at back |
| `InsertAtPosition(int, T*)` | Insert at 1-based position |
| `DeleteFromListFirst()` | Remove front node |
| `DeleteFromListLast()` | Remove last node |
| `DeleteAtPosition(int)` | Remove node at position |
| `DisplayList()` | Print list forward |
| `DisplayListReverse()` | Print list backward |
| `CountListNode()` | Count total nodes |
| `SearchFirstOccurance(T)` | Position of first match (-1 if not found) |
| `FreeList()` | Release all nodes |

### Singly Circular Linked List — `SinglyCircular_linkedList<T>`
Same API as `Doubly_linkedList<T>` (without `DisplayListReverse`).

### Doubly Circular Linked List — `DoublyCircular_linkedList<T>`
Same API as `Doubly_linkedList<T>`, with circular traversal in both directions.

### Stack — `Stack<T>`  *(LIFO)*
| Method | Description |
|--------|-------------|
| `Push(T*)` | Push element onto top |
| `Pop()` | Remove top element |
| `Peek(T*)` | Read top without removing |
| `IsEmpty()` | Check if empty |
| `Count()` | Number of elements |
| `DisplayStack()` | Print top → bottom |
| `FreeStack()` | Release all nodes |

### Queue — `Queue<T>`  *(FIFO)*
| Method | Description |
|--------|-------------|
| `Enqueue(T*)` | Insert element at rear |
| `Dequeue()` | Remove element from front |
| `Peek(T*)` | Read front without removing |
| `IsEmpty()` | Check if empty |
| `Count()` | Number of elements |
| `DisplayQueue()` | Print front → rear |
| `FreeQueue()` | Release all nodes |

---

## Usage Example

```cpp
#include "Singly_linkedList.h"

int main()
{
    Singly_linkedList<int> list;

    int a = 10, b = 20, c = 30;
    list.AddToGenListLast(&a);
    list.AddToGenListLast(&b);
    list.AddToGenListLast(&c);

    list.DisplayGenList();          // 10 -> 20 -> 30 ->
    list.ReverseList();
    list.DisplayGenList();          // 30 -> 20 -> 10 ->

    cout << list.CountGenListNode() << endl;  // 3
    list.FreeGenList();

    return 0;
}
```

```cpp
#include "Stack.h"

int main()
{
    Stack<int> s;
    int x = 1, y = 2, z = 3;

    s.Push(&x); s.Push(&y); s.Push(&z);
    s.DisplayStack();   // TOP | 3 | | 2 | | 1 | BOTTOM

    s.Pop();
    int top;
    s.Peek(&top);       // top == 2

    s.FreeStack();
    return 0;
}
```

---

## Requirements

- C++11 or later
- Standard library headers: `<iostream>`, `<string.h>`

---

## Notes

- All structures are **header-only** — just `#include` the relevant `.h` file.
- Memory is managed with `malloc` / `free`; nodes are heap-allocated.
- `SearchFirstOccurance` / `SearchAllOccurance` use `memcmp` for comparison —
  best suited for plain-old-data (POD) types. For types with padding bytes or
  pointer members, prefer providing a custom comparator.
- None of the structures are thread-safe. Add external locking if sharing
  across threads.

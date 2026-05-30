#pragma once
#include <iostream>
#include <string.h>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
//  Convenience type aliases
// ─────────────────────────────────────────────────────────────────────────────
#define TRUE  1
#define FALSE 0

typedef int           BOOL;   // Boolean return type  (TRUE = 1, FALSE = 0)
typedef unsigned char BYTE;   // Single-byte unsigned integer


// ─────────────────────────────────────────────────────────────────────────────
//  GEN_STACK<T>
//
//  Generic node for a linked-list-based Stack.
//
//  Each node stores:
//    • Data  – the payload of type T
//    • pNext – pointer to the node below it in the stack (NULL at the bottom)
//
//  Memory layout (3-element stack, A pushed last):
//
//    pTop → [A] ──pNext──► [B] ──pNext──► [C] ──pNext──► NULL
//           (top)                          (bottom)
// ─────────────────────────────────────────────────────────────────────────────

template <typename T>
struct GEN_STACK
{
    T          Data;   // Payload stored in this node
    GEN_STACK *pNext;  // Pointer to the node beneath this one (NULL at bottom)
};


// ─────────────────────────────────────────────────────────────────────────────
//  Stack<T>
//
//  A generic Last-In First-Out (LIFO) stack backed by a singly-linked list.
//
//  LIFO discipline:
//    • Push  – inserts a new element at the top (O(1)).
//    • Pop   – removes the top element             (O(1)).
//    • Peek  – reads the top element without removal (O(1)).
//
//  The linked-list backing means the stack grows dynamically with no fixed
//  capacity limit beyond available heap memory.
//
//  Public data member
//  ──────────────────
//  pTop – pointer to the top-most node; NULL when the stack is empty.
//
//  Thread safety: NOT thread-safe. External synchronisation required when
//  shared across threads.
// ─────────────────────────────────────────────────────────────────────────────
template <class T>
class Stack
{
public:
    GEN_STACK<T> *pTop;  // Pointer to the top-most (most recently pushed) node

    // ── Construction / Destruction ──────────────────────────────────────────

    // Constructor – initialises an empty stack (pTop = NULL).
    Stack();

    // Destructor – releases all heap-allocated nodes when the object goes
    // out of scope.
    ~Stack();

    // ── Core Stack Operations ────────────────────────────────────────────────

    // Push – allocate a new node, copy *pNodeData into it, and place it on top.
    //   Parameters:
    //     pNodeData – pointer to the value to push (must not be NULL).
    //   Returns TRUE on success, FALSE on NULL input or allocation failure.
    BOOL Push(T *pNodeData);

    // Pop  – remove and free the top node (Stack Underflow if empty).
    //   Returns TRUE on success, FALSE if the stack is empty.
    BOOL Pop();

    // Peek – copy the top node's value into *pData without removing it.
    //   Parameters:
    //     pData – [OUT] pointer to receive the top value (must not be NULL).
    //   Returns TRUE on success, FALSE if the stack is empty.
    BOOL Peek(T *pData);

    // ── Utility ──────────────────────────────────────────────────────────────

    // IsEmpty     – return TRUE if the stack has no elements, FALSE otherwise.
    BOOL IsEmpty();

    // Count       – return the total number of elements currently in the stack.
    int  Count();

    // DisplayStack – print all elements from top to bottom in the form:
    //   TOP
    //   | D1 |
    //   | D2 |
    //   ...
    //   BOTTOM
    void DisplayStack();

    // FreeStack   – release every node and reset pTop to NULL.
    //   Returns TRUE on success, FALSE if the stack is already empty.
    BOOL FreeStack();
};


// ─────────────────────────────────────────────────────────────────────────────
//                        I M P L E M E N T A T I O N
// ─────────────────────────────────────────────────────────────────────────────


// ─────────────────────────────────────────────────────────────────────────────
//  Stack  (constructor)
//
//  Sets pTop to NULL, representing an empty stack.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
Stack<T>::Stack()
{
    pTop = NULL;
}


// ─────────────────────────────────────────────────────────────────────────────
//  ~Stack  (destructor)
//
//  Walks from pTop down through pNext pointers, freeing each node.
//  Equivalent to calling FreeStack(); invoked automatically on scope exit.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
Stack<T>::~Stack()
{
    GEN_STACK<T> *pTemp;
    GEN_STACK<T> *pNavigate;

    if (NULL != pTop)
    {
        pNavigate = pTop;
        while (NULL != pNavigate)
        {
            pTemp     = pNavigate->pNext;  // Save node below before freeing
            free(pNavigate);
            pNavigate = pTemp;
        }
    }
}


// ─────────────────────────────────────────────────────────────────────────────
//  Push
//
//  Allocates a new node, copies *pNodeData into it, and places it on top.
//
//  Empty stack case:
//    The new node becomes the only element; pNext stays NULL.
//
//  Non-empty stack case  (new node N, current top T):
//    Before: pTop → T → ...
//    After:  pTop → N → T → ...
//
//    Steps:
//      1. N.pNext = T    (N points down to old top)
//      2. pTop    = N    (advance top pointer)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Stack<T>::Push(T *pNodeData)
{
    GEN_STACK<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_STACK<T> *)malloc(sizeof(GEN_STACK<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, set payload and null the link pointer
    memset(pNewNode, 0, sizeof(GEN_STACK<T>));
    pNewNode->pNext = NULL;
    pNewNode->Data  = *pNodeData;

    if (NULL == pTop)
    {
        // ── First element: only node in the stack ──
        pTop = pNewNode;
    }
    else
    {
        // ── Push: wire N on top of current top ──
        pNewNode->pNext = pTop;  // 1. N → old top
        pTop            = pNewNode; // 2. advance top pointer
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  Pop
//
//  Removes and frees the top-most node (Stack Underflow if empty).
//
//  Steps:
//    1. Save pTop in temp.
//    2. Advance pTop to temp->pNext (next element becomes the new top).
//    3. Free temp.
//
//  Returns FALSE (and prints "Stack Underflow") if the stack is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Stack<T>::Pop()
{
    GEN_STACK<T> *temp;

    if (NULL == pTop)
    {
        cout << "Stack Underflow" << endl;  // Underflow: nothing to pop
        return FALSE;
    }

    temp = pTop;            // Save old top
    pTop = pTop->pNext;     // The element below becomes the new top
    free(temp);             // Release old top node

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  Peek
//
//  Reads the top element's value into *pData without modifying the stack.
//  Useful for inspecting the top before deciding whether to pop.
//
//  Returns FALSE if the stack is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Stack<T>::Peek(T *pData)
{
    if (NULL == pTop)
    {
        return FALSE;  // Nothing to peek at
    }

    *pData = pTop->Data;  // Copy top value to caller's variable
    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  IsEmpty
//
//  Returns TRUE if pTop is NULL (no elements), FALSE otherwise.
//  Implemented as a single ternary expression for clarity.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Stack<T>::IsEmpty()
{
    return (NULL == pTop) ? TRUE : FALSE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  Count
//
//  Walks from pTop to NULL, incrementing a counter per node.
//  O(n) — proportional to the number of elements.
//  Returns 0 for an empty stack.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
Stack<T>::Count()
{
    int           CountNode = 0;
    GEN_STACK<T> *pTemp     = pTop;

    while (NULL != pTemp)
    {
        CountNode++;
        pTemp = pTemp->pNext;
    }

    return CountNode;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DisplayStack
//
//  Prints all elements from top to bottom in a visual box format:
//    TOP
//    | D1 |
//    | D2 |
//    ...
//    BOTTOM
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
void
Stack<T>::DisplayStack()
{
    GEN_STACK<T> *pTempNode = pTop;

    cout << "TOP" << endl;
    while (NULL != pTempNode)
    {
        cout << "| " << pTempNode->Data << " |" << endl;
        pTempNode = pTempNode->pNext;
    }
    cout << "BOTTOM" << endl;
}


// ─────────────────────────────────────────────────────────────────────────────
//  FreeStack
//
//  Releases every node and resets pTop to NULL.
//  Callable on-demand to clear and reuse the stack object.
//
//  Returns TRUE on success, FALSE if the stack is already empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Stack<T>::FreeStack()
{
    GEN_STACK<T> *pTemp;
    GEN_STACK<T> *pNavigate;

    if (NULL == pTop)
    {
        return FALSE;  // Nothing to free
    }

    pNavigate = pTop;
    while (NULL != pNavigate)
    {
        pTemp     = pNavigate->pNext;  // Save node below before freeing
        free(pNavigate);
        pNavigate = pTemp;
    }

    pTop = NULL;
    return TRUE;
}


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
//  GEN_QUEUE<T>
//
//  Generic node for a linked-list-based Queue.
//
//  Each node stores:
//    • Data  – the payload of type T
//    • pNext – pointer to the node behind it in the queue (NULL at the rear)
//
//  Memory layout (3-element queue, A enqueued first):
//
//    pFront → [A] ──pNext──► [B] ──pNext──► [C] ──pNext──► NULL
//              ↑                              ↑
//           (front/dequeue here)         (rear/enqueue here)
//                                           pRear ──────────┘
// ─────────────────────────────────────────────────────────────────────────────

template <typename T>
struct GEN_QUEUE
{
    T          Data;   // Payload stored in this node
    GEN_QUEUE *pNext;  // Pointer to the next node toward the rear (NULL at rear)
};


// ─────────────────────────────────────────────────────────────────────────────
//  Queue<T>
//
//  A generic First-In First-Out (FIFO) queue backed by a singly-linked list.
//
//  FIFO discipline:
//    • Enqueue – inserts a new element at the rear  (O(1)).
//    • Dequeue – removes the front element           (O(1)).
//    • Peek    – reads the front element without removal (O(1)).
//
//  The linked-list backing means the queue grows dynamically with no fixed
//  capacity limit beyond available heap memory.
//
//  Public data members
//  ───────────────────
//  pFront – pointer to the front node (next to be dequeued); NULL when empty.
//  pRear  – pointer to the rear  node (most recently enqueued); NULL when empty.
//
//  Thread safety: NOT thread-safe. External synchronisation required when
//  shared across threads.
// ─────────────────────────────────────────────────────────────────────────────
template <class T>
class Queue
{
public:
    GEN_QUEUE<T> *pFront;  // Pointer to the front node (oldest element)
    GEN_QUEUE<T> *pRear;   // Pointer to the rear  node (newest element)

    // ── Construction / Destruction ──────────────────────────────────────────

    // Constructor – initialises an empty queue (pFront = pRear = NULL).
    Queue();

    // Destructor – releases all heap-allocated nodes when the object goes
    // out of scope.
    ~Queue();

    // ── Core Queue Operations ────────────────────────────────────────────────

    // Enqueue – allocate a new node, copy *pNodeData into it, and add it at
    //           the rear of the queue.
    //   Parameters:
    //     pNodeData – pointer to the value to enqueue (must not be NULL).
    //   Returns TRUE on success, FALSE on NULL input or allocation failure.
    BOOL Enqueue(T *pNodeData);

    // Dequeue – remove and free the front node (Queue Underflow if empty).
    //   Returns TRUE on success, FALSE if the queue is empty.
    BOOL Dequeue();

    // Peek – copy the front node's value into *pData without removing it.
    //   Parameters:
    //     pData – [OUT] pointer to receive the front value (must not be NULL).
    //   Returns TRUE on success, FALSE if the queue is empty.
    BOOL Peek(T *pData);

    // ── Utility ──────────────────────────────────────────────────────────────

    // IsEmpty      – return TRUE if the queue has no elements, FALSE otherwise.
    BOOL IsEmpty();

    // Count        – return the total number of elements in the queue.
    int  Count();

    // DisplayQueue – print all elements from front to rear:
    //   FRONT -> D1 -> D2 -> ... -> Dn -> REAR
    void DisplayQueue();

    // FreeQueue    – release every node and reset pFront/pRear to NULL.
    //   Returns TRUE on success, FALSE if the queue is already empty.
    BOOL FreeQueue();
};


// ─────────────────────────────────────────────────────────────────────────────
//                        I M P L E M E N T A T I O N
// ─────────────────────────────────────────────────────────────────────────────


// ─────────────────────────────────────────────────────────────────────────────
//  Queue  (constructor)
//
//  Sets pFront and pRear to NULL, representing an empty queue.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
Queue<T>::Queue()
{
    pFront = NULL;
    pRear  = NULL;
}


// ─────────────────────────────────────────────────────────────────────────────
//  ~Queue  (destructor)
//
//  Walks from pFront to NULL, freeing each node.
//  Equivalent to calling FreeQueue(); invoked automatically on scope exit.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
Queue<T>::~Queue()
{
    GEN_QUEUE<T> *pTemp;
    GEN_QUEUE<T> *pNavigate;

    if (NULL != pFront)
    {
        pNavigate = pFront;
        while (NULL != pNavigate)
        {
            pTemp     = pNavigate->pNext;  // Save successor before freeing
            free(pNavigate);
            pNavigate = pTemp;
        }
    }
}


// ─────────────────────────────────────────────────────────────────────────────
//  Enqueue
//
//  Allocates a new node, copies *pNodeData into it, and appends it at the rear.
//
//  Empty queue case:
//    The new node becomes both pFront and pRear; pNext stays NULL.
//
//  Non-empty queue case  (current rear R, new node N):
//    Before: pFront → ... → R → NULL
//    After:  pFront → ... → R → N → NULL
//
//    Steps:
//      1. R.pNext = N    (old rear points forward to N)
//      2. pRear   = N    (advance rear pointer)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Queue<T>::Enqueue(T *pNodeData)
{
    GEN_QUEUE<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_QUEUE<T> *)malloc(sizeof(GEN_QUEUE<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, set payload and null the link pointer
    memset(pNewNode, 0, sizeof(GEN_QUEUE<T>));
    pNewNode->pNext = NULL;
    pNewNode->Data  = *pNodeData;

    if (NULL == pFront)
    {
        // ── First element: becomes both front and rear ──
        pFront = pNewNode;
        pRear  = pNewNode;
    }
    else
    {
        // ── Append at rear ──
        pRear->pNext = pNewNode;  // 1. old rear → N
        pRear        = pNewNode;  // 2. advance rear pointer
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  Dequeue
//
//  Removes and frees the front node (Queue Underflow if empty).
//
//  Steps:
//    1. Save pFront in temp.
//    2. Advance pFront to temp->pNext.
//    3. If pFront is now NULL the queue became empty — reset pRear too.
//    4. Free temp.
//
//  Returns FALSE (and prints "Queue Underflow") if the queue is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Queue<T>::Dequeue()
{
    GEN_QUEUE<T> *temp;

    if (NULL == pFront)
    {
        cout << "Queue Underflow" << endl;  // Underflow: nothing to dequeue
        return FALSE;
    }

    temp   = pFront;           // Save old front
    pFront = pFront->pNext;    // Advance front pointer

    if (NULL == pFront)
    {
        pRear = NULL;  // Queue became empty; rear must also be NULL
    }

    free(temp);  // Release old front node
    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  Peek
//
//  Reads the front element's value into *pData without modifying the queue.
//  Useful for inspecting the next item to be dequeued.
//
//  Returns FALSE if the queue is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Queue<T>::Peek(T *pData)
{
    if (NULL == pFront)
    {
        return FALSE;  // Nothing to peek at
    }

    *pData = pFront->Data;  // Copy front value to caller's variable
    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  IsEmpty
//
//  Returns TRUE if pFront is NULL (no elements), FALSE otherwise.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Queue<T>::IsEmpty()
{
    return (NULL == pFront) ? TRUE : FALSE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  Count
//
//  Walks from pFront to NULL, incrementing a counter per node.
//  O(n) — proportional to the number of elements.
//  Returns 0 for an empty queue.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
Queue<T>::Count()
{
    int            CountNode = 0;
    GEN_QUEUE<T>  *pTemp     = pFront;

    while (NULL != pTemp)
    {
        CountNode++;
        pTemp = pTemp->pNext;
    }

    return CountNode;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DisplayQueue
//
//  Traverses from pFront to NULL and prints each element:
//    FRONT -> D1 -> D2 -> ... -> Dn -> REAR
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
void
Queue<T>::DisplayQueue()
{
    GEN_QUEUE<T> *pTempNode = pFront;

    cout << "FRONT -> ";
    while (NULL != pTempNode)
    {
        cout << pTempNode->Data << " -> ";
        pTempNode = pTempNode->pNext;
    }
    cout << "REAR" << endl;
}


// ─────────────────────────────────────────────────────────────────────────────
//  FreeQueue
//
//  Releases every node and resets pFront / pRear to NULL.
//  Callable on-demand to clear and reuse the queue object.
//
//  Returns TRUE on success, FALSE if the queue is already empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Queue<T>::FreeQueue()
{
    GEN_QUEUE<T> *pTemp;
    GEN_QUEUE<T> *pNavigate;

    if (NULL == pFront)
    {
        return FALSE;  // Nothing to free
    }

    pNavigate = pFront;
    while (NULL != pNavigate)
    {
        pTemp     = pNavigate->pNext;  // Save successor before freeing
        free(pNavigate);
        pNavigate = pTemp;
    }

    pFront = NULL;
    pRear  = NULL;

    return TRUE;
}


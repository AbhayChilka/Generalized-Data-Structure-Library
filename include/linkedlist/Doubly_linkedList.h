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
//  GEN_DLIST<T>
//
//  Generic node for a Doubly Linear Linked List.
//
//  Each node stores:
//    • Data  – the payload of type T
//    • pNext – pointer to the next     node (NULL at the tail)
//    • pPrev – pointer to the previous node (NULL at the head)
//
//  Memory layout (3-node example):
//
//    NULL ← [N1] ⇄ [N2] ⇄ [N3] → NULL
//            ↑                 ↑
//          pHead             pTail
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
struct GEN_DLIST
{
    T          Data;   // Payload stored in this node
    GEN_DLIST *pNext;  // Pointer to the next node     (NULL if this is the tail)
    GEN_DLIST *pPrev;  // Pointer to the previous node (NULL if this is the head)
};


// ─────────────────────────────────────────────────────────────────────────────
//  Doubly_linkedList<T>
//
//  A generic doubly-linked linear (non-circular) list.
//
//  Key properties
//  ──────────────
//  • Doubly linked  – every node carries both a forward (pNext) and a
//                     backward (pPrev) pointer, enabling O(1) insertion or
//                     deletion when a node pointer is already known.
//  • Linear         – the list has definite head and tail endpoints;
//                     pHead->pPrev == NULL and pTail->pNext == NULL.
//  • Generic        – parameterised on type T; works with any copyable type.
//
//  Public data members
//  ───────────────────
//  pHead – first node; NULL when the list is empty.
//  pTail – last  node; NULL when the list is empty.
//
//  Thread safety: NOT thread-safe. External synchronisation required when
//  shared across threads.
// ─────────────────────────────────────────────────────────────────────────────
template <class T>
class Doubly_linkedList
{
public:
    GEN_DLIST<T> *pHead;  // Pointer to the first node (front of the list)
    GEN_DLIST<T> *pTail;  // Pointer to the last  node (back  of the list)

    // ── Construction / Destruction ──────────────────────────────────────────

    // Constructor – initialises an empty list (pHead = pTail = NULL).
    Doubly_linkedList();

    // Destructor – releases all heap-allocated nodes automatically when the
    // object goes out of scope. Equivalent to calling FreeList().
    ~Doubly_linkedList();

    // ── Insertion ───────────────────────────────────────────────────────────

    // AddToListFirst   – prepend a new node at the front.
    // AddToListLast    – append  a new node at the back.
    // InsertAtPosition – insert at 1-based position `pos`; delegates to
    //                    AddToListFirst / AddToListLast for the boundary cases.
    //
    // Parameters:
    //   pNodeData – pointer to the data to copy into the new node (not NULL).
    // Returns TRUE on success, FALSE on NULL input / allocation failure /
    // out-of-range position.
    BOOL AddToListFirst(T *pNodeData);
    BOOL AddToListLast (T *pNodeData);
    BOOL InsertAtPosition(int pos, T *pNodeData);

    // ── Deletion ─────────────────────────────────────────────────────────────

    // DeleteFromListFirst – remove and free the first node.
    // DeleteFromListLast  – remove and free the last  node.
    // DeleteAtPosition    – remove and free the node at 1-based position `pos`.
    //
    // Returns TRUE on success, FALSE if the list is empty or pos is invalid.
    BOOL DeleteFromListFirst();
    BOOL DeleteFromListLast();
    BOOL DeleteAtPosition(int pos);

    // ── Display ──────────────────────────────────────────────────────────────

    // DisplayList        – print head → tail:  D1 <-> D2 <-> ... <-> NULL
    // DisplayListReverse – print tail → head:  Dn <-> ... <-> D1 <-> NULL
    void DisplayList();
    void DisplayListReverse();

    // ── Utility ──────────────────────────────────────────────────────────────

    // CountListNode – return the total number of nodes (0 for empty list).
    int CountListNode();

    // SearchFirstOccurance – return the 1-based position of the first node
    //   whose Data matches `data` (byte-wise via memcmp), or -1 if not found.
    int SearchFirstOccurance(T data);

    // FreeList – manually release every node and reset pHead/pTail to NULL.
    //   Returns TRUE on success, FALSE if the list is already empty.
    BOOL FreeList();
};


// ─────────────────────────────────────────────────────────────────────────────
//                        I M P L E M E N T A T I O N
// ─────────────────────────────────────────────────────────────────────────────


// ─────────────────────────────────────────────────────────────────────────────
//  Doubly_linkedList  (constructor)
//
//  Sets pHead and pTail to NULL, representing an empty list.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
Doubly_linkedList<T>::Doubly_linkedList()
{
    pHead = NULL;
    pTail = NULL;
}


// ─────────────────────────────────────────────────────────────────────────────
//  ~Doubly_linkedList  (destructor)
//
//  Walks the list from pHead to NULL, freeing each node.
//  The standard while-loop is correct here (unlike the circular variants)
//  because pNext == NULL at the tail, giving a natural termination condition.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
Doubly_linkedList<T>::~Doubly_linkedList()
{
    GEN_DLIST<T> *pTemp;
    GEN_DLIST<T> *pNavigate;

    if (NULL != pHead)
    {
        pNavigate = pHead;
        while (NULL != pNavigate)
        {
            pTemp     = pNavigate->pNext;  // Save successor before freeing
            free(pNavigate);
            pNavigate = pTemp;
        }
    }
}


// ─────────────────────────────────────────────────────────────────────────────
//  AddToListFirst
//
//  Prepends a new node to the front of the list.
//
//  Empty list case:
//    The new node becomes both pHead and pTail; both link pointers stay NULL.
//
//  Non-empty list case  (new node N, old head H):
//    Before: NULL ← H ⇄ ...
//    After:  NULL ← N ⇄ H ⇄ ...
//
//    Steps:
//      1. N.pNext = H    (N points forward to old head)
//      2. H.pPrev = N    (old head points back to N)
//      3. pHead   = N    (update head pointer)
//    N.pPrev remains NULL (N is the new head).
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Doubly_linkedList<T>::AddToListFirst(T *pNodeData)
{
    GEN_DLIST<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_DLIST<T> *)malloc(sizeof(GEN_DLIST<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, set payload, null the links
    memset(pNewNode, 0, sizeof(GEN_DLIST<T>));
    pNewNode->pNext = NULL;
    pNewNode->pPrev = NULL;
    pNewNode->Data  = *pNodeData;

    if (NULL == pHead)
    {
        // ── First node: becomes both head and tail ──
        pHead = pNewNode;
        pTail = pNewNode;
    }
    else
    {
        // ── Prepend: wire N before current head ──
        pNewNode->pNext = pHead;  // 1. N → old head
        pHead->pPrev    = pNewNode; // 2. old head ← N
        pHead           = pNewNode; // 3. advance head pointer
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  AddToListLast
//
//  Appends a new node to the back of the list.
//
//  Empty list case: same as AddToListFirst.
//
//  Non-empty list case  (current tail T, new node N):
//    Before: ... ⇄ T → NULL
//    After:  ... ⇄ T ⇄ N → NULL
//
//    Steps:
//      1. T.pNext = N    (old tail points forward to N)
//      2. N.pPrev = T    (N points back to old tail)
//      3. pTail   = N    (update tail pointer)
//    N.pNext remains NULL (N is the new tail).
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Doubly_linkedList<T>::AddToListLast(T *pNodeData)
{
    GEN_DLIST<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_DLIST<T> *)malloc(sizeof(GEN_DLIST<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, set payload, null the links
    memset(pNewNode, 0, sizeof(GEN_DLIST<T>));
    pNewNode->pNext = NULL;
    pNewNode->pPrev = NULL;
    pNewNode->Data  = *pNodeData;

    if (NULL == pHead)
    {
        // ── First node: becomes both head and tail ──
        pHead = pNewNode;
        pTail = pNewNode;
    }
    else
    {
        // ── Append: wire N after current tail ──
        pTail->pNext    = pNewNode;  // 1. old tail → N
        pNewNode->pPrev = pTail;     // 2. N ← old tail
        pTail           = pNewNode;  // 3. advance tail pointer
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  InsertAtPosition
//
//  Inserts a new node so it occupies 1-based position `pos`.
//
//  Boundary delegation:
//    pos == 1          → AddToListFirst
//    pos == size + 1   → AddToListLast
//
//  General case (2 ≤ pos ≤ size):
//    Walk to the node at position (pos - 1), then splice pNewNode between
//    that node (temp) and temp->pNext.
//
//    Splice steps (S = temp->pNext):
//      1. newNode.pNext = S      (new → S)
//      2. newNode.pPrev = temp   (new ← temp)
//      3. S.pPrev       = newNode (S ← new)
//      4. temp.pNext    = newNode (temp → new)
//
//  Returns FALSE if: list is empty, pos > size+1, or allocation fails.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Doubly_linkedList<T>::InsertAtPosition(int pos, T *pNodeData)
{
    int           i        = 1;
    GEN_DLIST<T> *temp     = NULL;
    GEN_DLIST<T> *pNewNode = NULL;

    if (NULL == pHead)
    {
        return FALSE;  // Cannot insert by position into an empty list
    }
    else if (pos == 1)
    {
        AddToListFirst(pNodeData);
    }
    else if (pos == (CountListNode() + 1))
    {
        AddToListLast(pNodeData);
    }
    else if (pos > CountListNode())
    {
        return FALSE;  // Position is out of range
    }
    else
    {
        // ── General case: allocate and splice ──
        pNewNode = (GEN_DLIST<T> *)malloc(sizeof(GEN_DLIST<T>));
        if (NULL == pNewNode)
        {
            return FALSE;  // Memory allocation failed
        }

        memset(pNewNode, 0, sizeof(GEN_DLIST<T>));
        pNewNode->pNext = NULL;
        pNewNode->pPrev = NULL;
        pNewNode->Data  = *pNodeData;

        // Walk to node at position (pos - 1)
        temp = pHead;
        while (i < pos - 1)
        {
            temp = temp->pNext;
            ++i;
        }

        // Splice pNewNode between temp and temp->pNext
        pNewNode->pNext    = temp->pNext;   // 1
        pNewNode->pPrev    = temp;           // 2
        temp->pNext->pPrev = pNewNode;       // 3
        temp->pNext        = pNewNode;       // 4
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteFromListFirst
//
//  Removes and frees the head node.
//
//  Steps:
//    1. Save pHead in temp.
//    2. Advance pHead to pHead->pNext.
//    3. If the new pHead exists, clear its pPrev (it is the new front).
//       Otherwise the list became empty — set pTail = NULL too.
//    4. Free temp.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Doubly_linkedList<T>::DeleteFromListFirst()
{
    GEN_DLIST<T> *temp;

    if (NULL == pHead)
    {
        return FALSE;  // Nothing to delete
    }

    temp  = pHead;            // Save old head
    pHead = pHead->pNext;     // Advance head pointer

    if (NULL != pHead)
    {
        pHead->pPrev = NULL;  // New head has no predecessor
    }
    else
    {
        pTail = NULL;         // List is now empty
    }

    free(temp);
    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteFromListLast
//
//  Removes and frees the tail node.
//
//  Steps:
//    1. Save pTail in temp.
//    2. Retreat pTail to pTail->pPrev.
//    3. If the new pTail exists, clear its pNext (it is the new back).
//       Otherwise the list became empty — set pHead = NULL too.
//    4. Free temp.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Doubly_linkedList<T>::DeleteFromListLast()
{
    GEN_DLIST<T> *temp;

    if (NULL == pHead)
    {
        return FALSE;  // Nothing to delete
    }

    temp  = pTail;            // Save old tail
    pTail = pTail->pPrev;     // Retreat tail pointer

    if (NULL != pTail)
    {
        pTail->pNext = NULL;  // New tail has no successor
    }
    else
    {
        pHead = NULL;         // List is now empty
    }

    free(temp);
    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteAtPosition
//
//  Removes and frees the node at 1-based position `pos`.
//
//  Boundary delegation:
//    pos == 1    → DeleteFromListFirst
//    pos == size → DeleteFromListLast
//
//  General case: walk to the target node, unlink it by updating its
//  neighbours' pointers, then free it.
//
//  Returns FALSE if pos > size.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Doubly_linkedList<T>::DeleteAtPosition(int pos)
{
    int           i    = 0;
    GEN_DLIST<T> *temp = NULL;

    if (pos > CountListNode())
    {
        return FALSE;  // Position is beyond the end of the list
    }
    else if (pos == 1)
    {
        DeleteFromListFirst();
    }
    else if (pos == CountListNode())
    {
        DeleteFromListLast();
    }
    else
    {
        // Walk to the node at position `pos`
        temp = pHead;
        for (i = 1; i < pos; i++)
        {
            temp = temp->pNext;
        }

        // Unlink: predecessor skips over temp, successor skips back over temp
        temp->pPrev->pNext = temp->pNext;
        temp->pNext->pPrev = temp->pPrev;
        free(temp);
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DisplayList
//
//  Traverses from pHead forward, printing:
//    D1 <-> D2 <-> ... <-> Dn <-> NULL
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
void
Doubly_linkedList<T>::DisplayList()
{
    GEN_DLIST<T> *pTempNode = pHead;

    while (NULL != pTempNode)
    {
        cout << pTempNode->Data << " <-> ";
        pTempNode = pTempNode->pNext;
    }
    cout << "NULL" << endl;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DisplayListReverse
//
//  Traverses from pTail backward (via pPrev), printing:
//    Dn <-> ... <-> D1 <-> NULL
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
void
Doubly_linkedList<T>::DisplayListReverse()
{
    GEN_DLIST<T> *pTempNode = pTail;

    while (NULL != pTempNode)
    {
        cout << pTempNode->Data << " <-> ";
        pTempNode = pTempNode->pPrev;
    }
    cout << "NULL" << endl;
}


// ─────────────────────────────────────────────────────────────────────────────
//  CountListNode
//
//  Walks from pHead to NULL, incrementing a counter per node.
//  Returns 0 for an empty list.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
Doubly_linkedList<T>::CountListNode()
{
    int           CountNode = 0;
    GEN_DLIST<T> *pListHead = pHead;

    while (NULL != pListHead)
    {
        CountNode++;
        pListHead = pListHead->pNext;
    }

    return CountNode;
}


// ─────────────────────────────────────────────────────────────────────────────
//  SearchFirstOccurance
//
//  Scans from pHead and returns the 1-based position of the first node whose
//  Data matches `data` byte-for-byte (memcmp).
//
//  Note: memcmp works for plain-old-data types; types with padding or pointer
//  members may produce unexpected results.
//
//  Returns -1 if not found or the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
Doubly_linkedList<T>::SearchFirstOccurance(T data)
{
    int           CountNode = 0;
    GEN_DLIST<T> *pListHead = pHead;

    while (NULL != pListHead)
    {
        ++CountNode;

        if (memcmp(&(pListHead->Data), &data, sizeof(data)) == 0)
        {
            return CountNode;  // Match found – return 1-based position
        }

        pListHead = pListHead->pNext;
    }

    return -1;  // Not found
}


// ─────────────────────────────────────────────────────────────────────────────
//  FreeList
//
//  Walks from pHead to NULL, freeing every node, then resets both pointers.
//  Callable on-demand to clear and reuse the list object without destroying it.
//
//  Returns TRUE on success, FALSE if the list is already empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Doubly_linkedList<T>::FreeList()
{
    GEN_DLIST<T> *pTemp;
    GEN_DLIST<T> *pNavigate;

    if (NULL == pHead)
    {
        return FALSE;  // Nothing to free
    }

    pNavigate = pHead;
    while (NULL != pNavigate)
    {
        pTemp     = pNavigate->pNext;  // Save successor before freeing
        free(pNavigate);
        pNavigate = pTemp;
    }

    pHead = NULL;
    pTail = NULL;

    return TRUE;
}


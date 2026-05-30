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
//  GEN_LIST<T>
//
//  Generic node for a Singly Linear Linked List.
//
//  Each node stores:
//    • Data  – the payload of type T
//    • pNext – pointer to the next node (NULL at the tail)
//
//  Memory layout (3-node example):
//
//    [N1] ──pNext──► [N2] ──pNext──► [N3] ──pNext──► NULL
//     ↑                                ↑
//   pHead                            pTail
// ─────────────────────────────────────────────────────────────────────────────

template <typename T>
struct GEN_LIST
{
    T         Data;   // Payload stored in this node
    GEN_LIST *pNext;  // Pointer to the next node (NULL if this is the tail)
};


// ─────────────────────────────────────────────────────────────────────────────
//  Singly_linkedList<T>
//
//  A generic singly-linked linear (non-circular) list.
//
//  Key properties
//  ──────────────
//  • Singly linked – each node has only a forward (pNext) pointer.
//  • Linear        – pHead->... chain terminates at pTail->pNext == NULL.
//  • Generic       – parameterised on type T; works with any copyable type.
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
class Singly_linkedList
{
public:
    GEN_LIST<T> *pHead;  // Pointer to the first node (front of the list)
    GEN_LIST<T> *pTail;  // Pointer to the last  node (back  of the list)

    // ── Construction / Destruction ──────────────────────────────────────────

    // Constructor – initialises an empty list (pHead = pTail = NULL).
    Singly_linkedList();

    // Destructor – releases all heap-allocated nodes when the object goes
    // out of scope.
    ~Singly_linkedList();

    // ── Insertion ───────────────────────────────────────────────────────────

    // AddToGenListFirst  – prepend a new node at the front.
    // AddToGenListLast   – append  a new node at the back.
    // InsertAtPosition   – insert at 1-based position `pos`; delegates to
    //                      AddToGenListFirst / AddToGenListLast for boundaries.
    //
    // Parameters:
    //   pNodeData – pointer to the data to copy into the new node (not NULL).
    // Returns TRUE on success, FALSE on NULL input / allocation failure /
    // out-of-range position.
    BOOL AddToGenListFirst(T *pNodeData);
    BOOL AddToGenListLast (T *pNodeData);
    BOOL InsertAtPosition(int pos, T *pNodeData);

    // ── Deletion ─────────────────────────────────────────────────────────────

    // DeleteFromGenListFirst – remove and free the first node.
    // DeleteFromGenListLast  – remove and free the last  node.
    // DeleteAtPosition       – remove and free the node at 1-based pos.
    //
    // Returns TRUE on success, FALSE if the list is empty or pos is invalid.
    BOOL DeleteFromGenListFirst();
    BOOL DeleteFromGenListLast();
    BOOL DeleteAtPosition(int pos);

    // ── Display ──────────────────────────────────────────────────────────────

    // DisplayGenList – print head → tail:  D1 -> D2 -> ... -> Dn ->
    void DisplayGenList();

    // ── Utility ──────────────────────────────────────────────────────────────

    // CountGenListNode     – return total node count (0 for empty list).
    // SearchFirstOccurance – return 1-based position of first match, or -1.
    // SearchAllOccurance   – return total count of matching nodes.
    // FreeGenList          – release all nodes; returns TRUE on success.
    // ReverseList          – reverse the list in-place; returns TRUE on success.
    int  CountGenListNode();
    int  SearchFirstOccurance(T data);
    int  SearchAllOccurance  (T data);
    BOOL FreeGenList();
    BOOL ReverseList();
};


// ─────────────────────────────────────────────────────────────────────────────
//                        I M P L E M E N T A T I O N
// ─────────────────────────────────────────────────────────────────────────────


// ─────────────────────────────────────────────────────────────────────────────
//  Singly_linkedList  (constructor)
//
//  Sets pHead and pTail to NULL, representing an empty list.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
Singly_linkedList<T>::Singly_linkedList()
{
    pHead = NULL;
    pTail = NULL;
}


// ─────────────────────────────────────────────────────────────────────────────
//  ~Singly_linkedList  (destructor)
//
//  Walks from pHead to NULL, freeing each node.
//  pTemp saves the successor so pNavigate can be freed safely.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
Singly_linkedList<T>::~Singly_linkedList()
{
    GEN_LIST<T> *pTemp;
    GEN_LIST<T> *pNavigate;

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
//  AddToGenListFirst
//
//  Prepends a new node to the front of the list.
//
//  Empty list case:
//    The new node becomes both pHead and pTail; pNext stays NULL.
//
//  Non-empty list case  (new node N, old head H):
//    Before: H -> ...
//    After:  N -> H -> ...
//
//    Steps:
//      1. N.pNext = H    (N points forward to old head)
//      2. pHead   = N    (advance head pointer)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Singly_linkedList<T>::AddToGenListFirst(T *pNodeData)
{
    GEN_LIST<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_LIST<T> *)malloc(sizeof(GEN_LIST<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, set payload and null the link pointer
    memset(pNewNode, 0, sizeof(GEN_LIST<T>));
    pNewNode->pNext = NULL;
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
        pHead           = pNewNode; // 2. advance head pointer
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  AddToGenListLast
//
//  Appends a new node to the back of the list.
//
//  Empty list case: same as AddToGenListFirst.
//
//  Non-empty list case  (current tail T, new node N):
//    Before: ... -> T -> NULL
//    After:  ... -> T -> N -> NULL
//
//    Steps:
//      1. T.pNext = N    (old tail points forward to N)
//      2. pTail   = N    (advance tail pointer)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Singly_linkedList<T>::AddToGenListLast(T *pNodeData)
{
    GEN_LIST<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_LIST<T> *)malloc(sizeof(GEN_LIST<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, set payload and null the link pointer
    memset(pNewNode, 0, sizeof(GEN_LIST<T>));
    pNewNode->pNext = NULL;
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
        pTail->pNext = pNewNode;  // 1. old tail → N
        pTail        = pNewNode;  // 2. advance tail pointer
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  FreeGenList
//
//  Releases every node and resets pHead to NULL.
//  (Note: pTail is not reset here — callers should be aware of this.)
//
//  Returns TRUE on success, FALSE if the list is already empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Singly_linkedList<T>::FreeGenList()
{
    GEN_LIST<T> *pTemp;
    GEN_LIST<T> *pNavigate;

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

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  CountGenListNode
//
//  Walks from pHead to NULL, incrementing a counter per node.
//  Returns 0 for an empty list.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
Singly_linkedList<T>::CountGenListNode()
{
    int          CountNode = 0;
    GEN_LIST<T> *pListHead = pHead;

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
//  Note: memcmp suits plain-old-data types. Types with padding bytes or
//  pointer members may yield unexpected results.
//
//  Returns -1 if not found.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
Singly_linkedList<T>::SearchFirstOccurance(T data)
{
    int          CountNode = 0;
    GEN_LIST<T> *pListHead = pHead;

    while (pListHead != NULL)
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
//  SearchAllOccurance
//
//  Scans the entire list and returns the total number of nodes whose Data
//  matches `data` byte-for-byte (memcmp).
//
//  Returns 0 if no matches are found.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
Singly_linkedList<T>::SearchAllOccurance(T data)
{
    int          CountNode = 0;
    GEN_LIST<T> *pListHead = pHead;

    while (pListHead != NULL)
    {
        if (memcmp(&(pListHead->Data), &data, sizeof(data)) == 0)
        {
            ++CountNode;  // Tally every matching node (don't stop early)
        }

        pListHead = pListHead->pNext;
    }

    return CountNode;
}


// ─────────────────────────────────────────────────────────────────────────────
//  InsertAtPosition
//
//  Inserts a new node so it occupies 1-based position `pos`.
//
//  Boundary delegation:
//    pos == 1          → AddToGenListFirst
//    pos == size + 1   → AddToGenListLast
//
//  General case (2 ≤ pos ≤ size):
//    Walk to the node at position (pos - 1), then splice pNewNode:
//      1. newNode.pNext = temp.pNext   (new → old successor)
//      2. temp.pNext    = newNode      (predecessor → new)
//
//  Returns FALSE if list is empty, pos > size+1, or allocation fails.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Singly_linkedList<T>::InsertAtPosition(int pos, T *pNodeData)
{
    int          i        = 1;
    GEN_LIST<T> *temp     = NULL;
    GEN_LIST<T> *pNewNode = NULL;

    if (pHead == NULL)
    {
        return FALSE;  // Cannot insert by position into an empty list
    }
    else if (pos == 1)
    {
        AddToGenListFirst(pNodeData);
    }
    else if (pos == (CountGenListNode() + 1))
    {
        AddToGenListLast(pNodeData);
    }
    else if (pos > CountGenListNode())
    {
        return FALSE;  // Position is out of range
    }
    else
    {
        // ── General case: allocate and splice ──
        pNewNode = (GEN_LIST<T> *)malloc(sizeof(GEN_LIST<T>));
        if (NULL == pNewNode)
        {
            return FALSE;  // Memory allocation failed
        }

        memset(pNewNode, 0, sizeof(GEN_LIST<T>));
        pNewNode->pNext = NULL;
        pNewNode->Data  = *pNodeData;

        // Walk to the node at position (pos - 1)
        temp = pHead;
        while (i < pos - 1)
        {
            temp = temp->pNext;
            ++i;
        }

        // Splice pNewNode between temp and temp->pNext
        pNewNode->pNext = temp->pNext;  // 1. new → old successor
        temp->pNext     = pNewNode;     // 2. predecessor → new
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteFromGenListFirst
//
//  Removes and frees the head node.
//
//  Steps:
//    1. Save pHead in temp.
//    2. Advance pHead to temp->pNext.
//    3. Free temp.
//
//  Returns FALSE if the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Singly_linkedList<T>::DeleteFromGenListFirst()
{
    GEN_LIST<T> *temp = pHead;

    if (pHead == NULL)
    {
        return FALSE;  // Nothing to delete
    }

    pHead = temp->pNext;  // Advance head; NULL if this was the only node
    free(temp);

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteFromGenListLast
//
//  Removes and frees the tail node.
//  Because the list is singly-linked, two pointers are needed to find the
//  penultimate node without a separate backward link.
//
//  temp1 races ahead; temp2 follows one step behind.
//  When temp1->pNext == NULL, temp1 is at the tail and temp2 is just before it.
//
//  Steps:
//    1. Advance both pointers until temp1->pNext == NULL.
//    2. Set temp2->pNext = NULL (new tail).
//    3. Free temp1 (old tail).
//
//  Returns FALSE if the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Singly_linkedList<T>::DeleteFromGenListLast()
{
    GEN_LIST<T> *temp1, *temp2;
    temp1 = temp2 = pHead;

    if (pHead == NULL)
    {
        return FALSE;  // Nothing to delete
    }

    // Walk temp1 to the tail, temp2 stays one step behind
    while (temp1->pNext != NULL)
    {
        temp2 = temp2->pNext;
        temp1 = temp2->pNext;
    }

    temp2->pNext = NULL;  // temp2 becomes the new tail
    free(temp1);          // Release old tail

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteAtPosition
//
//  Removes and frees the node at 1-based position `pos`.
//
//  Boundary delegation:
//    pos == 1    → DeleteFromGenListFirst
//    pos == size → DeleteFromGenListLast
//
//  General case: `t` trails one position behind `temp` so after the loop
//  temp is the target and t is its predecessor:
//    t->pNext    = temp->pNext  (predecessor skips target)
//    temp->pNext = NULL         (isolate before freeing)
//    free(temp)
//
//  Returns FALSE if pos > size.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Singly_linkedList<T>::DeleteAtPosition(int pos)
{
    if (pos > CountGenListNode())
    {
        return FALSE;  // Position is beyond the end of the list
    }
    else if (pos == 1)
    {
        DeleteFromGenListFirst();
    }
    else if (pos == CountGenListNode())
    {
        DeleteFromGenListLast();
    }
    else
    {
        int          i    = 0;
        GEN_LIST<T> *temp = pHead;
        GEN_LIST<T> *t    = NULL;  // Trails one step behind temp

        for (i = 0; i < pos - 1; i++)
        {
            t    = temp;
            temp = temp->pNext;
        }

        t->pNext    = temp->pNext;  // Predecessor skips over target
        temp->pNext = NULL;         // Isolate the target node
        free(temp);
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DisplayGenList
//
//  Traverses from pHead to NULL, printing each node's Data:
//    D1 -> D2 -> ... -> Dn ->
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
void
Singly_linkedList<T>::DisplayGenList()
{
    GEN_LIST<T> *pTempNode = pHead;

    while (NULL != pTempNode)
    {
        cout << pTempNode->Data << " -> ";
        pTempNode = pTempNode->pNext;
    }
}


// ─────────────────────────────────────────────────────────────────────────────
//  ReverseList
//
//  Reverses the list in-place using three pointers in a single O(n) pass.
//
//  At each step:
//    1. Save current->pNext in `next`.
//    2. Flip current->pNext to point at `prev` (reverse the link).
//    3. Advance `prev` to `current`, `current` to `next`.
//
//  After the loop, `prev` points to the old tail (new head).
//
//  Visual (3 nodes):
//    Before: pHead → [A] → [B] → [C] → NULL
//    After:  pHead → [C] → [B] → [A] → NULL
//
//  Returns FALSE if the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
Singly_linkedList<T>::ReverseList()
{
    GEN_LIST<T> *prev    = NULL;
    GEN_LIST<T> *current = pHead;
    GEN_LIST<T> *next    = NULL;

    if (pHead == NULL)
    {
        return FALSE;  // Nothing to reverse
    }

    while (current != NULL)
    {
        next            = current->pNext;  // 1. Save successor
        current->pNext  = prev;            // 2. Reverse the link
        prev            = current;         // 3. Advance prev
        current         = next;            // 4. Advance current
    }

    pHead = prev;  // prev now points to the former tail (new head)

    return TRUE;
}


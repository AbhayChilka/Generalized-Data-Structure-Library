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
//  GEN_SCLIST<T>
//
//  Generic node for a Singly Circular Linked List.
//
//  Each node stores:
//    • Data  – the payload of type T
//    • pNext – pointer to the next node (tail's pNext wraps back to head)
//
//  Memory layout (3-node example):
//
//       ┌──────────────────────────────┐  (circular wrap)
//       ▼                              │
//      [N1] ──pNext──► [N2] ──pNext──► [N3]
//       ↑                               ↑
//     pHead                           pTail
// ─────────────────────────────────────────────────────────────────────────────

template <typename T>
struct GEN_SCLIST
{
    T           Data;   // Payload stored in this node
    GEN_SCLIST *pNext;  // Pointer to the next node (pTail->pNext == pHead)
};


// ─────────────────────────────────────────────────────────────────────────────
//  SinglyCircular_linkedList<T>
//
//  A generic singly-linked circular list.
//
//  Key properties
//  ──────────────
//  • Singly linked – each node has only a forward (pNext) pointer; backward
//                    traversal is not directly supported.
//  • Circular      – pTail->pNext always equals pHead; there is no NULL
//                    terminator. Traversal wraps around seamlessly.
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
class SinglyCircular_linkedList
{
public:
    GEN_SCLIST<T> *pHead;  // Pointer to the first node (front of the list)
    GEN_SCLIST<T> *pTail;  // Pointer to the last  node (back  of the list)

    // ── Construction / Destruction ──────────────────────────────────────────

    // Constructor – initialises an empty list (pHead = pTail = NULL).
    SinglyCircular_linkedList();

    // Destructor – releases all heap-allocated nodes when the object goes
    // out of scope.
    ~SinglyCircular_linkedList();

    // ── Insertion ───────────────────────────────────────────────────────────

    // AddToListFirst   – prepend a new node at the front.
    // AddToListLast    – append  a new node at the back.
    // InsertAtPosition – insert at 1-based position `pos`; delegates to
    //                    AddToListFirst / AddToListLast for boundary cases.
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

    // DisplayList – print head → tail (one revolution):
    //   D1 -> D2 -> ... -> Dn -> (Head)
    void DisplayList();

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
//  SinglyCircular_linkedList  (constructor)
//
//  Sets pHead and pTail to NULL, representing an empty list.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
SinglyCircular_linkedList<T>::SinglyCircular_linkedList()
{
    pHead = NULL;
    pTail = NULL;
}


// ─────────────────────────────────────────────────────────────────────────────
//  ~SinglyCircular_linkedList  (destructor)
//
//  Walks the circular list exactly once using a do-while loop and frees every
//  node. A do-while is required because the termination condition
//  (pNavigate != pHead) would be TRUE at the very start for a non-empty list —
//  we must enter the loop body at least once.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
SinglyCircular_linkedList<T>::~SinglyCircular_linkedList()
{
    if (NULL == pHead)
    {
        return;  // Already empty; nothing to free
    }

    GEN_SCLIST<T> *pNavigate = pHead;
    GEN_SCLIST<T> *pTemp     = NULL;

    do
    {
        pTemp     = pNavigate->pNext;  // Save successor before freeing
        free(pNavigate);
        pNavigate = pTemp;
    } while (pNavigate != pHead);      // Stop after one full revolution

    pHead = NULL;
    pTail = NULL;
}


// ─────────────────────────────────────────────────────────────────────────────
//  AddToListFirst
//
//  Prepends a new node and restores the circular invariant (pTail->pNext == pHead).
//
//  Empty list case:
//    Single self-pointing node: pHead == pTail, pTail->pNext == pHead.
//
//  Non-empty list case  (new node N, old head H, tail T):
//    Before:  T -> H -> ... -> T  (circular)
//    After:   T -> N -> H -> ... -> T  (N is new head)
//
//    Steps:
//      1. N.pNext    = H    (N points forward to old head)
//      2. pHead      = N    (advance head pointer)
//      3. pTail.pNext = N   (tail wraps back to new head)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
SinglyCircular_linkedList<T>::AddToListFirst(T *pNodeData)
{
    GEN_SCLIST<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_SCLIST<T> *)malloc(sizeof(GEN_SCLIST<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, then set the payload
    memset(pNewNode, 0, sizeof(GEN_SCLIST<T>));
    pNewNode->pNext = NULL;
    pNewNode->Data  = *pNodeData;

    if (NULL == pHead)
    {
        // ── First node: self-pointing circular list ──
        pHead        = pNewNode;
        pTail        = pNewNode;
        pTail->pNext = pHead;   // Only node points to itself
    }
    else
    {
        // ── Prepend: wire N before current head, update tail wrap ──
        pNewNode->pNext = pHead;  // 1. N → old head
        pHead           = pNewNode; // 2. advance head pointer
        pTail->pNext    = pHead;  // 3. tail wraps to new head
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  AddToListLast
//
//  Appends a new node and restores the circular invariant.
//
//  Empty list case: same as AddToListFirst.
//
//  Non-empty list case  (current tail T, new node N, head H):
//    Before: H -> ... -> T -> H  (circular)
//    After:  H -> ... -> T -> N -> H  (N is new tail)
//
//    Steps:
//      1. T.pNext    = N    (old tail points forward to N)
//      2. pTail      = N    (advance tail pointer)
//      3. pTail.pNext = H   (new tail wraps back to head)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
SinglyCircular_linkedList<T>::AddToListLast(T *pNodeData)
{
    GEN_SCLIST<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_SCLIST<T> *)malloc(sizeof(GEN_SCLIST<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, then set the payload
    memset(pNewNode, 0, sizeof(GEN_SCLIST<T>));
    pNewNode->pNext = NULL;
    pNewNode->Data  = *pNodeData;

    if (NULL == pHead)
    {
        // ── First node: self-pointing circular list ──
        pHead        = pNewNode;
        pTail        = pNewNode;
        pTail->pNext = pHead;
    }
    else
    {
        // ── Append: wire N after current tail, restore wrap ──
        pTail->pNext    = pNewNode;  // 1. old tail → N
        pTail           = pNewNode;  // 2. advance tail pointer
        pTail->pNext    = pHead;     // 3. new tail wraps back to head
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
//    Walk to node at position (pos - 1), then splice pNewNode:
//      1. newNode.pNext = temp.pNext   (new → successor)
//      2. temp.pNext    = newNode      (predecessor → new)
//
//  Returns FALSE if list is empty, pos > size+1, or allocation fails.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
SinglyCircular_linkedList<T>::InsertAtPosition(int pos, T *pNodeData)
{
    int            i        = 1;
    GEN_SCLIST<T> *temp     = NULL;
    GEN_SCLIST<T> *pNewNode = NULL;

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
        pNewNode = (GEN_SCLIST<T> *)malloc(sizeof(GEN_SCLIST<T>));
        if (NULL == pNewNode)
        {
            return FALSE;  // Memory allocation failed
        }

        memset(pNewNode, 0, sizeof(GEN_SCLIST<T>));
        pNewNode->pNext = NULL;
        pNewNode->Data  = *pNodeData;

        // Walk to the node at position (pos - 1)
        temp = pHead;
        while (i < pos - 1)
        {
            temp = temp->pNext;
            ++i;
        }

        // Splice: new node slots in between temp and temp->pNext
        pNewNode->pNext = temp->pNext;  // 1
        temp->pNext     = pNewNode;     // 2
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteFromListFirst
//
//  Removes and frees the head node, restoring the circular invariant.
//
//  Single-node case: free and set both pointers to NULL.
//
//  Multi-node case  (old head H, new head H2, tail T):
//    1. Save H in temp.
//    2. Advance pHead to H->pNext.
//    3. Re-link tail: pTail->pNext = new pHead.
//    4. Free temp.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
SinglyCircular_linkedList<T>::DeleteFromListFirst()
{
    GEN_SCLIST<T> *temp = NULL;

    if (NULL == pHead)
    {
        return FALSE;  // Nothing to delete
    }

    if (pHead == pTail)
    {
        // Only one node
        free(pHead);
        pHead = NULL;
        pTail = NULL;
    }
    else
    {
        temp         = pHead;            // Save old head
        pHead        = pHead->pNext;     // Advance head pointer
        pTail->pNext = pHead;            // Re-link tail → new head
        free(temp);                      // Release old head node
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteFromListLast
//
//  Removes and frees the tail node.
//  Because the list is singly-linked, finding the node just before pTail
//  requires a linear walk from pHead.
//
//  Single-node case: free and set both pointers to NULL.
//
//  Multi-node case:
//    1. Walk from pHead until temp->pNext == pTail (penultimate node).
//    2. Free pTail.
//    3. Set pTail = temp; pTail->pNext = pHead.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
SinglyCircular_linkedList<T>::DeleteFromListLast()
{
    GEN_SCLIST<T> *temp = pHead;

    if (NULL == pHead)
    {
        return FALSE;  // Nothing to delete
    }

    if (pHead == pTail)
    {
        // Only one node
        free(pHead);
        pHead = NULL;
        pTail = NULL;
    }
    else
    {
        // Walk until we reach the node just before pTail
        while (temp->pNext != pTail)
        {
            temp = temp->pNext;
        }

        free(pTail);             // Release old tail
        pTail        = temp;     // Penultimate node becomes new tail
        pTail->pNext = pHead;    // Restore circular link
    }

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
//  General case: use two pointers — `t` trails one step behind `temp` so
//  that after the loop t->pNext == temp (the target). Then:
//    t->pNext    = temp->pNext  (predecessor skips target)
//    temp->pNext = NULL         (isolate before freeing)
//    free(temp)
//
//  Returns FALSE if pos > size.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
SinglyCircular_linkedList<T>::DeleteAtPosition(int pos)
{
    int            i    = 0;
    GEN_SCLIST<T> *temp = NULL;
    GEN_SCLIST<T> *t    = NULL;  // Trails one node behind temp

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
        // Walk temp to position `pos`, keeping t one step behind
        temp = pHead;
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
//  DisplayList
//
//  Traverses one full revolution (do-while so the head is printed before
//  the termination check) and prints:
//    D1 -> D2 -> ... -> Dn -> (Head)
//
//  No-op when the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
void
SinglyCircular_linkedList<T>::DisplayList()
{
    GEN_SCLIST<T> *pTempNode = pHead;

    if (NULL == pHead)
    {
        return;  // Empty list – nothing to display
    }

    do
    {
        cout << pTempNode->Data << " -> ";
        pTempNode = pTempNode->pNext;
    } while (pTempNode != pHead);  // Stop after one full revolution

    cout << "(Head)" << endl;
}


// ─────────────────────────────────────────────────────────────────────────────
//  CountListNode
//
//  Counts nodes by completing exactly one revolution using a do-while loop.
//  Returns 0 for an empty list.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
SinglyCircular_linkedList<T>::CountListNode()
{
    int            CountNode = 0;
    GEN_SCLIST<T> *pListHead = pHead;

    if (NULL == pHead)
    {
        return 0;  // Empty list
    }

    do
    {
        CountNode++;
        pListHead = pListHead->pNext;
    } while (pListHead != pHead);  // Stop after one full revolution

    return CountNode;
}


// ─────────────────────────────────────────────────────────────────────────────
//  SearchFirstOccurance
//
//  Scans one full revolution and returns the 1-based position of the first
//  node whose Data matches `data` byte-for-byte (memcmp).
//
//  Returns -1 if not found or the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
SinglyCircular_linkedList<T>::SearchFirstOccurance(T data)
{
    int            CountNode = 0;
    GEN_SCLIST<T> *pListHead = pHead;

    if (NULL == pHead)
    {
        return -1;  // Empty list
    }

    do
    {
        ++CountNode;

        if (memcmp(&(pListHead->Data), &data, sizeof(data)) == 0)
        {
            return CountNode;  // Match found – return 1-based position
        }

        pListHead = pListHead->pNext;
    } while (pListHead != pHead);

    return -1;  // Not found
}


// ─────────────────────────────────────────────────────────────────────────────
//  FreeList
//
//  Releases every node in one revolution and resets pHead/pTail to NULL.
//  Callable on-demand to clear the list without destroying the object.
//
//  Returns TRUE on success, FALSE if the list is already empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
SinglyCircular_linkedList<T>::FreeList()
{
    GEN_SCLIST<T> *pTemp     = NULL;
    GEN_SCLIST<T> *pNavigate = NULL;

    if (NULL == pHead)
    {
        return FALSE;  // Nothing to free
    }

    pNavigate = pHead;
    do
    {
        pTemp     = pNavigate->pNext;  // Save successor before freeing
        free(pNavigate);
        pNavigate = pTemp;
    } while (pNavigate != pHead);      // Stop after one full revolution

    pHead = NULL;
    pTail = NULL;

    return TRUE;
}


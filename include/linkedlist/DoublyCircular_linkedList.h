#pragma once
#include <iostream>
#include <string.h>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
//  Convenience type aliases
// ─────────────────────────────────────────────────────────────────────────────
#define TRUE  1
#define FALSE 0

typedef int           BOOL;   // Boolean return type (TRUE / FALSE)
typedef unsigned char BYTE;   // Single-byte unsigned integer


// ─────────────────────────────────────────────────────────────────────────────
//  GEN_DCLIST<T>
//
//  Generic node structure for a Doubly Circular Linked List.
//
//  Each node stores:
//    • Data  – the payload of type T
//    • pNext – pointer to the next node (wraps around from tail → head)
//    • pPrev – pointer to the previous node (wraps around from head → tail)
//
//  Memory layout (3-node example):
//
//       ┌──────┐  pNext  ┌──────┐  pNext  ┌──────┐
//  ┌───►│  N1  │────────►│  N2  │────────►│  N3  │──┐
//  │|    │      │◄────────│      │◄────────│      │  │  (circular)
//  └────│      │  pPrev  │      │  pPrev  │      │◄─┘
//       └──────┘         └──────┘         └──────┘
//        (Head)                             (Tail)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
struct GEN_DCLIST
{
    T           Data;   // Payload stored in this node
    GEN_DCLIST *pNext;  // Pointer to the next node in the list
    GEN_DCLIST *pPrev;  // Pointer to the previous node in the list
};


// ─────────────────────────────────────────────────────────────────────────────
//  DoublyCircular_linkedList<T>
//
//  A generic, doubly-linked circular list.
//
//  Key properties
//  ──────────────
//  • Doubly linked  – every node has both a forward (pNext) and backward
//                     (pPrev) pointer, enabling O(1) insertion/deletion when
//                     a node pointer is already known.
//  • Circular       – pTail->pNext == pHead and pHead->pPrev == pTail.
//                     Traversal naturally wraps around without a sentinel node.
//  • Generic        – parameterised on type T via C++ templates; works with
//                     any copyable/comparable type.
//
//  Public data members
//  ───────────────────
//  pHead – points to the first (front) node; NULL when the list is empty.
//  pTail – points to the last  (back)  node; NULL when the list is empty.
//
//  Thread safety: NOT thread-safe. External synchronisation is required when
//  the list is shared across threads.
// ─────────────────────────────────────────────────────────────────────────────
template <class T>
class DoublyCircular_linkedList
{
public:

    GEN_DCLIST<T> *pHead;  // Pointer to the first node (front of the list)
    GEN_DCLIST<T> *pTail;  // Pointer to the last  node (back  of the list)

    // ── Construction / Destruction ──────────────────────────────────────────

    // Default constructor – initialises an empty list (pHead = pTail = NULL).

    DoublyCircular_linkedList();

    // Destructor – releases all heap-allocated nodes.
    // Equivalent to calling FreeList(), but also invoked automatically when
    // the object goes out of scope.

    ~DoublyCircular_linkedList();

    // ── Insertion ───────────────────────────────────────────────────────────

    // AddToListFirst  – prepend a new node at the front of the list.
    // AddToListLast   – append  a new node at the back  of the list.
    // InsertAtPosition – insert a node so that it occupies 1-based position
    //                    `pos`; delegates to AddToListFirst / AddToListLast
    //                    when pos == 1 or pos == size+1.
    //
    // Parameters:
    //   pNodeData – pointer to the data to copy into the new node; must not
    //               be NULL.
    // Returns:
    //   TRUE  on success.
    //   FALSE if pNodeData is NULL, memory allocation fails, or (for
    //         InsertAtPosition) pos is out of range.

    BOOL AddToListFirst(T *pNodeData);
    BOOL AddToListLast (T *pNodeData);
    BOOL InsertAtPosition(int pos, T *pNodeData);

    // ── Deletion ─────────────────────────────────────────────────────────────

    // DeleteFromListFirst – remove and free the first node.
    // DeleteFromListLast  – remove and free the last  node.
    // DeleteAtPosition    – remove and free the node at 1-based position `pos`.
    //
    // Returns:
    //   TRUE  on success.
    //   FALSE if the list is empty, or pos is out of range.

    BOOL DeleteFromListFirst();
    BOOL DeleteFromListLast();
    BOOL DeleteAtPosition(int pos);

    // ── Display ──────────────────────────────────────────────────────────────

    // DisplayList        – print all nodes from head to tail in the form:
    //                        D1 <-> D2 <-> ... <-> Dn <-> (Head)
    // DisplayListReverse – print all nodes from tail to head in the form:
    //                        Dn <-> ... <-> D1 <-> (Tail)
    // Both functions are no-ops when the list is empty.

    void DisplayList();
    void DisplayListReverse();

    // ── Utility ──────────────────────────────────────────────────────────────

    // CountListNode – return the total number of nodes currently in the list.
    //   Returns 0 for an empty list.

    int CountListNode();

    // SearchFirstOccurance – scan the list from head to tail and return the
    //   1-based position of the first node whose Data equals `data`
    //   (compared byte-by-byte via memcmp).
    //   Returns -1 if the value is not found or the list is empty.

    int SearchFirstOccurance(T data);

    // FreeList – release every node and reset pHead / pTail to NULL.
    //   Unlike the destructor this can be called manually to clear the list
    //   while keeping the object alive for re-use.
    //   Returns TRUE on success, FALSE if the list is already empty.

    BOOL FreeList();

};


// ─────────────────────────────────────────────────────────────────────────────
//                        I M P L E M E N T A T I O N
// ─────────────────────────────────────────────────────────────────────────────


// ─────────────────────────────────────────────────────────────────────────────
//  DoublyCircular_linkedList  (constructor)
//
//  Initialises the list to an empty state.
//  Both pHead and pTail are set to NULL to indicate no nodes exist yet.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
DoublyCircular_linkedList<T>::DoublyCircular_linkedList()
{
    pHead = NULL;
    pTail = NULL;
}


// ─────────────────────────────────────────────────────────────────────────────
//  ~DoublyCircular_linkedList  (destructor)
//
//  Walks the circular list once and frees every node.
//  Uses a do-while loop (instead of a regular while) because the termination
//  condition pNavigate != pHead would be TRUE at the very start for a
//  non-empty list — we need to enter the loop at least once.
//
//  After freeing, pHead and pTail are set to NULL so accidental use-after-free
//  is easier to detect.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
DoublyCircular_linkedList<T>::~DoublyCircular_linkedList()
{
    if (NULL == pHead)
    {
        return;  // Already empty; nothing to free
    }

    GEN_DCLIST<T> *pNavigate = pHead;
    GEN_DCLIST<T> *pTemp     = NULL;

    do
    {
        pTemp     = pNavigate->pNext;  // Save the successor before freeing
        free(pNavigate);               // Release current node
        pNavigate = pTemp;             // Advance to the saved successor
    } while (pNavigate != pHead);      // Stop once we've looped back to head

    pHead = NULL;
    pTail = NULL;
}


// ─────────────────────────────────────────────────────────────────────────────
//  AddToListFirst
//
//  Allocates a new node, copies *pNodeData into it, and prepends it to the
//  front of the circular list.
//
//  Empty list case:
//    The single node points to itself in both directions, satisfying the
//    circular invariant: pHead->pNext == pHead, pHead->pPrev == pHead.
//
//  Non-empty list case  (new node N, old head H, current tail T):
//    Before:  T <-> H <-> ... <-> T  (circular)
//    After:   T <-> N <-> H <-> ... <-> T  (N is new head)
//
//    Steps:
//      1. N.pNext = H          (N points forward to old head)
//      2. N.pPrev = T          (N points backward to tail)
//      3. H.pPrev = N          (old head now points back to N)
//      4. T.pNext = N          (tail now points forward to N)
//      5. pHead   = N          (update the head pointer)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
DoublyCircular_linkedList<T>::AddToListFirst(T *pNodeData)
{
    GEN_DCLIST<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_DCLIST<T> *)malloc(sizeof(GEN_DCLIST<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, then set the payload and null the link pointers
    memset(pNewNode, 0, sizeof(GEN_DCLIST<T>));
    pNewNode->pNext = NULL;
    pNewNode->pPrev = NULL;
    pNewNode->Data  = *pNodeData;

    if (NULL == pHead)
    {
        // ── First node: make it point to itself (circular single-node list) ──
        pHead        = pNewNode;
        pTail        = pNewNode;
        pHead->pNext = pTail;   // head.next → tail (same node)
        pHead->pPrev = pTail;   // head.prev → tail (same node)
        pTail->pNext = pHead;   // tail.next → head (same node)
        pTail->pPrev = pHead;   // tail.prev → head (same node)
    }
    else
    {
        // ── Prepend: splice new node between pTail and current pHead ──
        pNewNode->pNext = pHead;  // 1. new → old head
        pNewNode->pPrev = pTail;  // 2. new → tail
        pHead->pPrev    = pNewNode; // 3. old head ← new
        pTail->pNext    = pNewNode; // 4. tail → new
        pHead           = pNewNode; // 5. advance head pointer
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  AddToListLast
//
//  Allocates a new node, copies *pNodeData into it, and appends it to the
//  back of the circular list.
//
//  Empty list case:  identical to AddToListFirst (single self-pointing node).
//
//  Non-empty list case  (new node N, current tail T, head H):
//    Before:  H <-> ... <-> T <-> H  (circular)
//    After:   H <-> ... <-> T <-> N <-> H  (N is new tail)
//
//    Steps:
//      1. T.pNext = N          (old tail now points forward to N)
//      2. N.pPrev = T          (N points backward to old tail)
//      3. N.pNext = H          (N wraps forward back to head)
//      4. H.pPrev = N          (head now points backward to N)
//      5. pTail   = N          (update the tail pointer)
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
DoublyCircular_linkedList<T>::AddToListLast(T *pNodeData)
{
    GEN_DCLIST<T> *pNewNode;

    if (NULL == pNodeData)
    {
        return FALSE;  // Guard: refuse NULL data pointer
    }

    // Allocate memory for the new node
    pNewNode = (GEN_DCLIST<T> *)malloc(sizeof(GEN_DCLIST<T>));
    if (NULL == pNewNode)
    {
        return FALSE;  // Memory allocation failed
    }

    // Zero-initialise, then set the payload and null the link pointers
    memset(pNewNode, 0, sizeof(GEN_DCLIST<T>));
    pNewNode->pNext = NULL;
    pNewNode->pPrev = NULL;
    pNewNode->Data  = *pNodeData;

    if (NULL == pHead)
    {
        // ── First node: self-pointing circular list ──
        pHead        = pNewNode;
        pTail        = pNewNode;
        pHead->pNext = pTail;
        pHead->pPrev = pTail;
        pTail->pNext = pHead;
        pTail->pPrev = pHead;
    }
    else
    {
        // ── Append: splice new node between current pTail and pHead ──
        pTail->pNext    = pNewNode;  // 1. old tail → new
        pNewNode->pPrev = pTail;     // 2. new ← old tail
        pNewNode->pNext = pHead;     // 3. new → head (maintain circular link)
        pHead->pPrev    = pNewNode;  // 4. head ← new
        pTail           = pNewNode;  // 5. advance tail pointer
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  InsertAtPosition
//
//  Inserts a new node so it occupies the given 1-based position `pos`.
//
//  Special cases handled via delegation:
//    pos == 1              → AddToListFirst  (prepend)
//    pos == size + 1       → AddToListLast   (append after last element)
//
//  General case (2 ≤ pos ≤ size):
//    Walk from pHead to the node at position (pos - 1), then splice the new
//    node between that node (temp) and temp->pNext.
//
//    Splice steps (temp = node before insertion point, S = temp->pNext):
//      1. newNode.pNext = S            (new → S)
//      2. newNode.pPrev = temp         (new ← temp)
//      3. S.pPrev       = newNode      (S ← new)
//      4. temp.pNext    = newNode      (temp → new)
//
//  Returns FALSE if:
//    • The list is empty.
//    • pos > size + 1  (position beyond one past the end).
//    • Memory allocation fails.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
DoublyCircular_linkedList<T>::InsertAtPosition(int pos, T *pNodeData)
{
    int            i        = 1;
    GEN_DCLIST<T> *temp     = NULL;
    GEN_DCLIST<T> *pNewNode = NULL;

    if (NULL == pHead)
    {
        return FALSE;  // Cannot insert by position into an empty list
    }
    else if (pos == 1)
    {
        // Inserting at the front – delegate to AddToListFirst
        AddToListFirst(pNodeData);
    }
    else if (pos == (CountListNode() + 1))
    {
        // Inserting one past the last element – delegate to AddToListLast
        AddToListLast(pNodeData);
    }
    else if (pos > CountListNode())
    {
        return FALSE;  // Position is out of range
    }
    else
    {
        // ── General case: allocate and splice ──
        pNewNode = (GEN_DCLIST<T> *)malloc(sizeof(GEN_DCLIST<T>));
        if (NULL == pNewNode)
        {
            return FALSE;  // Memory allocation failed
        }

        memset(pNewNode, 0, sizeof(GEN_DCLIST<T>));
        pNewNode->pNext = NULL;
        pNewNode->pPrev = NULL;
        pNewNode->Data  = *pNodeData;

        // Walk to the node at position (pos - 1)
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
//  Removes and frees the first (head) node of the list.
//
//  Single-node case:
//    Free the only node and set both pHead and pTail to NULL.
//
//  Multi-node case  (old head H, new head H2, tail T):
//    1. Save H in temp.
//    2. Advance pHead to H->pNext (= H2).
//    3. Restore the circular links: H2.pPrev = T, T.pNext = H2.
//    4. Free temp (old H).
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
DoublyCircular_linkedList<T>::DeleteFromListFirst()
{
    GEN_DCLIST<T> *temp = NULL;

    if (NULL == pHead)
    {
        return FALSE;  // Nothing to delete
    }

    if (pHead == pTail)
    {
        // Only one node in the list
        free(pHead);
        pHead = NULL;
        pTail = NULL;
    }
    else
    {
        temp         = pHead;            // Save old head
        pHead        = pHead->pNext;     // Advance head pointer
        pHead->pPrev = pTail;            // Re-link: new head ← tail
        pTail->pNext = pHead;            // Re-link: tail → new head
        free(temp);                      // Release old head node
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteFromListLast
//
//  Removes and frees the last (tail) node of the list.
//
//  Single-node case:
//    Free the only node and set both pHead and pTail to NULL.
//
//  Multi-node case  (head H, old tail T, new tail T2):
//    1. Save T in temp.
//    2. Retreat pTail to T->pPrev (= T2).
//    3. Restore the circular links: T2.pNext = H, H.pPrev = T2.
//    4. Free temp (old T).
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
DoublyCircular_linkedList<T>::DeleteFromListLast()
{
    GEN_DCLIST<T> *temp = NULL;

    if (NULL == pHead)
    {
        return FALSE;  // Nothing to delete
    }

    if (pHead == pTail)
    {
        // Only one node in the list
        free(pHead);
        pHead = NULL;
        pTail = NULL;
    }
    else
    {
        temp         = pTail;            // Save old tail
        pTail        = pTail->pPrev;     // Retreat tail pointer
        pTail->pNext = pHead;            // Re-link: new tail → head
        pHead->pPrev = pTail;            // Re-link: head ← new tail
        free(temp);                      // Release old tail node
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DeleteAtPosition
//
//  Removes and frees the node at the given 1-based position `pos`.
//
//  Special cases handled via delegation:
//    pos == 1    → DeleteFromListFirst
//    pos == size → DeleteFromListLast
//
//  General case (2 ≤ pos ≤ size - 1):
//    Walk from pHead to the node at position `pos`, then unlink it:
//      temp->pPrev->pNext = temp->pNext  (predecessor skips over temp)
//      temp->pNext->pPrev = temp->pPrev  (successor skips over temp)
//      free(temp)
//
//  Returns FALSE if pos is out of range (> size).
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
DoublyCircular_linkedList<T>::DeleteAtPosition(int pos)
{
    int            i    = 0;
    GEN_DCLIST<T> *temp = NULL;

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
        // Walk to the node at position `pos` (loop advances i from 1 to pos)
        temp = pHead;
        for (i = 1; i < pos; i++)
        {
            temp = temp->pNext;
        }

        // Unlink temp from its neighbours
        temp->pPrev->pNext = temp->pNext;  // predecessor skips over temp
        temp->pNext->pPrev = temp->pPrev;  // successor  skips over temp
        free(temp);
    }

    return TRUE;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DisplayList
//
//  Traverses the list from pHead and prints each node's Data in the format:
//    D1 <-> D2 <-> ... <-> Dn <-> (Head)
//
//  Uses a do-while loop so the check pTempNode != pHead happens *after* the
//  first node is printed (otherwise the loop body would never execute because
//  pTempNode starts equal to pHead).
//
//  No-op when the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
void
DoublyCircular_linkedList<T>::DisplayList()
{
    GEN_DCLIST<T> *pTempNode = pHead;

    if (NULL == pHead)
    {
        return;  // Empty list – nothing to display
    }

    do
    {
        cout << pTempNode->Data << " <-> ";
        pTempNode = pTempNode->pNext;
    } while (pTempNode != pHead);  // Stop after one full revolution

    cout << "(Head)" << endl;
}


// ─────────────────────────────────────────────────────────────────────────────
//  DisplayListReverse
//
//  Traverses the list backwards from pTail and prints each node's Data:
//    Dn <-> ... <-> D1 <-> (Tail)
//
//  Uses the same do-while trick as DisplayList, starting from pTail and
//  following pPrev pointers until we complete one full revolution.
//
//  No-op when the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
void
DoublyCircular_linkedList<T>::DisplayListReverse()
{
    GEN_DCLIST<T> *pTempNode = pTail;

    if (NULL == pHead)
    {
        return;  // Empty list – nothing to display
    }

    do
    {
        cout << pTempNode->Data << " <-> ";
        pTempNode = pTempNode->pPrev;
    } while (pTempNode != pTail);  // Stop after one full (backward) revolution

    cout << "(Tail)" << endl;
}


// ─────────────────────────────────────────────────────────────────────────────
//  CountListNode
//
//  Counts nodes by walking the list exactly once using a do-while loop.
//  Returns 0 for an empty list without entering the loop.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
DoublyCircular_linkedList<T>::CountListNode()
{
    int            CountNode = 0;
    GEN_DCLIST<T> *pListHead = pHead;

    if (NULL == pHead)
    {
        return 0;  // Empty list
    }

    do
    {
        CountNode++;                       // Tally this node
        pListHead = pListHead->pNext;      // Advance to next node
    } while (pListHead != pHead);          // Stop after one full revolution

    return CountNode;
}


// ─────────────────────────────────────────────────────────────────────────────
//  SearchFirstOccurance
//
//  Performs a linear scan from pHead and returns the 1-based position of the
//  first node whose Data field matches `data` byte-for-byte (via memcmp).
//
//  Using memcmp makes the search work for any type T including plain structs
//  without a custom operator==, though it may give unexpected results for
//  types with padding bytes or pointer members.
//
//  Returns:
//    Position (≥ 1) on a successful match.
//    -1 if the value is not found or the list is empty.
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
int
DoublyCircular_linkedList<T>::SearchFirstOccurance(T data)
{
    int            CountNode = 0;
    GEN_DCLIST<T> *pListHead = pHead;

    if (NULL == pHead)
    {
        return -1;  // Empty list – search is trivially unsuccessful
    }

    do
    {
        ++CountNode;

        // Compare the node's Data against the target byte-by-byte
        if (memcmp(&(pListHead->Data), &data, sizeof(data)) == 0)
        {
            return CountNode;  // Found – return 1-based position
        }

        pListHead = pListHead->pNext;
    } while (pListHead != pHead);  // Stop after one full revolution

    return -1;  // Not found
}


// ─────────────────────────────────────────────────────────────────────────────
//  FreeList
//
//  Releases every node in the list and resets pHead / pTail to NULL.
//  Identical traversal logic to the destructor, but callable on-demand to
//  clear and reuse the list object without destroying it.
//
//  Returns:
//    TRUE  on success.
//    FALSE if the list is already empty (nothing to free).
// ─────────────────────────────────────────────────────────────────────────────
template <typename T>
BOOL
DoublyCircular_linkedList<T>::FreeList()
{
    GEN_DCLIST<T> *pTemp     = NULL;
    GEN_DCLIST<T> *pNavigate = NULL;

    if (NULL == pHead)
    {
        return FALSE;  // Already empty; nothing to do
    }

    pNavigate = pHead;
    do
    {
        pTemp     = pNavigate->pNext;  // Save the successor before freeing
        free(pNavigate);               // Release current node
        pNavigate = pTemp;             // Advance to the saved successor
    } while (pNavigate != pHead);      // Stop after one full revolution

    pHead = NULL;
    pTail = NULL;

    return TRUE;
}


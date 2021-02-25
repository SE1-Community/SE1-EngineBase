
#ifndef SE_INCL_LISTITERATOR_INL
#define SE_INCL_LISTITERATOR_INL
#ifdef PRAGMA_ONCE
#pragma once
#endif

// Simple list iterator: 4 bytes structure, all functions are inline
template<class Cbase, int iOffset> class CListIter {
  private:
    CListNode *li_CurrentNode;

  public:
    // Constructor - no list attached
    CListIter(void) {
      li_CurrentNode = NULL;
    };

    // Constructor with list attaching
    CListIter(const CListHead &lhHead) {
      li_CurrentNode = &lhHead.IterationHead();
    };

    // Constructor to start from given node
    CListIter(CListNode &lnNode) {
      ASSERT(lnNode.IsLinked());
      li_CurrentNode = &lnNode;
    };

    // Start iterating
    void Reset(const CListHead &lhHead) {
      li_CurrentNode = &lhHead.IterationHead();
    };

    // Move to next node
    void MoveToNext(void) {
      li_CurrentNode = &li_CurrentNode->IterationSucc();
    };

    // Move to previous node
    void MoveToPrev(void) {
      li_CurrentNode = &li_CurrentNode->IterationPred();
    };

    // Check if finished
    BOOL IsPastEnd(void) {
      return li_CurrentNode->IsTailMarker();
    };

    // Insert a node after current one
    inline void InsertAfterCurrent(CListNode &lnNew) {
      li_CurrentNode->IterationInsertAfter(lnNew);
    };

    // Insert a node before current one
    inline void InsertBeforeCurrent(CListNode &lnNew) {
      li_CurrentNode->IterationInsertBefore(lnNew);
    };

    // Get current element
    Cbase &Current(void) {
      return *((Cbase *)((UBYTE *)li_CurrentNode - iOffset));
    }

    Cbase &operator*(void) {
      return *((Cbase *)((UBYTE *)li_CurrentNode - iOffset));
    }

    operator Cbase *(void) {
      return ((Cbase *)((UBYTE *)li_CurrentNode - iOffset));
    }

    Cbase *operator->(void) {
      return ((Cbase *)((UBYTE *)li_CurrentNode - iOffset));
    }
};

// Taken from stddef.h
//#ifndef offsetof
//#define offsetof(s,m)	(size_t)&(((s *)0)->m)
//#endif

// Declare a list iterator for a class with a CListNode member
#define LISTITER(baseclass, member) CListIter<baseclass, offsetof(baseclass, member)>

// Make 'for' construct for walking a list
#define FOREACHINLIST(baseclass, member, head, iter) \
  for (LISTITER(baseclass, member) iter(head); !iter.IsPastEnd(); iter.MoveToNext())

// Make 'for' construct for walking a list, keeping the iterator for later use
#define FOREACHINLISTKEEP(baseclass, member, head, iter) \
  LISTITER(baseclass, member) iter(head); \
  for (; !iter.IsPastEnd(); iter.MoveToNext())

// Make 'for' construct for deleting a list
#define FORDELETELIST(baseclass, member, head, iter) \
  for (LISTITER(baseclass, member) iter(head), iter##next; \
       iter##next = iter, iter##next.IsPastEnd() || (iter##next.MoveToNext(), 1), !iter.IsPastEnd(); iter = iter##next)

// Get the pointer to the first element in the list
#define LIST_HEAD(listhead, baseclass, member) ((baseclass *)(((UBYTE *)(&(listhead).Head())) - offsetof(baseclass, member)))

// Get the pointer to the last element in the list
#define LIST_TAIL(listhead, baseclass, member) ((baseclass *)(((UBYTE *)(&(listhead).Tail())) - offsetof(baseclass, member)))

// Get the pointer to the predecessor of the element
#define LIST_PRED(element, baseclass, member) ((baseclass *)(((UBYTE *)(&(element).member.Pred())) - offsetof(baseclass, member)))

// Get the pointer to the successor of the element
#define LIST_SUCC(element, baseclass, member) ((baseclass *)(((UBYTE *)(&(element).member.Succ())) - offsetof(baseclass, member)))

#endif /* include-once check. */

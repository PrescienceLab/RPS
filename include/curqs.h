// Pointer based Queue, Stack, and Priority Queue classes


// Interface 

#ifndef __NewQueues
#define __NewQueues

#ifndef NULL
#define NULL ((void*)0x0)
#endif

template <class T> 
class QueueNode {
 public:
	T *data;
	class QueueNode<T> *next, *prev;
};

template <class T> 
class Queue {
protected:
	class QueueNode<T> *first, *last, *current;
	T *ReturnCurrent() { return (current==NULL) ? (T*) NULL : current->data;};
public:
	Queue();
	~Queue();
	void AddAtFront(T *item);
	void AddAtBack(T* item);
	T *RemoveFromFront();
	T *RemoveFromBack();
	T *First() { current=first; return ReturnCurrent();};
        T *Last() { current=last; return ReturnCurrent();};
	T *Next() { current=current->next; return ReturnCurrent();}
	T *Previous() { current=current->prev; return ReturnCurrent();}
	void Clear();
        int GetNumItems();
	inline int IsEmpty() { return first==(QueueNode<T>*)NULL;};
} ;


template <class T> 
class Stack : public Queue<T> {
public:
	inline void Push(T *item) { Queue<T>::AddAtFront(item); };
	inline T   *Pop() { return Queue<T>::RemoveFromFront();};
} ;




// static int C::Compare(T*left, T*right) is expected
template <class T, class C> 
class SearchableQueue : public Queue<T> {
protected:
        QueueNode<T> *InternalFind(T *item);
public:
	int IsInQueue(T *item);
	T* FindRemove(T *item);
	T* Find(T *item);
};

template <class T, class C> 
class SearchableStack : public SearchableQueue<T,C> {
public:
	inline void Push(T *item) { Queue<T>::AddAtFront(item); };
	inline T   *Pop() { return Queue<T>::RemoveFromFront();};
} ;

template <class T> 
class PriorityQueue : public Queue<T> {
public:
	void AddInOrder(T *item, double priority);
	inline T *GetNextItem() { return Queue<T>::RemoveFromFront();};
};

template <class T, class C> 
class SearchablePriorityQueue : public SearchableQueue<T,C> {
public:
	void AddInOrder(T *item, double priority);
	inline T *GetNextItem() { return Queue<T>::RemoveFromFront();};
};

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

template <class T, class C> 
class PriorityQueueNode : public QueueNode<T> {
	friend class PriorityQueue<T>;
private:
	double priority;
};




template <class T> 
Queue<T>::Queue()
{
	first=last=(QueueNode<T>*)NULL;
} ;

template <class T> 
Queue<T>::~Queue()
{
	while (RemoveFromFront()!=(T*)NULL)
		;
};

template <class T>
int Queue<T>::GetNumItems()
{
  int num;
  QueueNode<T> *c;

  for (num=0,c=first;c!=0;c=c->next,num++) {
  }
  
  return num;
};
  

template <class T> 
void Queue<T>::Clear()
{
	T *item;

	while ((item= RemoveFromFront())!=(T*)NULL) {
		delete item;
	}
};

template <class T> 
void Queue<T>::AddAtFront(T *item)
{
	QueueNode<T> *node=new QueueNode<T>;

	node->data=item;
	if (first==(QueueNode<T>*)NULL) {
		node->prev=(QueueNode<T>*)NULL;
		node->next=(QueueNode<T>*)NULL;
		first=last=node;
	} else {
		node->prev=(QueueNode<T>*)NULL;
		node->next=first;
		first->prev=node;
		first=node;
	}
};

template <class T> 
void Queue<T>::AddAtBack(T *item)
{
	QueueNode<T> *node=new QueueNode<T>;

	node->data=item;
	if (last==(QueueNode<T>*)NULL) {
		node->prev=(QueueNode<T>*)NULL;
		node->next=(QueueNode<T>*)NULL;
		first=last=node;
	} else {
		node->next=(QueueNode<T>*)NULL;
		node->prev=last;
		last->next=node;
		last=node;
	}
};

template <class T> 
T * Queue<T>::RemoveFromFront()
{
	if (first==(QueueNode<T>*)NULL) {
		return (T*)NULL;
	} else {
		QueueNode<T> *node=first;
		T *item=node->data;

		first=node->next;
		if (first==(QueueNode<T>*)NULL) {
			first=last=(QueueNode<T>*)NULL;
		} else {
			first->prev=(QueueNode<T>*)NULL;
		}

		delete node;
		return item;
	}
};

template <class T> 
T * Queue<T>::RemoveFromBack()
{
	if (last==(QueueNode<T>*)NULL) {
		return (T*)NULL;
	} else {
		QueueNode<T> *node=last;
		T *item=node->data;

		last=node->prev;
		if (last==(QueueNode<T>*)NULL) {
			first=last=(QueueNode<T>*)NULL;
		} else {
			last->next=(QueueNode<T>*)NULL;
		}

		delete node;
		return item;
	}
};

template <class T, class C> 
int SearchableQueue<T,C>::IsInQueue(T *item)
{
	QueueNode<T> *node;

	for (node=first;node!=(QueueNode<T>*)NULL;node=node->next) {
		if (C::Compare(item,node->data)==0) {
			return 1;
		}
	}

	return 0;
};





template <class T, class C> 
T * SearchableQueue<T,C>::FindRemove(T *item)
{
  QueueNode<T> *node = InternalFind(item);
  T *it=node->data;
  if (node->prev==(QueueNode<T>*)NULL) {
    first=node->next;
  } else {
    node->prev->next=node->next;
  }
  if (node->next==(QueueNode<T>*)NULL) {
    last=node->prev;
  } else {
    node->next->prev=node->prev;
  }
  delete node;
  return it;
}


template <class T,class C> 
QueueNode<T> * SearchableQueue<T,C>::InternalFind(T *theitem) 
{
  QueueNode<T> *node;
  for (node=first;node!=(QueueNode<T>*)NULL;node=node->next) {
    if (C::Compare(theitem,node->data)==0) 
      return node;
  }
  return 0;
}

template <class T, class C> 
T * SearchableQueue<T,C>::Find(T *item)
{
  QueueNode<T> *node = InternalFind(item);
  if (node!=0) {
    return node->data;
  } else {
    return 0;
  }
} ;


template <class T> 
void PriorityQueue<T>::AddInOrder(T *item, double pri)
{
	PriorityQueueNode<T> *node, *newnode;

	newnode=new PriorityQueueNode<T>;
	newnode->data=item;
	newnode->priority=pri;

	for (node=(PriorityQueueNode<T>*)first;node!=(PriorityQueueNode<T>*)NULL;node=node->next) {
		if (node->priority < pri) {
			break;
		}
	}

	if (node!=(PriorityQueueNode<T>*)NULL) {                   // We will insert before this node
		if (node->prev!=(PriorityQueueNode<T>*)NULL) {         // Case 1 - it is not the first node
			node->prev->next=newnode;
			newnode->prev=node->prev;
			newnode->next=node;
			node->prev=newnode;
		} else {                        // Case 2 - it is the first node
			node->prev=newnode;
			newnode->next=node;
			newnode->prev=(PriorityQueueNode<T>*)NULL;
			first=newnode;
		}
	} else {                            // We will insert at the end of the list
		if (first==(PriorityQueueNode<T>*)NULL) {              // Case 1 - the list is empty
			newnode->next=newnode->prev=(PriorityQueueNode<T>*)NULL;
			first=last=newnode;
		} else {                        // Case 2 - the list is not empty
			last->next=newnode;
			newnode->prev=last;
			newnode->next=(PriorityQueueNode<T>*)NULL;
			last=newnode;
		}
	}
} ;		

template <class T, class C> 
void SearchablePriorityQueue<T,C>::AddInOrder(T *item, double pri)
{
	PriorityQueueNode<T> *node, *newnode;

	newnode=new PriorityQueueNode<T>;
	newnode->data=item;
	newnode->priority=pri;

	

	for (node=(PriorityQueueNode<T> *) first;node!=(PriorityQueueNode<T>*)NULL;
		node=(PriorityQueueNode<T>*)node->next) {
		if (node->priority < pri) {
			break;
		}
	}

	if (node!=(PriorityQueueNode<T>*)NULL) {                   // We will insert before this node
		if (node->prev!=(PriorityQueueNode<T>*)NULL) {         // Case 1 - it is not the first node
			node->prev->next=newnode;
			newnode->prev=node->prev;
			newnode->next=node;
			node->prev=newnode;
		} else {                        // Case 2 - it is the first node
			node->prev=newnode;
			newnode->next=node;
			newnode->prev=(PriorityQueueNode<T>*)NULL;
			first=newnode;
		}
	} else {                            // We will insert at the end of the list
		if (first==(PriorityQueueNode<T>*)NULL) {              // Case 1 - the list is empty
			newnode->next=newnode->prev=(PriorityQueueNode<T>*)NULL;
			first=last=newnode;
		} else {                        // Case 2 - the list is not empty
			last->next=newnode;
			newnode->prev=last;
			newnode->next=(PriorityQueueNode<T>*)NULL;
			last=newnode;
		}
	}
} ;		

#endif

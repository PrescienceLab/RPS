#ifndef _HashTable
#define _HashTable

#define DEFAULT_SIZE 251

#define DELETE_FRACTION 2   // 2/10s

// T is the data type
// K is the key  type
// C provides static int C::CompareKeys(K first, K second)
// H provides static unsigned H::Hash(T)

template <class T, class K, class C, class H> class HashTable {
public:
  virtual ~HashTable() {};
  virtual void Add(T item, K key)=0;
  virtual void Delete(K key)=0;
  virtual void DeleteItem(K key)=0;
  virtual  T Lookup(K key)=0;
  virtual unsigned EnumerateKeys(unsigned max, K *keys)=0;
};

template <class T, class K> class ChainingHashTableItem {
private:
  T        item;
  K        key;
public:
  ChainingHashTableItem(T *i, K k) { item=i; key=k;};
  T GetItem() { return item;};
  K GetKey() { return key; }
};

template <class T, class K, class C, class H> 
class ChainingHashTable : public HashTable<T,K,C,H> {
private:
  Queue<ChainingHashTableItem<T,K> > *list;
  C compare;
  H hash;

protected:
  unsigned size;
  virtual unsigned HashFunction(K key);
  virtual ChainingHashTableItem<T,K> *InternalLookup(K key);

public:
  ChainingHashTable(unsigned size=DEFAULT_SIZE);
  ~ChainingHashTable();

  void ClearAll();
  void Add(T item, K key);
  void Delete(K key);
  void DeleteItem(K key);
  T Lookup(K key);
  unsigned EnumerateKeys(unsigned max, K *keys);
};

template <class T, class K, class C, class H> 
ChainingHashTable<T,K,C,H>::ChainingHashTable(unsigned s)
{
  size=s;
  list = new SearchableQueue<ChainingHashTableItem<T,K> >[size];
};

template <class T, class K, class C, class H> 
ChainingHashTable<T,K,C,H>::~ChainingHashTable()
{
  unsigned i;
  for (i=0;i<size;i++) {
    list[i].Clear(); // Will not wipe out the data items, only the HT items
  }
  delete [] list;
};

template <class T, class K, class C, class H> 
unsigned ChainingHashTable<T,K,C,H>::EnumerateKeys(unsigned max, K *keys)
{
  unsigned i,j;
  ChainingHashTableItem<T,K> *it;

  for (i=0,j=0;i<size && j<max;i++) {
    for (it=list[i].First(); it!=0 && j<max; it=list[i].Next(), j++) {
      keys[j]=it->GetKey();
    }
  }
  return j;
};

template <class T, class K, class C, class H> 
unsigned ChainingHashTable<T,K,C,H>::HashFunction(K key) 
{
  return hash.Hash(K,size);
};

template <class T, class K, class C, class H> 
void ChainingHashTable<T,K,C,H>::Add(T item, K key) 
{
  list[HashFunction(key)].AddAtBack(new ChainingHashTableItem<T,K>(item,key));
};


template <class T, class K, class C, class H> 
ChainingHashTableItem<T,K,C,H>  *ChainingHashTable<T,K,C,H>::InternalLookup(K key) 
{
  
  ChainingHashTableItem<T,K> testitem(0,key);

  SearchableQueue<ChainingHashTableItem<T,K> > 
          &thelist = list[HashFunction(key)];
  
  return thelist.Find(&testitem);
};


template <class T> void ChainingHashTable<T>::Delete(unsigned key)
{
  ChainingHashTableItem<T> testitem(0,key);

  SearchableQueue<ChainingHashTableItem<T> > &thelist = list[HashFunction(key)];
  
  ChainingHashTableItem<T> *it=thelist.FindRemove(&testitem);
  if (it!=0) {
    delete it;
  }
};

template <class T> void ChainingHashTable<T>::DeleteItem(unsigned key)
{
  ChainingHashTableItem<T> testitem(0,key);

  SearchableQueue<ChainingHashTableItem<T> > &thelist = list[HashFunction(key)];
  
  ChainingHashTableItem<T> *it=thelist.FindRemove(&testitem);
  if (it!=0) {
    delete it->GetItem();
    delete it;
  }
};

template <class T> T *ChainingHashTable<T>::Lookup(unsigned key) 
{
  ChainingHashTableItem<T> *it=InternalLookup(key);

  if (it==0) { 
    return 0;
  } else {
    return it->GetItem();
  }
};
  
#endif

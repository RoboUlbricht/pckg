#ifndef __rurarray_h
#define __rurarray_h

class RurTemplateBaseArray {
  protected:
  void** data; // pointre

  private:
  int delta;        // prirastok
  int poc;          // pocet poloziek
  int max;          // maximum alokovaneho priestoru
  bool has_objects; // ci vlastni vsetky objekty

  public:
  RurTemplateBaseArray(int beg, int delt);
  ~RurTemplateBaseArray();

  int GetItemsInContainer() {
    return poc;
  }

  bool HasObjects() {
    return has_objects;
  }

  void Zero() {
    poc = 0;
  }

  void* operator[](int i) {
    return data[i];
  }
  void Add(void* o);
  void AddAt(void* o, int n);
  void* Detach(int n);
  bool Destroy(int n);
  bool Destroy(void*);
  void Flush();
  void Resize(int n);

  void OwnsElements(bool o) {
    has_objects = o;
  }
  int FindPosition(void* o);
  bool HasMember(void* o);
};

// pole pointrov na objekty
template<class T>
class RurTemplateArray :
      public RurTemplateBaseArray {
  public:
  RurTemplateArray(int beg, int delt) :
        RurTemplateBaseArray(beg, delt) {
  }

  ~RurTemplateArray() {
    Flush();
  }
  void Flush();

  void DelObj(T* o) {
    delete o;
  }

  T* operator[](int i) {
    return (T*)data[i];
  }

  T* Detach(int n) {
    return (T*)RurTemplateBaseArray::Detach(n);
  }
  bool Destroy(int n);
};

template<class T>
void RurTemplateArray<T>::Flush() {
  if (HasObjects()) {
    int poc = GetItemsInContainer();
    for (int i = 0; i < poc; i++) {
      DelObj((T*)data[i]);
    }
  }
  Zero();
}

template<class T>
bool RurTemplateArray<T>::Destroy(int n) {
  T* o = Detach(n);
  if (!o) {
    return false;
  }
  if (o && HasObjects()) {
    delete o;
  }
  return true;
}

// obycajne pole objektov
template<class T>
class RArray {
  protected:
  T* data;
  int delta; // prirastok
  int poc;   // pocet poloziek
  int max;   // maximum alokovaneho priestoru

  public:
  RArray(int beg, int delt) {
    data  = new T[beg];
    delta = delt;
    poc   = 0;
    max   = beg;
  }

  virtual ~RArray() {
    delete[]data;
  }

  void Flush() {
    poc = 0;
  }
  void Add(T& o);
  void AddAt(T& o, int n);
  void Resize(int n);

  int GetItemsInContainer() {
    return poc;
  }

  T& operator[](int i) {
    return data[i];
  }
  int Detach(int n);

  bool Destroy(int n) {
    return Detach(n);
  }
};

template<class T>
void RArray<T>::Add(T& o) {
  if (poc >= max) {
    Resize(poc + 1);
  }
  data[poc++] = o;
}

template<class T>
void RArray<T>::AddAt(T& o, int n) {
  if (poc >= max) {
    Resize(poc + 1);
  }
  for (int i = poc; i > n; i--) {
    data[i] = data[i - 1];
  }
  data[n] = o;
  poc++;
}

template<class T>
int RArray<T>::Detach(int n) {
  if (n >= poc) {
    return 0;
  }
  poc--;
  for (int i = n; i < poc; i++) {
    data[i] = data[i + 1];
  }
  return 1;
}

template<class T>
void RArray<T>::Resize(int n) {
  if (n < max) { // dost priestoru
    return;
  }
  int newmax = max;
  while (newmax < n) {
    newmax += delta;
  }
  T* tmp = new T[newmax];
  for (int i = 0; i < poc; i++) {
    tmp[i] = data[i];
  }
  delete[]data;
  data = tmp;
  max  = newmax;
}

// obycajne triedene pole objektov
template<class T>
class RSArray :
      public RArray<T> {
  public:
  RSArray(int beg, int delt) :
        RArray<T>(beg, delt) {
  }
  void Add(T& o);
};

template<class T>
void RSArray<T>::Add(T& o) {
  if (this->poc >= this->max) {
    Resize(this->poc + 1);
  }
  int po = this->poc;
  while (po > 0 && o < this->data[po - 1]) {
    this->data[po] = this->data[po - 1];
    po--;
  }
  this->data[po] = o;
  this->poc++;
}

// obycajne pole objektov s hladanim
// objekt musi mat definovany == operator
template<class T>
class RHArray :
      public RArray<T> {
  public:
  RHArray(int beg, int delt) :
        RArray<T>(beg, delt) {
  }
  int Find(T& o);
};

template<class T>
int RHArray<T>::Find(T& o) {
  for (int i = 0; i < this->poc; i++) {
    if (this->data[i] == o) {
      return i;
    }
  }
  return -1;
}

#endif

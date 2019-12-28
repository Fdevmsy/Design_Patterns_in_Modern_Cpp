# Design Patterns
Shiyu Mou
## Single Responsibilities Principle

A class should have a primary responsibility and should not take up other responsibilities. 

### Persistence Code 
https://en.wikipedia.org/wiki/Persistence_(computer_science)#cite_note-1

In  [computer science](https://en.wikipedia.org/wiki/Computer_science "Computer science"),  **persistence**  refers to the characteristic of  [state](https://en.wikipedia.org/wiki/State_(computer_science) "State (computer science)")  that outlives the  [process](https://en.wikipedia.org/wiki/Process_(computing) "Process (computing)")  that created it. This is achieved in practice by storing the state as data in  [computer data storage](https://en.wikipedia.org/wiki/Computer_data_storage "Computer data storage"). Programs have to transfer data to and from storage devices and have to provide mappings from the native  [programming-language](https://en.wikipedia.org/wiki/Programming_language "Programming language")  [data structures](https://en.wikipedia.org/wiki/Data_structure "Data structure")  to the storage device data structures. 
~~~c++
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
using namespace std;

struct Journal
{
  string title;
  vector<string> entries;

  explicit Journal(const string& title)
    : title{title}
  {
  }

  void add(const string& entry);

  // persistence is a separate concern
  void save(const string& filename);
};

void Journal::add(const string& entry)
{
  static int count = 1;
  entries.push_back(boost::lexical_cast<string>(count++)
    + ": " + entry);
}

void Journal::save(const string& filename)
{
  ofstream ofs(filename);
  for (auto& s : entries)
    ofs << s << endl;
}

struct PersistenceManager
{
  static void save(const Journal& j, const string& filename)
  {
    ofstream ofs(filename);
    for (auto& s : j.entries)
      ofs << s << endl;
  }
};

void main()
{
  Journal journal{"Dear Diary"};
  journal.add("I ate a bug");
  journal.add("I cried today");

  //journal.save("diary.txt");

  PersistenceManager pm;
  pm.save(journal, "diary.txt");
}
~~~
Here it's a bad idea to put persistence code in the class. Imagine you have hundreds of class and each of them has a persistence code. When your database changed, you have to go to all the hundreds class to modify them. It's a better diea to create another class for handling all the persistence code. This is called **Separation of concerns**.

### Static Class Object

~~~c++
class GfG 
{  
public: 
  GfG() 
  {cout << "Inside Constructor\n"; } 
  ~GfG() 
  {cout << "Inside Destructor\n"; } 
}; 

int main() 
{ 
  int x = 0; 
  if (x==0) 
  { 
    //case1: GfG obj;
    //case2: static GfG obj;
  } 
  cout << "End of main\n"; 
} 
~~~
Case1: 
1. Inside Constructor
2. Inside Destructor	
3. End of main

Case2:
1. Inside Constructor
2. End of Main
3. Inside Destructor

The **object stays out of scope** when declaring an object as static. 

### Static functions in a class 
Just like the static data members or static variables inside the class, static member functions also does not depend on object of class. We are allowed to invoke a static member function using the object and the ‘.’ operator but it is recommended to invoke the static members using the class name and the scope resolution operator.  
**Static member functions are allowed to access only the static data members or other static member functions**, they can not access the non-static data members or member functions of the class.

~~~c++
class GfG 
{ 
public: 
	// static member function 
	static void printMsg() 
	{ 
		cout<<"Welcome to GfG!"; 
	} 
}; 
// main function 
int main() 
{ 
	// invoking a static member function 
	GfG::printMsg(); 
}
~~~

**Static member/member function in class can be called without creating an object.**

**But, static member is only declared in class, not defined, it has to be define outside using resolution scope.** Check here for details: https://www.geeksforgeeks.org/static-data-members-c/
https://docs.microsoft.com/en-us/cpp/cpp/static-members-cpp?view=vs-2019

### Explicit Keywords
When the constructor has one single argument, 
https://stackoverflow.com/questions/121162/what-does-the-explicit-keyword-mean

~~~c++
```
class Foo
{
public:
  // single parameter constructor, can be used as an implicit conversion
  Foo (int foo) : m_foo (foo) 
  {
  }

  int GetFoo () { return m_foo; }

private:
  int m_foo;
};
```
```
void DoBar (Foo foo)
{
  int i = foo.GetFoo ();
}
```
```
int main ()
{
  DoBar (42); 
  // expect type Foo, but now it still works! Because the
  //constructor of foo takes int as a single argument. 
}
```
~~~


 ## Open-Closed Principle
So the open closed principle basically states that your systems should be open to extensions so you should be able to extend systems by inheritance for example but closed for modification.
~~~c++
// open closed principle

// open for extension, closed for modification

#include <string>
#include <vector>
#include <iostream>
using namespace std;

enum class Color { red, green, blue };
enum class Size { small, medium, large };

struct Product
{
  string name;
  Color color;
  Size size;
};

struct ProductFilter
{
  typedef vector<Product*> Items;

  Items by_color(Items items, const Color color)
  {
    Items result;
    for (auto& i : items)
      if (i->color == color)
        result.push_back(i);
    return result;
  }

  Items by_size(Items items, const Size size)
  {
    Items result;
    for (auto& i : items)
      if (i->size == size)
        result.push_back(i);
    return result;
  }

  Items by_size_and_color(Items items, const Size size, const Color color)
  {
    Items result;
    for (auto& i : items)
      if (i->size == size && i->color == color)
        result.push_back(i);
    return result;
  }
};

template <typename T> struct AndSpecification;

template <typename T> struct Specification
{
  virtual ~Specification() = default;
  virtual bool is_satisfied(T* item) const = 0;

  // new: breaks OCP if you add it post-hoc
  /*AndSpecification<T> operator&&(Specification<T>&& other)
  {
    return AndSpecification<T>(*this, other);
  }*/
};

// new: 
template <typename T> AndSpecification<T> operator&&
  (const Specification<T>& first, const Specification<T>& second)
{
  return { first, second };
}

template <typename T> struct Filter
{
  virtual vector<T*> filter(vector<T*> items,
                            Specification<T>& spec) = 0;
};

struct BetterFilter : Filter<Product>
{
  vector<Product*> filter(vector<Product*> items,
                           Specification<Product> &spec) override
  {
    vector<Product*> result;
    for (auto& p : items)
      if (spec.is_satisfied(p))
        result.push_back(p);
    return result;
  }
};

struct ColorSpecification : Specification<Product>
{
  Color color;

  ColorSpecification(Color color) : color(color) {}

  bool is_satisfied(Product *item) const override {
    return item->color == color;
  }
};

struct SizeSpecification : Specification<Product>
{
  Size size;

  explicit SizeSpecification(const Size size)
    : size{ size }
  {
  }


  bool is_satisfied(Product* item) const override {
    return item->size == size;
  }
};

template <typename T> struct AndSpecification : Specification<T>
{
  const Specification<T>& first;
  const Specification<T>& second;

  AndSpecification(const Specification<T>& first, const Specification<T>& second) 
    : first(first), second(second) {}

  bool is_satisfied(T *item) const override {
    return first.is_satisfied(item) && second.is_satisfied(item);
  }
};

// new:

int main()
{
  Product apple{"Apple", Color::green, Size::small};
  Product tree{"Tree", Color::green, Size::large};
  Product house{"House", Color::blue, Size::large};

  const vector<Product*> all { &apple, &tree, &house };

  BetterFilter bf;
  ColorSpecification green(Color::green);
  auto green_things = bf.filter(all, green);
  for (auto& x : green_things)
    cout << x->name << " is green\n";


  SizeSpecification large(Size::large);
  AndSpecification<Product> green_and_large(green, large);

  //auto big_green_things = bf.filter(all, green_and_large);

  // use the operator instead (same for || etc.)
  auto spec = green && large;
  for (auto& x : bf.filter(all, spec))
    cout << x->name << " is green and large\n";

  // warning: the following will compile but will NOT work
  // auto spec2 = SizeSpecification{Size::large} &&
  //              ColorSpecification{Color::blue};

  getchar();
  return 0;
}
~~~

### enum class
https://www.geeksforgeeks.org/enum-classes-in-c-and-their-advantage-over-enum-datatype/

### const member function 
In practice, try to make as many functions const as possible. For example: getValue(), print(), etc... 
Const object can only call const functions. But const functions can be called by any object.  

### Struct and Class, where are the memory allocated

In C++, the **only** difference between struct and class is **struct** makes members **public** by default while **class** makes them **private**. 

It depends. If you allocate them inside a function as local variables, for example

```c++
void somefunction(){	MyClass foo;	…}
```

they’ll be allocated on the stack. If you allocate them as global variables, such as

```c++
MyClass foo;void somefunction(){}
```

they’ll be in the static area - neither the heap, nor the stack, but an area of fixed size allocated for this purpose when the program is loaded. If you allocate them inside a function using new, like this:

```c++
void somefunction(){	MyClass *foo = new MyClass();	…	delete foo;}
```

they’ll be on the heap.

The above policy applies to everything (int, string, etc) in C++.



## Liskov Substitution Principle
**Objects** in a program should be **replaceable** with **instances** of their **subtypes** w/o altering the correctness of the program. 

If a function takes a base class object as argument, then the function should also works with all the other subclass objects. 

~~~C++
#include <iostream>

class Rectangle
{
protected:
  int width, height;
public:
  Rectangle(const int width, const int height)
    : width{width}, height{height} { }

  int get_width() const { return width; }
  virtual void set_width(const int width) { this->width = width; }
  int get_height() const { return height; }
  virtual void set_height(const int height) { this->height = height; }

  int area() const { return width * height; }
};

class Square : public Rectangle
{
public:
  Square(int size): Rectangle(size,size) {}
  void set_width(const int width) override {
    this->width = height = width;
  }
  void set_height(const int height) override {
    this->height = width = height;
  }
};

struct RectangleFactory
{
  static Rectangle create_rectangle(int w, int h);
  static Rectangle create_square(int size);
};

void process(Rectangle& r)
{
  int w = r.get_width();
  r.set_height(10);

  std::cout << "expected area = " << (w * 10) 
    << ", got " << r.area() << std::endl;
}

int main()
{
  Rectangle r{ 5,5 };
  process(r);

  Square s{ 5 };
  process(s);

  getchar();
  return 0;
}
~~~

1. Expected 50, got 50
2. **Expected 50, got 100**



## Interface Segregation Principle 

Clients should not be forced to depend upon interfaces that they do not use. Avoid stuffing too much into a single interface.

~~~c++
#include <vector>
struct Document;

/*** bad interface, all subclass will has these 3 functions but usually only 
      one is needed. You have to leave the rest as empty or throw exceptions ***/

//struct IMachine
//{
//  virtual void print(Document& doc) = 0;
//  virtual void fax(Document& doc) = 0;
//  virtual void scan(Document& doc) = 0;
//};
//
//struct MFP : IMachine
//{
//  void print(Document& doc) override;
//  void fax(Document& doc) override;
//  void scan(Document& doc) override;
//};

// 1. Recompile
// 2. Client does not need this
// 3. Forcing implementors to implement too much

/*** better interface, seperate them ***/

struct IPrinter
{
  virtual void print(Document& doc) = 0;
};

struct IScanner
{
  virtual void scan(Document& doc) = 0;
};

struct Printer : IPrinter
{
  void print(Document& doc) override;
};

struct Scanner : IScanner
{
  void scan(Document& doc) override;
};

struct IMachine: IPrinter, IScanner
{
};

struct Machine : IMachine
{
  IPrinter& printer;
  IScanner& scanner;

  Machine(IPrinter& printer, IScanner& scanner)
    : printer{printer},
      scanner{scanner}
  {
  }

  void print(Document& doc) override {
    printer.print(doc);
  }
  void scan(Document& doc) override;
};

// IPrinter --> Printer
// everything --> Machine
~~~

## Dependency Inversion Principle

1. The high level modules should not depend on low level modules. Both should depend on abstractions 
2. Abstractions should not depend on details. Details should depend on abstractions. 

The abstraction is also base class, interface. The idea is to make abstractions as generic as possible. 

**Low level** structure: class that stores data, with some API functions for add data and get data.  

High Level structure: perform some research on the data. 

~~~c++
/*** 
Acknowledagement: This is a lecture note for course https://www.udemy.com/course/patterns-cplusplus/ by 
@Dmitri Nesteruk
Title: Dependency Inversion Principle
// A. High-level modules should not depend on low-level modules.
//    Both should depend on abstractions.
// B. Abstractions should not depend on details. 
//    Details should depend on abstractions.

Shiyu Mou
***/
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
using namespace std;

enum class Relationship
{
  parent,
  child,
  sibling
};

struct Person
{
  string name;
};

/*** A bad idea that breaks dependency inversion principle***/

// low level module
struct Relationships // low level
{
  vector<tuple<Person, Relationship, Person>> relations;
  void add_parent_and_child(const Person& parent, const Person& child){
    relations.push_back({parent, Relationship::parent, child});
    realtions.push_back({child, Relationship::child, parent});
  }
}; 

// high level research module
struct Research
{
  Research(Relationships& relationships) // depend on low level
  {
    // depend on the low level details 
    // bad if the low level code decides to change 
    // the container of relations (map) or make it private
    // then the high level code breaks
    auto& relations = relationships.relations;
    for (auto&& [first, rel, second] : relations)
    {
      if (first.name == "John" && rel == Relationship::parent)
      {
        cout << "John has a child called "<< second.name;
      }
    }
  }
};

/*** A better idea that breaks dependency inversion principle***/

// now introduce an abstraction
struct RelationshipBrowser
{
  virtual vector<Person> find_all_children_of(const string& name) = 0;
};

// low level 
struct Relationships : RelationshipBrowser // low level
{
  vector<tuple<Person, Relationship, Person>> relations;
  void add_parent_and_child(const Person& parent, const Person& child){
    relations.push_back({parent, Relationship::parent, child});
    relations.push_back({child, Relationship::child, parent});
  }
  // move f
  vector<Person> find_all_children_of(const string& name) override
  {
    vector<Person> result;
    for (auto&& [first, rel, second]: relations)
    {
      if (first.name == name && rel == Relationships::parent)
      {
        result.push_back(second);
      }
    }
    return result;
  }
};

// high level
struct Research 
{
  // won't depend on details, now depends on abstraction, thanks to polymorphsim, 
  // you can pass subclass object here
  Research(RelationshipBrowser& browser)
  {
    for (auto& child : browser.find_all_children_of("John"))
    {
      cout << "John has a child called " << child.name << endl;
    }
  }
};

int main(){
  Person parent{"John"};
  Person child1{"Chris"};
  Person child2{"Matt"};

  Relationships relationships;
  relationships.add_parent_and_child(parent, child1);
  relationships.add_parent_and_child(parent, child2);

  Research _(relationships);
}
~~~


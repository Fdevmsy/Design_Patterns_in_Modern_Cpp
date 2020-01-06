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

**Class objects** could also be initialized **using assignment from a single value** if they **had a conversion constructor** (prior to C++11, a constructor wit**h a single parameter** was called a ***conversion constructor***):

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



## Gamma Categorization 

### Creational Patterns

- Deal with the creation (construction) of objects
- Explicit (constructor) vs implicit (DI, reflection, etc)
- Wholesale (single statement) vs piecewise (step-by-step)

### Structural Patterns

- Concerned with the structure (e.g. class members).
- Many patterns are wrappers that mimic the underlying class' interface
- Stress the importance of good API design

### Behavior Patterns 

They are all different, no central theme

## Builder 

- Simple objects are simple and can be created in a single constructor call.

- Other objects require a lot of ceremony to create 
- Having an object with **10 constructor arguments** is **not productive** 
- Instead, opt for piecewise construction
- Builder provides and API for constructing an object step-by-step

When **piecewise object construction** is **complicated**, provide an **API for doing it succinctly** .

Think the HtmlElement as a TreeNode, it has data and a vector of children. And let's say build a TreeNode is complicated. Instead of implementing add_child function inside TreeNode to fill all the children, we create a builder, keep a treeNode as a member, and provide API to such as add_child function there. 

~~~c++
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
using namespace std;

struct HtmlElement
{
  string name;
  string text;
  vecotor<HtmlElement> elements;
  HtmlElement() {}
  HtmlElement(const string& name, const string& text): name(name), text(text){}

  string str(int indent = 0) const
  {
		// some code
    // in C++, str is usually for printing
  }
};

struct HtmlBuilder
{
  HtmlElement root;
  HtmlBuilder(string rootname)
  {
    root.name = root_name;
  }

	void add_child(string child_name, string child_text)
  {
    HtmlElement e{child_name, child_text};
    root.elements.emplace_back(e);
  }
  string str() {return root.str();}
};

int main()
{
  // easier way
  HtmlBuilder builder{ "ul" };
  builder.add_child("li", "hello"); 
  builder.add_child("li", "world");
  cout << builder.str() << endl;
  return 0;
}
~~~



## Fluent Builder

Fluent interface is something like `object.add().add()` so you don't need to do `object.add(); object.add();`This is achieved by returning either a reference or a pointer. 

The static member in HtmlElement gives users a hint to use the builder to build object. 

~~~c++
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
using namespace std;

// the struct here is just used for modeling this data.
struct HtmlElement
{
  string name;
  string text;
  vecotor<HtmlElement> elements;
  HtmlElement() {}
  HtmlElement(const string& name, const string& text): name(name), text(text){}

  string str(int indent = 0) const
  {
		// some code
  }
	// give costumer a hint to use the builder 
  static HtmlBuilder build(string root_name)
  {
    return {root_name}; // here would be an implicit conversion for HtmlBuilder
  }
};

// use a builder to actually build it
struct HtmlBuilder
{
  HtmlElement root;
  HtmlBuilder(string rootname)
  {
    root.name = root_name;
  }

  // fluent builder #1
  // reference
  HtmlBuilder& add_child(string child_name, string child_text)
  {
    HtmlElement e{child_name, child_text};
    root.elements.emplace_back(e);
    return *this;
  }
  
  string str() {return root.str();}
  // the operator here convert builder to element.
  operator HtmlElement() const {return root;}
};

int main()
{
  // this way you get a builder 
  auto builder = HtmlElement::build('ul').add.child("", "").add.child("", "");
  // if you want to get a HtmlElement, add an operator
  HtmlElement elem = HtmlElement::build('ul').add.child("", "").add.child("", "");
  return 0;
}
~~~

Now use pointer

~~~c++
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
using namespace std;

struct HtmlElement
{
  string name;
  string text;
  vecotor<HtmlElement> elements;
  HtmlElement() {}
  HtmlElement(const string& name, const string& text): name(name), text(text){}

  string str(int indent = 0) const
  {
		// some code
  }
	// give costumer a hint to use the builder 
  static unique_ptr<HtmlElement> build(string root_name)
  {
    return make_unique<HtmlElement>(root_name);
  }
};

struct HtmlBuilder
{
  HtmlElement root;
  HtmlBuilder(string rootname)
  {
    root.name = root_name;
  }

 	// fluent builder #2
  // pointer 
  HtmlBuilder* add_child_2(string child_name, string child_text)
  {
    HtmlElement e{ child_name, child_text };
    root.elements.emplace_back(e);
    return this;
  }
  string str() {return root.str();}
  operator HtmlElement() const {return root;}
};

int main()
{
	// fluent builder #2
  auto builder2 = HtmlElement::build("ul")
    ->add_child_2("li", "hello")->add_child_2("li", "world");
  cout << builder2 << endl;
  return 0;
}
~~~

To force users to use our builder API by `HtmlBuilder::build()`, make sure they can only construct the HtmlElement through builder. 

**Hide the constructor of HtmlElement.** So the user cannot build HtmlElement directly. To make builder work, we need to make builder a friend of HtmlElement. 

~~~c++
class HtmlElement
{
  friend class HtmlBuilder;
  string name;
  string text;
  vecotor<HtmlElement> elements;
  HtmlElement() {}
  HtmlElement(const string& name, const string& text): name(name), text(text){}
	
public:
  string str(int indent = 0) const
  {
		// some code
  }
	// give costumer a hint to use the builder 
  static HtmlBuilder create(string root_name)
  {
    return {root_name}; // here would be an implicit conversion for HtmlBuilder
  }
};

class HtmlBuilder
{
  HtmlElement root;
public:
  HtmlBuilder(string rootname)
  {
    root.name = root_name;
  }

  HtmlBuilder& add_child(string child_name, string child_text)
  {
    HtmlElement e{child_name, child_text};
    root.elements.emplace_back(e);
    return *this;
  }
  HtmlElement build() {return root;}
  string str() {return root.str();}
  operator HtmlElement() const {return root;}
};

int main()
{
  // if you want to get a HtmlElement, add an operator
  HtmlElement elem = HtmlElement::build('ul').add.child("", "").add.child("", "").build();
}
~~~





### Min Heap

In C++ STL, there is [priority_queue](https://www.geeksforgeeks.org/priority-queue-container-adaptors-the-c-standard-template-library-stl/) that can directly be used to implement Max Heap. 

~~~c++
// C++ program to show that priority_queue is by 
// default a Max Heap 
#include <bits/stdc++.h> 
using namespace std; 
  
// Driver code 
int main () 
{ 
    // Creates a max heap 
    priority_queue <int> pq; 
    pq.push(5); 
    pq.push(1); 
    pq.push(10); 
    pq.push(30); 
    pq.push(20); 
  
    // One by one extract items from max heap 
    while (pq.empty() == false) 
    { 
        cout << pq.top() << " "; 
        pq.pop(); 
    } 
  
    return 0; 
} 
// Output: 30 20 10 5 1
~~~

**How to implement Min Heap?**
priority_queue supports a constructor that requires two extra arguments to make it min heap.

```c++
 //   priority_queue <Type, vector<Type>, ComparisonType > min_heap;
priority_queue <int, vector<int>, greater<int> > pq; 
```

**How to make a min heap of user defined class?**
Let us consider below example where we build a min heap of 2 D points ordered by X axis.

~~~c++
// C++ program to use priority_queue to implement Min Heap 
// for user defined class 
#include <bits/stdc++.h> 
using namespace std; 
  
// User defined class, Point 
class Point 
{ 
   int x; 
   int y; 
public: 
   Point(int _x, int _y) 
   { 
      x = _x; 
      y = _y; 
   } 
   int getX() const { return x; } 
   int getY() const { return y; } 
}; 
  
// To compare two points 
class myComparator 
{ 
public: 
    int operator() (const Point& p1, const Point& p2) 
    { 
        return p1.getX() > p2.getX(); 
    } 
}; 
  
// Driver code 
int main () 
{ 
    // Creates a Min heap of points (order by x coordinate) 
    priority_queue <Point, vector<Point>, myComparator > pq; 
  
    // Insert points into the min heap 
    pq.push(Point(10, 2)); 
    pq.push(Point(2, 1)); 
    pq.push(Point(1, 5)); 
  
    // One by one extract items from min heap 
    while (pq.empty() == false) 
    { 
        Point p = pq.top(); 
        cout << "(" << p.getX() << ", " << p.getY() << ")"; 
        cout << endl; 
        pq.pop(); 
    } 
  
    return 0; 
} 
~~~

### Member function defined inside class or outside class

If defined **inside** class, the functions are **automatically inline** (unless complicated). 

Function definition is **better outside** the class. That way your code can **remain safe if required**. The **header** file should **just give declarations**.

Suppose someone wants to use your code, you can just give him the .**h file and the .obj file** (obtained after compilation) of your class. **He does not need the .cpp file to use your code.**

That way your implementation is not visible to anyone else.

### Structured binding in C++

https://www.geeksforgeeks.org/structured-binding-c/

It doesn't work for vector. Because this only works for structures that are known at compile time. In compile time, the size of vector is unknown. 

### Difference between & and && 

&& is a new reference **operator defined in the C++11** standard. int&& a means "a" is an **r-value reference**. && is normally only used to declare a parameter of a function. And it only takes an **r-value expression**.

Simply put, an **r-value is a value that doesn't have a memory address**. E.g. the number 6, and character 'v' are both r-values. **int a, a is an l-value**, however (a+2) is an r-value. For example:

```c++
void foo(int&& a)
{
   //Some magical code...
}
int main()
{
   int b;
   foo(b);       //Error. An rValue reference cannot be pointed to a lValue.
   foo(5);       //Compiles with no error.
   foo(b+3);     //Compiles with no error.
   int&& c = b;  //Error. An rValue reference cannot be pointed to a lValue.
   int&& d = 5;  //Compiles with no error.
}
```

### POD Type 

A Plain Old Data Structure in C++ is an **aggregate class** that contains only PODS as members, has no user-defined destructor, no user-defined copy assignment operator, and no non static members of pointer-to-member type.

~~~c++
struct bar { int a_; double b_;};
bar b{ 42, 1.2 };
~~~

### Uniform initialization 

In C++03, initialization of variables has **been different for different kinds of variables**, and sometimes it was not even possible. With C++11 we got so-called *uniform initialization*, which attempts to make the whole topic a bit easier for developers.

Uniform initialization is pretty simple: you can **initialize practically everything with arguments in curly braces. The compiler then will do just the right thing.**

~~~c++
# in the past
std::string s1("test");   // direct initialization 
std::string s2 = "test";  // copy initialization
~~~

To uniformly initialize objects regardless of their type, use the brace-initialization form `{}` that can be used for both direct initialization and copy initialization. When used with brace initialization, these are called **direct list** and **copy list initialization.**

They are **equal in the sense that they do mostly the same thing**. But they're not technically equal; **copy-list-initialization cannot call `explicit` constructors**. So if the selected constructor were `explicit`, the code would **fail to compile** in the copy-list-initialization cases.

~~~c++
T object {other};   // direct list initialization 
T object = {other}; // copy list initialization
~~~

~~~c++
class foo
{
  int a_;
  double b_;
  public:
  foo():a_(0), b_(0) {}
  foo(int a, double b = 0.0):a_(a), b_(b) {}
};

foo f1{}; 
foo f2{ 42, 1.2 }; 
foo f3{ 42 };


struct bar 
{
  int a_; 
  double b_;
};
bar b{ 42, 1.2 };
~~~

### Initializer list

I**nitialization of standard containers**, such as the **vector** and the **map** also shown above, is possible because all standard containers have an additional constructor in C++11 that takes an argument of type `std::initializer_list`. This is basically a lightweight proxy over an array of elements of type `T const`. These constructors then initialize the internal data from the values in the initializer list.

~~~c++
// so you can do
vector<int> a{1, 2, 3, 4};
map<int, int> b{{1, 2}, {2, 4}};
~~~

In C++11

The following sample shows several examples of direct-list-initialization and copy-list-initialization. In C++11, the **deduced type** of all these **expressions** is `std::initializer_list`.

~~~c++
auto a = {42};   // std::initializer_list<int>
auto b {42};     // std::initializer_list<int>
auto c = {4, 2}; // std::initializer_list<int>
auto d {4, 2};   // std::initializer_list<int>
~~~

C++17 has changed the rules for list initialization, differentiating between the direct- and copy-list-initialization. The new rules for type deduction are as follows:

- for copy list initialization auto deduction will deduce a `std::initializer_list` if all elements in the list have the same type, or be ill-formed.
- for direct list initialization auto deduction will deduce a `T` if the list has a single element, or be ill-formed if there is more than one element.

Base on the new rules, the previous examples would change as follows: `a` and `c` are deduced as `std::initializer_list`; `b` is deduced as an `int`; `d`, which u**ses direct initialization and has more than one value in the brace-init-list, triggers a compiler error**.

~~~c++
auto a = {42};   // std::initializer_list<int>
auto b {42};     // int
auto c = {4, 2}; // std::initializer_list<int>
auto d {4, 2};   // error, too many
~~~

### Subclass and Base class Constructor 

Here’s what actually happens when **base** is instantiated:

1. Memory for base is set aside
2. The appropriate Base constructor is called
3. The initialization list initializes variables
4. The body of the constructor executes
5. Control is returned to the caller

Here’s what actually happens when derived is instantiated:

1. Memory for derived is set aside (enough for both the Base and Derived portions)
2. The appropriate Derived constructor is called
3. **The Base object is constructed first using the appropriate Base constructor**. If no base constructor is specified, the default constructor will be used.
4. The initialization list initializes variables
5. The body of the constructor executes
6. Control is returned to the caller

### How to init Base Class members when instantiate Derived Class

~~~c++
class Base
{
public:
    int m_id;
    Base(int id=0)
        : m_id{ id }
    {
    }
    int getId() const { return m_id; }
};
~~~



~~~c++
class Derived: public Base
{
public:
  double m_cost;
  Derived(double cost=0.0, int id=0)
    // does not work
    : m_cost{ cost }, m_id{ id }
  {
  }
  double getCost() const { return m_cost; }
};

~~~

~~~c++
class Derived: public Base
{
public:
  double m_cost;

  Derived(double cost=0.0, int id=0)
    : m_cost{ cost }
  {
    // work but bad, what is m_id is const or reference? 
    m_id = id;
  }
  double getCost() const { return m_cost; }
};
~~~

When derived class is initiated, first the derived class constructor is called, but not initialized, then base class constructor is called, and finish the initialization, then derived constructor finish the initialization. If not specific, the default constructor of derived class will be called/

Fortunately, C++ gives us the ability to explicitly **choose which Base class constructor** will be called! To do this, simply add a call to the base class Constructor in the initialization list of the derived class:

~~~c++
class Derived: public Base
{
public:
  double m_cost;
	// we can even make m_id private in base class
  Derived(double cost=0.0, int id=0)
    : Base{ id }, // Call Base(int) constructor with value id!
  m_cost{ cost }
  {
  }
  double getCost() const { return m_cost; }
};
~~~

The base class constructor Base(int) will be used to initialize m_id to 5, and the derived class constructor will be used to initialize m_cost to 1.3!

1. Memory for derived is allocated.
2. The Derived(double, int) constructor is called, where cost = 1.3, and id = 5
3. The compiler looks to see if we’ve asked for a particular Base class constructor. We have! So it calls Base(int) with id = 5.
4. The base class constructor initialization list sets m_id to 5
5. The base class constructor body executes, which does nothing
6. The base class constructor returns
7. The derived class constructor initialization list sets m_cost to 1.3
8. The derived class constructor body executes, which does nothing
9. The derived class constructor returns

## Builder Facet 

Builder with multiple concrete builder classes inheriting from Builder. These classes contain the functionality to create a particular complex product.



## Builder Summay

- **Product –** The product class defines the type of the complex object that is to be generated by the builder pattern. 
- **Builder –** This abstract base class defines all of the steps that must be taken in order to correctly create a product. Each step is generally abstract as the actual functionality of the builder is carried out in the concrete subclasses. The GetProduct method is used to return the final product. The builder class is often replaced with a simple interface. 
- **ConcreteBuilder –** There may be any number of concrete builder classes inheriting from Builder. These classes contain the functionality to create a particular complex product. 
- **Director –** The director class controls the algorithm that generates the final product object. A director object is instantiated and its Construct method is called. The method includes a parameter to capture the specific concrete builder object that is to be used to generate the product. The director then calls methods of the concrete builder in the correct order to generate the product object. On completion of the process, the GetProduct method of the builder object can be used to return the product.

### Customized Comparator

http://fusharblog.com/3-ways-to-define-comparison-functions-in-cpp/

### How vector works internally 

When std::vector’s internal memory completely finishes then it increases the size of its memory. To do that it performs following steps,

1.) It will allocate a bigger chunk of memory on heap i.e. almost double the size of previously allocated.
2.) Then it copies all the elements from old memory location to new one. Yes it copies them, so in case our elements are **user defined objects** then their **copy constructor** will be called. Which makes this step quite heavy in terms of speed.
3.) Then after successful copying it deletes the old memory.

### Fill random Numbers in std::vector using Lambda functions

~~~c++
// Initialize a vector with 10 ints of value 0
std::vector<int> vecOfRandomNums(10);
std::generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), []() {
	return rand() % 100;
});
~~~

### Fill Random Numbers in std::vector using a Functor

~~~c++
struct RandomGenerator {
  int maxValue;
  RandomGenerator(int max) :
  maxValue(max) {}
	
  int operator()() const {
  	return rand() % maxValue;
  }
};
// Generate 10 random numbers by a Functor and fill it in vector
std::generate(vecOfRandomNums.begin(), vecOfRandomNums.end(), RandomGenerator(500));
~~~



## Functor 

A functor (or function object) is a **C++ class that acts like a function**. Functors are called using the same old function call syntax. To create a functor, we create an object that **overloads the *operator()*.**

~~~c++
#include <iostream>
class myFunctorClass
{
	int _x; 
	public:
		myFunctorClass (int x) : _x( x ) {}
		int operator() (int y) { return _x + y; }
		// I'm overloading this operator "()", and (int y) is arugment 
};
 
int main()
{
//	myFunctorClass addFive( 5 );
	std::cout << myFunctorClass(5)(6);
	return 0;
}

// same as 
int main()
{
	myFunctorClass object( 5 );
	std::cout << object(6);
	return 0;
}
~~~

### Copy Constructor and assign operator 

~~~c++
#include<iostream>  
#include<stdio.h>  
  
using namespace std;  
  
class Test  
{  
    public:  
    Test() {}  
    Test(const Test &t)  
    {  
        cout<<"Copy constructor called "<<endl;  
    }  
      
    Test& operator = (const Test &t) 
    { 
        cout<<"Assignment operator called "<<endl; 
        return *this; 
    }  
};  
  
// Driver code 
int main()  
{  
    Test t1, t2;  
    t2 = t1;   // assignment 
    Test t3 = t1;  // copy 
    getchar();  
    return 0;  
}  
~~~

*Assignment operator called
Copy constructor called*



They all have a default one by compiler. 

- All STL contains always stores the copy of **inserted** **objects** not the actual one. So, whenever we insert any element or object in container then its **copy constructor** **is called** to create a copy and then this copy is inserted in the container.
- While insertion in std::vector it might be possible that storage **relocation** takes place internally due to insufficient space. In such cases **assignment operator** will be called on objects inside the container to copy them from one location to another.

### Copy Constructor 

**When is copy constructor called?**
In C++, a Copy Constructor may be called in following cases:
\1. When an object of the class is returned by value.
\2. When an object of the class is passed (to a function) by value as an argument.
\3. When an object is constructed based on another object of the same class.
\4. When the compiler generates a temporary object.

**When is user-defined copy constructor needed?**
If we don’t define our own copy constructor, the **C++ compiler creates a default copy constructor** for each class which does a **member-wise copy between objects**. The compiler created copy constructor works fine in general. We need to define our own copy constructor only i**f an object has pointers or any runtime allocation** of the resource like file handle, a network connection, etc. **Deep copy.**  

### Deque

**How std::deque works internally**

Now a question rises how high deque is able to give good performance for insertion and deletion at both ends?
To know the answer we need to know little internal implementation of dequeue.

A deque is generally implemented as a collection of memory blocks. These memory blocks contains the elements at contiguous locations.

[![deque](https://thispointer.com//wp-content/uploads/2015/07/deque.png)](https://thispointer.com//wp-content/uploads/2015/07/deque.png)

When we create a deque object it internally allocates a memory block to store the elements at contigious location.

- When we insert an element in end it stores that in allocated memory block untill it gets filled and when this memory block gets filled with elements then it allocates a new memory block and links it with the end of previous memory block. Now further inserted elements in the back are stored in this new memory block.
- When we insert an element in front it allocates a new memory block and links it with the front of previous memory block. Now further inserted elements in the front are stored in this new memory block unless it gets filled.
- Consider deque as a linked list of vectors i.e. each node of this linked list is a memory block that store elements at contiguous memory location and each of the memory block node is linked with its previous and next memory block node.

Therefore, insertion at front is fast as compared to vector because it doesn’t need the elements to shift by 1 in current memory block to make space at front. It just checks if any space is left at left of first element in current memory block, if not then allocates a new memory block.

### Smart Pointer 

~~~c++
#include <memory>
 
struct Task
{
	int mId;
	Task(int id ) :mId(id)
	{std::cout<<"Task::Constructor"<<std::endl;}
	~Task()
	{std::cout<<"Task::Destructor"<<std::endl;}
};
 
int main()
{
	// Create a unique_ptr object through raw pointer
	std::unique_ptr<Task> taskPtr(new Task(23));
 
	//Access the element through unique_ptr
	int id = taskPtr->mId;
 	// move ownership
  std::unique_ptr<Task> taskPtr4 = std::move(taskPtr);

  // Reseting the unique_ptr will delete the associated
  // raw pointer and make unique_ptr object empty
  taskPtr.reset();
  
  // Release the ownership of object from raw pointer
  // it will return a raw pointer
	Task * ptr = taskPtr5.release();
  delete ptr;
	return 0;
}
~~~

Shared Pointer 

~~~c++
// create shared pointer  
shared_ptr<int> ptr1(new int(1));
// count # of owners 
ptr1.use_count();

shared_ptr<int> ptr2 = make_shared<int> (2);
// reset will decrease the count by 1
ptr2.reset();
~~~

### Lambda function and for_each

In the following method, we have to write a separate function to do this

~~~c++
#include <iostream>
#include "vector"
using namespace std;
void display(int a)
{
	cout<<a<<" ";
}
using namespace std;
int main(int argc, char *argv[]) {
	vector<int> vect{1, 2, 3, 4, 5};
	for_each(vect.begin(), vect.end(), &display);
  // or for_each(vect.begin(), vect.end(), display);
}

output: 1 2 3 4 5;
~~~

Instead, we can use lambda function to embedded the function directly.

~~~c++
[](int x) {
        std::cout<<x<<" ";
    }
~~~

- [] is used to pass the outer scope elements
- (int x) shows argument x is passed

~~~c++
#include <iostream>
 
int main(int argc, char *argv[]) {
	vector<int> vect{1, 2, 3, 4, 5};
	for_each(vect.begin(), vect.end(), [](int x){x++; cout<<x<<" ";});
}
~~~

~~~c++
[=](int &x) {
  // All outer scope elements has been passed by value
}
~~~

~~~c++
[&](int &x) {
  // All outer scope elements has been passed by reference
}
~~~

~~~c++
#include <iostream>
int main() {
    int arr[] = { 1, 2, 3, 4, 5 };
 
    int mul = 5;
 
    std::for_each(arr, arr + sizeof(arr) / sizeof(int), [&](int x) {
        std::cout<<x<<" ";
        // Can modify the mul inside this lambda function because
        // all outer scope elements has write access here.
            mul = 3;
        });
    std::cout << std::endl;
 
    std::for_each(arr, arr + sizeof(arr) / sizeof(int), [=](int &x) {
        x= x*mul;
        // Can not modify the mul inside this lambda function because
        // all outer scope elements has read only access here
        });
    std::cout << std::endl;
 
    std::for_each(arr, arr + sizeof(arr) / sizeof(int), [](int x) {
        // No access to mul inside this lambda function because
        // all outer scope elements are not visible here.
        //std::cout<<mul<<" ";
        });
    std::cout << std::endl;
 
}
~~~



### Thread 

1. Create thread 

~~~c++
void thread_function()
{
	for (int i=0; i<10; i++) cout<<i<<" ";
	cout<<"t1: "<<this_thread::get_id()<<endl;
}

class functor
{
public:
	void operator() ()
	{
		for (int i=10; i<20; i++) cout<<i<<" ";
		cout<<endl;
	}			
};

int main()
{
	// function pointer 
	thread t1{&thread_function};
	t1.join();
	
	// functor
	thread t2{functor()};
  cout<<"t2: "<<t2.get_id()<<endl;
	t2.join();
	
	// lambda function 
	thread t3{[](){
		for (int i=100; i<110; i++) cout<<i<<" ";
			cout<<endl;
		}};
	cout<<"t3: "<<t3.get_id()<<endl;
	t3.join();
	return 0;
}
~~~

#### Join and detach 

**Use join in most cases.** 

`detach()` is mainly useful when you have a **task that has to be done in background**, but you don't **care about its execution**. This is usually a case for some libraries. They may silently create a background worker thread and detach it so you won't even notice it.

You cannot join or detach a thread that is already joined or detached. It will cause the program to terminate. So check if it's joinable first.

~~~c++
std::thread threadObj( (WorkerThread()) );
if(threadObj.joinable())
{
  std::cout<<"Detaching Thread "<<std::endl;
  threadObj.detach();
}
if(threadObj.joinable())    
{
  std::cout<<"Detaching Thread "<<std::endl;
  threadObj.detach();
}

std::thread threadObj2( (WorkerThread()) );
if(threadObj2.joinable())
{
  std::cout<<"Joining Thread "<<std::endl;
  threadObj2.join();
}
if(threadObj2.joinable())    
{
  std::cout<<"Joining Thread "<<std::endl;
  threadObj2.join();
}
~~~

### Pass argument to thread

The argument is **passed by value by default.** 

~~~c++
void threadCallback(int x, std::string str)
{
    std::cout<<"Passed Number = "<<x<<std::endl;
    std::cout<<"Passed String = "<<str<<std::endl;
}
int main()  
{
    int x = 10;
    std::string str = "Sample String";
    std::thread threadObj(threadCallback, x, str);
    threadObj.join();
    return 0;
}
~~~

~~~c++
void thread_function(int x)
{cout<<x<<endl;}

class functor
{
public:
	void operator() (int x)
	{cout<<x<<endl;}			
};

int main()
{
	// function pointer 
	thread t1{&thread_function, 1};
	t1.join();
	
	// functor
	thread t2{functor(), 10};
	t2.join();
	
	// lambda function 
	thread t3([](int y) {cout<<y<<endl;}, 
											20);
	t3.join();
	return 0;
}
~~~

**To pass by reference:** 

~~~c++ 
void threadCallback(int const & x)
{
    int & y = const_cast<int &>(x);
    y++;
    std::cout<<"Inside Thread x = "<<x<<std::endl;
}

int main()
{
    int x = 9;
    std::thread t1(threadCallback, std::ref(x));
    t1.join();
    return 0;
}
~~~



**Don’t pass addresses of variables from local stack** to thread’s callback function. Because it might be possible that local variable in Thread 1 goes out of scope but Thread 2 is still trying to access it through it’s address.

~~~c++
void newThreadCallback(int * p)
{
    std::cout<<"Inside Thread :  "" : p = "<<p<<std::endl;
    std::chrono::milliseconds dura( 1000 );
    std::this_thread::sleep_for( dura );
    *p = 19;
}
void startNewThread()
{
    int i = 10;
    std::cout<<"Inside Main Thread :  "" : i = "<<i<<std::endl;
    std::thread t(newThreadCallback,&i);
    t.join();
  	// or t.detach();
    std::cout<<"Inside Main Thread :  "" : i = "<<i<<std::endl;
}
int main()
{
    startNewThread();
    std::chrono::milliseconds dura( 2000 );
    std::this_thread::sleep_for( dura );
    return 0;
}

// output 
// if join()
Inside Main Thread :   : i = 10
Inside Thread :   : p = 0x7ffee71267bc
Inside Main Thread :   : i = 19
  
// if detach
Inside Main Thread :   : i = 10
Inside Main Thread :   : i = 10
Inside Thread :   : p = 0x7ffee75627bc

~~~

### **Assigning pointer to member function of a class as thread function:**

Pass the **pointer to member function as callback function** and **pass pointer to Object** as **second argument.**

~~~c++
class foo {
public:
  foo(){}
  foo(const foo& obj){}
  void thread_fun(int x) {}
}

int main(){
  foo obj;
  std::thread t1{&foo::thread_fun, &obj, 10};
  t1.join();
  return 0;
}
~~~

### Mutex

~~~c++
class wallet
{
	int money;
	mutex m;
	wallet() : money(0){}
	void addMoney(int x){
		m.lock();
		money++;
		m.unlock();
	}
	// or use lock_guard
	void addMoney2(int x){
		// automatically locked in the constructor
		std::lock_guard<std::mutex> guard(m);
		money++; 
		// goes out of scope, destuctor automatically unlock
	}
~~~

### Event handling 

Suppose we are building a network based application. This application does following tasks

1. Perform some handshaking with server
2. Load data from XML files
3. Do processing on the data loaded from XML

Task 1 and task 2 -> parallel.

Task 3 depends on task 2.



Thread 1: 

1. Handshake
2. Wait for thread 2 to finish 
3. Do some work on data

Thread 2:

1. Load data from XML
2. Notify thread 1. 

**Option1: global variable with mutex** 

Cons: cost a lot CPU resources for the while loop (lock and unlock)

~~~c++
class App
{
	std::mutex m;
	bool done;

public:
	App(): done(false){}
	void loadData(){
		// function for thread 2
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout<<" loading xml"<<std::endl;
		std::lock_guard<std::mutex> guard(m);
		done = true;
	}	
	void mainTask(){
		std::cout<<" handshaking "<<std::endl;
		std::cout<<" handshaking finished "<<std::endl;
		m.lock();
		while (!done) {
			m.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			m.lock();
		}
		m.unlock();
		std::cout<<" data loaded, start processing"<<std::endl;
	}
};

int main(){
	App app;
	std::thread t1{&App::mainTask, &app};
	std::thread t2{&App::loadData, &app};
	
	t2.join();
	t1.join();
	return 0;
}

/*** 
 handshaking 
 handshaking finished 
 loading xml
 data loaded, start processing
/
~~~

`lock_guard` and `unique_lock` are **pretty much the same thing**; `lock_guard`is a restricted version with a limited interface.

A `lock_guard` always holds a lock from its construction to its destruction. A `unique_lock` can be created without immediately locking, can unlock at any point in its existence, and can transfer ownership of the lock from one instance to another.

So you always use `lock_guard`, unless you need the capabilities of `unique_lock`. **A `condition_variable` needs a `unique_lock`** because it needs to **lock and unlock it while wait**.  

~~~c++
#include "condition_variable"

class App
{
	std::mutex m;
	std::condition_variable cv;
	bool done;
	
public:
	App(): done(false){}
	void loadData(){
		// function for thread 2
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout<<" start loading xml"<<std::endl;
		std::lock_guard<std::mutex> guard(m);
		// or 
		// std::unique_lock<std::mutex> guard(m);
		// here doesn't matter 
		done = true;
		cv.notify_all();
		// cv.notify_one();
	}	
	void mainTask(){
		std::cout<<" handshaking "<<std::endl;
		std::cout<<" handshaking finished "<<std::endl;
		
		std::unique_lock<std::mutex> u_lock(m);
		// condition variable requires unique_lock
		// option 1: cv.wait(u_lock, std::bind(&App::isDone, this));
		// option 2:
		while (!done) cv.wait(u_lock);
		std::cout<<" data loaded, start processing"<<std::endl;
	}
//	bool isDone() {return done;}
};

int main(){
	App app;
	std::thread t1{&App::mainTask, &app};
	std::thread t2{&App::loadData, &app};
	
	t2.join();
	t1.join();
	return 0;
}
~~~

### Future and promise 

Can we do this even more easier? 

~~~c++
#include "future"
class App
{
	std::promise<void> p1;
public:
	void loadData(){
		// function for thread 2
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout<<" start loading xml"<<std::endl;
		p1.set_value();
	}	
	void mainTask(){
		std::cout<<" handshaking "<<std::endl;
		std::cout<<" handshaking finished "<<std::endl;
		
		p1.get_future().wait();
		std::cout<<" data loaded, start processing"<<std::endl;
	}
};

int main(){
	App app;
	std::thread t1{&App::mainTask, &app};
	std::thread t2{&App::loadData, &app};
	
	t2.join();
	t1.join();
	return 0;
}
~~~


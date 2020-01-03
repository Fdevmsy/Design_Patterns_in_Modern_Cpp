/*** 
Acknowledagement: This is a lecture note for course https://www.udemy.com/course/patterns-cplusplus/ by 
@Dmitri Nesteruk
Title: Builder
- Simple objects are simple and can be created in a single constructor call.

- Other objects require a lot of ceremony to create 
- Having an object with 10 constructor arguments is not productive 
- Instead, opt for piecewise construction
- Builder provides and API for constructing an object step-by-step

When piecewise object construction is complicated, provide an API for doing it succinctly .
Think the HtmlElement as a TreeNode, it has data and a vector of children,
Instead of implementing add_child function inside TreeNode to fill all the children,
we create a builder, keep a treeNode as a member, and add the add_child function there

Shiyu Mou
***/

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
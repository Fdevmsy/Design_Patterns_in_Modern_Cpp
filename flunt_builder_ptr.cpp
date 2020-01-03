/*** 
Acknowledagement: This is a lecture note for course https://www.udemy.com/course/patterns-cplusplus/ by 
@Dmitri Nesteruk
Title: Flunt Builder
Fluent interface is something like object.add().add() so you don't need to do 
object.add(); 
object.add();
This is achieved by returning either a reference or a pointer. 

The static member in HtmlElement gives users a hint to use the builder to build object. 

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
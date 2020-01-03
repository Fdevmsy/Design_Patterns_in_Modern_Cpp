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
  // or add a get function
  HtmlElement elem = HtmlElement::build('ul').add.child("", "").add.child("", "");
  return 0;
}
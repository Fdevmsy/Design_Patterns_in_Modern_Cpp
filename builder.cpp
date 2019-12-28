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
    ostringstream oss;
    string i(ident_size*indent, ' ');
    oss << i << "<" << name << ">" << endl;
    if (text.size() > 0)
      oss << string(indent_size*(indent + 1), ' ') << text << endl;
    for (const auto& e: elements)
      oss < e.str(indent + 1);

    oss << i << "</" << name << ">" << endl;
    return oss.str();
  }

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

  // void to start with
  HtmlBuilder& add_child(string child_name, string child_text)
  {
    HtmlElement e{child_name, child_text};
    root.elements.emplace_back(e);
    return this;
  }
 
  // pointer based
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
  /*** bad way ***/ 
  // <p>hello</p>
  auto text = "hello";
  string output;
  output += "<p>";
  output += text;
  output += "</p>";
  printf("<p>%s</p>", text);

  // <ul><li>hello</li><li>world</li></ul>
  string words[] = { "hello", "world" };
  ostringstream oss;
  oss << "<ul>";
  for (auto w : words)
    oss << "  <li>" << w << "</li>";
  oss << "</ul>";
  printf(oss.str().c_str());


  // easier way
  HtmlBuilder builder{ "ul" };
  builder.add_child("li", "hello").add_child("li", "world");
  cout << builder.str() << endl;


  auto builder2 = HtmlElement::build("ul")
    ->add_child_2("li", "hello")->add_child_2("li", "world");
  cout << builder2 << endl;
  return 0
}
/*** 
Acknowledagement: This is a lecture note for course https://www.udemy.com/course/patterns-cplusplus/ by 
@Dmitri Nesteruk
Title: friend and force builder
To force users to use our builder API by HtmlBuilder::build(), 
make sure they can only construct the HtmlElement through builder. 

Hide the constructor of HtmlElement. So the user cannot build HtmlElement directly. 
To make builder work, we need to make builder a friend of HtmlElement. 

Shiyu Mou
***/

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
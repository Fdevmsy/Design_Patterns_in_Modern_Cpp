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
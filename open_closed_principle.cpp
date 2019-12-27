/*** 
Acknowledagement: This is a lecture note for course https://www.udemy.com/course/patterns-cplusplus/ by 
@Dmitri Nesteruk
Title: Open-Close-Principle
Task: The task is to design a product filter that can select some products,
the code should meet Open-Close-Principle so we can easily extend the code
to change the filter criterion or the product type without modify the code
we already wrote. 

Shiyu Mou
***/

#include <string> 
#include <vector>
#include <iostream> 
using namespace std;

enum class Color {red, green, blue};
enum class Size {small, medium, large};

struct Product
{
  string name;
  Color color;
  Size size;
};

/*** bad example, break OCP, not extentable and have to modify this part of 
    code if a change is required ***/
struct ProductFilter 
{
  typedef vector<Product*> Items;
  // here we have to add 3 different functions in this filter code 
  Items by_color(Items items, const Color color){
    Items results;
    for (auto& i: items)
      if (i->color==color)
        results.push_back(i);
    return results;
  }

  Items by_size(Items items, const Size size){
    Items results;
    for (auto& i: items)
      if (i->size==size)
        results.push_back(i);
    return results;
  }
  
  Items by_size_and_color(Items items, const Size size, const Color color){
    Items results;
    for (auto& i: items)
      if (i->size==size && i->color==color)
        results.push_back(i);
    return results;
  }
}; // and too many repeating code 

/*** now make a better filter ***/

// first let's have an abstract class called Specification for comparing things 
template <typename T> 
struct Specification
{
  virtual ~Specification() = default;
  virtual bool is_satisfied(T* item) const = 0;
};

// subclass Specification to have a color comparator
struct ColorSpecification : Specification<Product>
{
  Color color;
  explicit ColorSpecification(const Color color): color(color){}
  bool is_satisfied(Product* item) const override{
    return item->color==color;
  }
};

// subclass Specification to have a size comparator
struct SizeSpecification : Specification<Product>
{
  Size size;
  explicit SizeSpecification(const Size size): size(size){}
  bool is_satisfied(Product* item) const override{
    return item->size==size;
  }
};

// subclass Specification to have a size_and_color comparator
template <typename T> 
struct AndSpecification : Specification<T>
{
  const Specification<T>& first;
  const Specification<T>& second;
  AndSpecification(const Specification<T>& first, const Specification<T>& second): first(first), second(second) {}
  bool is_satisfied(T* item) const override{
    return first.is_satisfied(item) && second.is_satisfied(item);
  } 
};

// abstract class Filter
template <typename T> 
struct Filter 
{
  virtual void filter(vector<T*> items, Specification<T>& spec, vector<T*> &results) = 0;
  virtual ~Filter() = default;
};

// the better filter implementation
struct BetterFilter: Filter<Product>
{
  void filter(vector<Product*> items, Specification<Product>& spec, vector<Product*> &results) override{
    for (auto& p: items){
      if (spec.is_satisfied(p)) results.push_back(p);
    }
  }
};

int main()
{
  Product apple = {"Apple", Color::green, Size::small};
  Product tree{"Tree", Color::green, Size::large};
  Product house = {"House", Color::blue, Size::large};
  const vector<Product*> all{&apple, &tree, &house};

  BetterFilter bf;
  // Specification by color
  ColorSpecification green(Color::green);
  vector<Product*> green_things;
  bf.filter(all, green, green_things);
  for (auto& x : green_things)
    cout<<x->name<<" is green\n";

  // Specification by size
  SizeSpecification large(Size::large);
  // Specification by both color and size
  AndSpecification<Product> green_large(green, large);
  vector<Product*> green_large_things;
  bf.filter(all, green_large, green_large_things);
  for (auto& x : green_large_things)
    cout<<x->name<<" is green and large\n";
  return 0;   
}
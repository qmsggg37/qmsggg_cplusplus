智能指针是`C++`标准库中提供的一系列用于内存自动化管理的工具。智能指针就是利用`RAII`(Resource Acquisition Is Initialization)来进行
内存管理的一个典型例子。

## 1. 为什么需要智能指针
从形式上来看，智能指针是用对象封装了指向某个特定类型的对象指针，其本质上是一个对象，但是通过一些操作符重载使得其使用行为又类似一个原生的指针。
智能指针对象在生命周期结束时（离开作用域），自动的删除对象。智能指针的作用很明显，起到自动化的内存管理，防止开发者忘记了内存释放。
智能指针还是“异常安全的”，也就是说虽然你没忘记写delete对象的操作，但是也可能由于中间过程中抛出的异常而使程序在未执行到delete对象
之前就跳转到异常处理中了，从而导致内存泄露的发生。作为一个栈上的对象，即便是在有异常抛出的场景下，编译器也能保证销毁该对象，进而释放相应的内存。

```
int test_func() {
  try {
	...
	Object* obj = new Object();
	...
	delete obj;
	return 0;
  } catch(...) {
	...
	return 1;
  }
}
```

从语言的设计上来看，C++中的对象是具有“值语义”(Value Semantic)的，而不是Reference Semantic，这个是和Java有重大区别的。比如：Java中的一段代码：
```
Object obj1 = new Object();
Object obj2 = obj1;
```
这里仅仅只有一个对象创建出来，而obj1和obj2仅仅是“对象引用”而已。也就是说obj1和obj2实际关联到的是同一个对象，同一块内存区域。
但是，在C++中的对象却是“值语义”的，如下面这段代码：
```
Object obj1;
Object obj2 = obj1;
```
这里obj1和obj2是两个不同的对象，各自有自己的内存区域。C++对象的“值语义”在实际的使用中也带来的一些问题：

1）一些类型的对象的拷贝是毫无意义的，比如：socket对象、thread对象、抽象的TCP连接对象等，如果使用值传递，
那必然有一个对象的复制动作。而这类对象通常都是禁止复制的（类定义时通常将copy-constructor和operator =声明为private，
或者继承boost::noncopyable），因此没法进行值传递；

2）有些对象“很大”，其构造过程以及对象的复制过程很复杂，而“值语义”带来的对象复制和创建过程带来的开销是非常可观的，
这个会带来效率上的下降。（现在的编译器基本上都有返回值优化RVO/NVRO，可以一定程度上的缓解这类问题；
另外，C++11中引入的右值引用也是为了解决这个效率问题）

作为C开发人员，肯定第一时间就能想到用指针来解决这个问题。但是，指针是内存泄漏的万恶源头，而且使用指针来传递对象信息就更难管理了，
尤其是在大规模的软件开发中，什么时候释放指针？有哪些地方用到了这个指针，释放后别的地方要再使用了怎么办？在多线程环境下就更麻烦了，
没法掌控。智能指针是了解决这些问题而产生的，但是不同类型的智能指针的实现以及使用上也存在细节上的差异。

## 2. C++提供了哪些智能指针可供使用

C++标准库中提供的智能指针有下面几种：

1）`std::auto_ptr`: 从C++98开始出现，auto_ptr由于存在所有权转移以及无法和STL容器配合等原因很少被用到，在C++11中已经被废弃了，
这里也就不再介绍了，"More Effective C++"中有对auto_ptr的详细介绍；

2）`std::unique_ptr`：C++11标准中引入，从名字可以看出，unique_ptr是不支持共享的，也就是不允许多个unique_ptr对象关联到同一个具体的对象指针；

3）`std::shared_ptr`：C++11标准中引入，伴随shared_ptr的还有std::weak_ptr和std::enable_shared_from_this等一些辅助类；
shared_ptr通过“引用计数”的方法实现了多个shared_ptr共享一个对象指针的处理。shared_ptr的引用计数的处理是线程安全的，
但是并不是说shared_ptr就是线程安全的，在讲到shared_ptr的时候再展开吧。

 
除了C++标准库中的这几种外，还有准标准库boost中的boost::scoped_ptr。boost中也有boost::shared_ptr，
C++标准库中的std::shared_ptr就是从boost::shared_ptr发展来的。本文先介绍一下std::unique_ptr，下一次再介绍复杂一点的std::shared_ptr。

 

## 3. std::unique_ptr的使用介绍

`std::unique_ptr`有两种形式，分别对应到指针和数组类型的管理。后者对于数组的管理使用了模板特化语法(更准确的说是部分特化或者是偏特化)，
这里只要先能看懂就行了。

```
template<
	class T,
	class Deleter = std::default_delete > class unique_ptr;
template <
	class T,
	class Deleter
> class unique_ptr;
```
#### 3.1 std::unique_ptr的基本使用方式

`std::unique_ptr`最简单的使用方式如下。在该例子中，`unique_ptr`仅用到了自动内存管理的特性，避免在分支处理中漏写了`delete obj`的操作。

```
int test_func1() {
  std::unique_ptr<Object> obj(new Object());
  ....
  /// 在离开函数作用域时, obj的析构函数会自动释放内存
  return 0;
```

```
int test_func1() {
  std::unique_ptr<Object> obj = new Object(); /// Compiler Error
  return 0;
}
int test_func1() {
  std::unique_ptr<Object> obj;
  obj = new Object(); /// Compiler Error
  return 0;
}
```

第一个错误用法是试图使用copy-initialization进行对象的创建。以前的文章中介绍过，因为new Object()创建的Object对象指针和unique_ptr<Object>的
类型不同，也不是其派生类，因此需要先根据Object对象指针创建一个unique_ptr<Object>临时对象，这里的对unique_ptr的构造函数调用是“隐式”调用。
但是，unique_ptr的构造函数是申明为explicit的，禁止了“隐式”调用，因此这里会有编译错误。

```
explicit unique_ptr(pointer __p) noexcept
  : _M_t(__p, deleter_type()) { 
	static_assert(!is_pointer<deleter_type>::value,
	  "constructed with null function pointer deleter"); 
}
```

第二个错误用法是试图通过使用opeator = 来进行赋值。而事实上unique_ptr并未提供这种类型的赋值操作重载，unique_ptr只支持下面几种方式赋值操作。

```
unique_ptr& operator=(unique_ptr&& __u) noexcept;   /// 第一种方式
template<typename _Up, typename _Ep>
 typename enable_if<__safe_conversion<_Up, _Ep>::value, unique_ptr&>::type
 operator=(unique_ptr<_Up, _Ep>&& __u) noexcept;   /// 第一种方式的模板形式
unique_ptr& operator=(nullptr_t) noexcept;	 /// 第二种方式	
/// unique_ptr禁止了左值的拷贝构造函数和赋值操作
unique_ptr(const unique_ptr&) = delete;
unique_ptr& operator=(const unique_ptr&) = delete;
```

unique_ptr禁止了左值的拷贝构造函数和赋值操作，而auto_ptr是允许的，这也是两者之间的本质区别。因此，你无法实施对象的拷贝和赋值操作，
这些操作都会导致编译失败。unique_ptr只支持“右值引用”的赋值操作，这个是C++11标准中提出的“移动语义”(Move Semantic)，
必须通过移动语义来进行对象的所有权转移。unique_ptr的意图就是保持对对象指针的唯一引用，而“拷贝语义”破坏了这一点，
而“移动语义”则更适用于unique_ptr。unique_ptr还支持空指针的赋值操作，
通过空指针赋值来释放对象的所有权。

```
std::unique_ptr<Object> obj1(new Object());
std::unique_ptr<Object> obj2;
obj2 = std::move(obj1); /// 通过move的方式进行对象指针的所有权转移；所有权转移后，obj1不再持有对象的指针
obj2 = nullptr; /// 将空指针赋值给obj2，obj2释放对象所有权
```


注：nullptr是C++11新引入的关键字，用来表示空指针；std::nullptr_t用来表示空指针类型。用来解决以前使用NULL(0)来表示空指针在函数重载上引入的二义性。

#### 3.2 std::unique_ptr的数组管理使用方法
unique_ptr除了可以管理指针，还可以管理数组，auto_ptr则不能数组的管理。

```
std::unique_ptr<Object[]> obj_array(new Object[10]);
for (unsigned int i = 0; i < 10; i++) {
Object obj = obj_array[i];
......
}
```

数组类型版本unique_ptr重载了[]操作符，使用起来就像普通的数组一样。在离开作用域后，obj_array会自动调用delete []来进行数组的删除。

#### 3.3 使用自定义deleter的std::unique_ptr

前面的文章中介绍过C++的多种new的形式，如果unique_ptr管理的对象指针是通过placement new创建的或者其他的自定义new操作，
那么需要将对应形式的delete操作传递给unique_ptr，以便在消除对象的时候能够正确的删除。unique_ptr接收两个模板参数，
第一个是对象类型，第二个是进行对象删除操作的类型，也叫“函数对象”(Function Object或者Functor)，该类需要重载“()”操作。
第二个模板参数是缺省值，如果用户不指定，就用默认的deleter类型，该类型的对象会使用全局的::operator delete操作来销毁对象。

下面这个例子示例了如何在unique_ptr中使用自定义的deleter。这个例子中，创建的对象是通过placement new操作申请的内存，
在释放时，使用传入的MyDeleter函数对象进行对象的删除。如果仍然使用默认的deleter进行对象删除则会触发程序崩溃。

```
#include <stdlib.h>
#include <iostream>
#include <memory>
class MyDeleter {
public:
  template<class T>
  void operator() (T* ptr) {
	std::cout << "MyDeleter" << std::endl;
	free(ptr);
  }
};
class Object {};
int main() {
  void* buf = malloc(sizeof(Object));
  std::unique_ptr<Object, MyDeleter> obj(new (buf) Object());
  return 0;
}
```

#### 3.4 在STL容器中使用unique_ptr

 

如果在容器中管理原生的对象指针，那么需要在容器销毁时把容器中保存的对象指针也释放，否则就容易发生内存泄漏。
如果在容器中保存智能指针对象不就可以解决这个问题了吗，容器销毁时会调用每个容器中对象的析构函数，进而删除对象指针。
悲剧的是，在C++98标准中，auto_ptr是无法在STL容器中使用的。C++98标准规定了容器中的对象必须满足"CopyConstructible"和"CopyAssignable"两个约束，
也就说STL容器假定了存储的对象是可拷贝和赋值的，并且经过拷贝和赋值后形成了两个独立的对象。但是auto_ptr的拷贝构造函数和赋值操作导致了指针的所有权交换，
经过拷贝和赋值操作后，容器内的auto_ptr把持的对象指针可能变成NULL了！

伴随着“右值引用”的提出，在C++11中，STL容器也都提供了容器对象的"MoveConstructible"和"MoveAssignable"支持，当然也保留了原来的“拷贝语义”。

```
#include <vector>
#include <iostream>
#include <memory>
class Object {
public:
  Object(int i = 0) : i_(i) {}
  void dump() { std::cout << "Dump Object " << i_ << std::endl; }
  ~Object() { std::cout << "Delete Object " << i_ << std::endl; }
private:
  int i_;
};
int main() {
  std::vector<std::unique_ptr<Object> > v;
  for (unsigned int i = 0; i < 10; i++) {
	v.push_back(std::unique_ptr<Object>(new Object(i)));  ///(1)
  }
  std::unique_ptr<Object> obj(new Object(20));
  v.push_back(std::move(obj));							///(2)
  std::unique_ptr<Object> obj2 = std::move(v[3]);		 ///(3)
  obj2->dump();
  return 0;
}
```


这段代码演示了在STL容器中使用unique_ptr来管理对象指针简单用法，这里面有三个地方需要注意：(1)这里调用的vector的push_back成员函数是下面的这个重载函数，参数是“右值引用”而不是以前的“左值应用”，使用“移动语义”避免了一次临时对象拷贝带来的开销。

```
#if __cplusplus >= 201103L
  void push_back(value_type&& __x) { 
   emplace_back(std::move(__x)); 
  }
#endif
```

(2)将一个unique_ptr对象放入容器中时，需要使用“移动语义”，而不能使用“拷贝语义”。如果写成下面的这样，也是会编译失败的。v.push_back(obj); 
原因前面也提到：unique_ptr不支持拷贝构造和赋值，必须使用“移动语义”。(3)将容器中的对象转移到其他对象，此时，
容器中的对象已经不在拥有对象指针的控制权，控制权已经在obj2中了，在obj2的生命周期结束后会释放对象。

这部分介绍有些涉及到C++11的部分特性，而且涉及的细节也很多，可能会有疏漏和错误，大家发现的时候请及时指出来。下期介绍shared_ptr。

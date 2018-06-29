# 了解dynamic_cast

我们的项目中对C方式的“强制类型转换”使用的非常普遍，消息处理、内存管理以及一些对面向对象的继承和多态机制的模拟。
使用“强制类型转换”的基础是开发者对于内存布局的精确了解和掌握，否则不小心就容易出现“数组越界”、“访问非法内存”等错误。

事实上，C语言的内存布局相对简单，使用“强制类型转换”的问题还不大，而C++的对象内存布局相比之下就复杂的多，
比如：我们前面介绍过的由于虚函数引入的`vptr`、`多继承的对象内存布局`、`虚继承下的对象内存布局`、`各种new/delete的内存行为`，
还有`空类的处理`、`空基类优化`、`STL容器类的封装`等等，如果不了解这些机制后面的编译器行为，使用“强制类型转化”就会大概率出错。

C++中提供了4种用于类型转化的表达式，分别是: static_cast, const_cast, dynamic_cast, reinterpret_cast。
此外，C++11的标准模板库中还额外增加了用于智能指针shared_ptr的类型转化，分别是: static_pointer_cast,
dynamic_pointer_cast以及const_pointer_cast。其中，static_cast, const_cast和reinterpret_cast属于编译期确定
的（根据转化的目标类型，可能会有临时对象创建和复制的开销），而dynamic_cast属于运行期执行的，并且dynamic_cast通常
用来在类对象指针之间进行转化，不了解其内部原理则可能会带来一定的误用。本文主要介绍dynamic_cast。

提起在C++实现中使`用dynamic_cast`，一定有人会说这是设计上有问题才导致代码中必须要通过dynamic_cast
来进行对象的类型信息转化。这种说法理论上是没错的，但过于学院派。在设计中是可以通过增加类层次、以及使用
一些设计模式来消除必须要使用dynamic_cast的地方。但是，任何一个大型的软件系统都需要设计、实现、开发周期等多方面进行平衡，
不能简单粗暴的认为是设计问题。

这里只讨论一下使用dynamic_cast需要注意的地方，了解其带来的运行时开销，以及可替代的实现方案。

## 1. dynamic_cast需要依赖类的vtable信息

```
class Base {};

class Derive : public Base {};

int main() {
  Base *b = new Derive();
  Derive *d = dynamic_cast<Derive*>(b);
  delete d;
  return 0;
}
```


这个是一段使用dynamic_cast进行类对象指针的“向下转换”(Down Casting)，也是最常见的一种使用方法。这段代码编译的时会出错，
出错的提示大概是"cannot dynamic_cast Base to Derive type  (source type is not polymorphic)"。使用dynamic_cast进行“向下转换”时，
需要类的类型信息(typeinfo)，而目前基本上所有的C++编译器都把typeinfo信息放在虚函数表(vtable)的头部。上例中，Base基类没有定义任何虚函数，
编译器没有为其以及其派生类Derive生成vtable，无法通过vtable找到typeinfo（实际上编译器在这种情况下也不会生成类型信息），因此会有编译错误。

修改一下也很简单，将基类Base的虚函数声明为virutal即可。

```
class Base {
public:
  virtual ~Base() {}
};

class Derive : public Base {};

int main() {
  Base *b = new Derive();
  Derive *d = dynamic_cast<Derive*>(b);
  delete d;
  return 0;
}
```

修改后的代码可以正确编译和执行。通过-fdump-class-hierarchy可以查看到编译器为Base基类和Derive派生类生成的vtable。其中的_ZTI4Base和ZTI6Derive分别就是指向Base和Derive的typeinfo信息的指针。

Vtable for Base

Base::_ZTV4Base: 4u entries

0     (int (*)(...))0

4     (int (*)(...))(& _ZTI4Base)

8     (int (*)(...))Base::~Base

Vtable for Derive

Derive::_ZTV6Derive: 4u entries

0     (int (*)(...))0

4     (int (*)(...))(& _ZTI6Derive)

8     (int (*)(...))Derive::~Derive

gcc编译器允许通过编译选项来禁止程序使用RTTI(Runtime Type Information)机制，而dynamic_cast机制本身是依赖RTTI机制的，
禁止掉RTTI后，dynamic_cast的使用就无法成功，编译器也会报错。编译时加上-fno-rtti，上面的例子会得到下面的编译错误信息：

error: ‘dynamic_cast’ not permitted with -fno-rtti

我们前面在介绍虚函数开销的时候也介绍过RTTI，如果系统中的类设计很多（大多数都需要用到多态），为每个类生成的typeinfo信息就很可观了，
在嵌入式系统中这个开销还是不小的，除了空间开销外，dynamic_cast利用typeinfo进行类型转化的运行时开销也不小（第3部分介绍）。
因此，一些嵌入式系统会禁止使用dynamic_cast（比如：ROSA-RB目前就禁止使用RTTI），那么如何实现类似的功能来替代C++系统的dynamic_cast呢？
我们在第4部分介绍。

## 2. 指针和引用的dynamic_cast差异

一般对对象指针使用dynamic_cast比较常见，如上面的例子中的“向下转化”。除了“向下转化”外，对象指针的“向上转化”也是自然支持的。

```
int main() {
  Derive*d = new Derive();
  Base *b = dynamic_cast<Base*>(d);
  delete d;
  return 0;
}
```


除了指针外，dynamic_cast还支持对引用类型的类型转化，同样包括“向下转化”和“向上转化”。如下例中，通过dynamic_cast转为为Derive的引用。

```
class Base {
public:
  virtual ~Base() {}
};

class Derive : public Base {
};

int main() {
  Base *b = new Derive();
  Derive& d = dynamic_cast<Derive&>(*b);
  delete b;
  return 0;
}
```

这两者有什么区别吗？从功能上来说，没有区别，但是考虑到错误处理，这两者在平时的使用中还是存在一定的差异。
C++标准5.2.7.9中对dynamic_cast的出错处理有如下说明：

5.2.7.9 The value of a failed cast to pointer type is the null pointer value of the required result type.
A failed cast to reference type throws std::bad_cast.

在对指针进行dynamic_cast处理时，如果失败，则返回一个对应类型的空指针（同时也会抛出std::bad_cast异常），
使用者可以根据返回值是否为空来判断操作的正确性，而不需要一定要catch异常。而对引用进行dynamic_cast处理时，
因为没有所谓的“空引用”，所以失败时只能通过抛出std::bad_cast异常来进行错误后处理。
因此，上面的代码正确的写法应该还要加上异常处理。

```
int main() {
  Base *b = new Derive();
  try {
	 Derive& d = dynamic_cast<Derive&>(*b);
  } catch (std::bad_cast& exp) {  /// 引用的dynamic_cast只能通过catch异常来进行错误后处理
	 delete b;
  }
  delete b;
  return 0;
}
```

大部分嵌入式系统中也会禁止使用异常（ROSA-RB就禁止使用异常），如果必须使用dynamic_cast，对指针进行dynamic_cast，而不要对引用进行dynamic_cast。

## 3. dynamic_cast性能开销


dynamic_cast的实现需要依赖typeinfo，我们这里主要看说一下运行期开销。dynamic_cast并不是完全由编译器生成最终的代码，
还需要依赖C++运行时库的支持。如果反汇编一下上面的程序，可以发现代码中依赖一个外面的符号“__dynamic_cast”。

__dynamic_cast是一个外部函数，其定义和实现是属于C++ ABI的一部分，各个编译器都有自己的实现，
如：gcc在libstdc++v3/libsupc++/dyncast.cc中实现；clang在libcxxabi/private_typeinfo.cpp中实现。
编译器在对dynamic_cast进行代码生成的时候，会生成一个对__dynamic_cast的调用，然后由链接器完成后面的链接。

__dynamic_cast的函数原型如下，其实现和编译器内部实现相关。

```
extern "C" void *
__dynamic_cast (const void *src_ptr,	// object started from
				const __class_type_info *src_type, // type of the starting object
				const __class_type_info *dst_type, // desired target type
				ptrdiff_t src2dst) // how src and dst are related
```

## 4. dynamic_cast替代解决方案

前面我们提到的一些在C++工程中使用dynamic_cast的问题，主要集中在两点：1）空间和时间的开销；2）某些C++项目禁止使用RTTI，会导致dynamic_cast无法使用。需要寻找类似的实现方案。

很多大型的C++系统都会有自己的类型转化系统的实现，举两个例子：

1）Visual C++中的MFC：MFC类库中的所有从CObject派生下来的类都有一个和其关联的CRuntimeClass，用于记录其类型信息，包括类的名字。MFC通过一系列的宏定义，很巧妙的把具有继承关系的类的CRuntimeClass形成一个链表，然后实现类似的类型转化。不过，MFC出现的年代很早，那时候的C++还不完善（可能连dynamic_cast都还没有，或者编译器支持还不完善），除了自己来实现类型系统外，也没什么别的方法。从现代的眼光来看，MFC中的类型系统的实现已经有些落后。

2）CLANG/LLVM: CLANG是现代C/Obj-C/C++编译器前端的实现，其中类型系统的设计就非常高效，也是推荐大家来在一些项目中替代dynamic_cast的一个方案。下面介绍一下CLANG中的动态类型转化的设计。

CLANG中使用的动态类型转化是在LLVM的基础支持库中提供的，包括：llvm::dyn_cast, llvm::is_a, llvm::dyn_cast_or_null。用起来和dynamic_cast很类似，如下面的例子。为了支持这种用法，需要在具有多态的类信息中额外增加一些处理，下面从dyn_cast实现的代码来说明。

```
int main() {
  Base *b = new Derive();
  Derive *d = dyn_cast<Derive>(b);
  delete b;
  return 0;
}
```

因为llvm::dyn_cast同样也支持指针类型和引用类型的动态类型转化，限于篇幅，只分析一下指针类型的动态转化的实现。

```
template <class X, class Y>
inline typename enable_if<
  is_same<Y, typename simplify_type<Y>::SimpleType>,
  typename cast_retty<X, Y*>::ret_type
>::type dyn_cast(Y *Val) {
  return isa<X>(Val) ? cast<X>(Val) : 0;
}
```


这是一个函数模板，返回值类型为：typename enable_if<is_same<Y, typename simplify_type<Y>::SimpleType>, typename cast_retty<X, Y*>::ret_type>::type。这个返回值是根据Y的类型静态推导出来的，首先需要了解enable_if的用法。enable_if是一个基于Metafunction的模板类，
简化的实现类似下面的代码：
	
```
template<bool Cond, class T = void>
struct enable_if {};
 
template<class T>
struct enable_if<true, T> { typedef T type; };
```


1) 如果Cond是true，则enable_if<...>::type就是第二个模板参数中的类型T；

2) 如果Cond是false，则enable_if<...>中没有任何type定义，会有编译失败；

simplify_type<Y>::SimpleType通过模板的特化来处理Y类型是经过const修饰的，去除const后的类型；cast_retty<X, Y*>::ret_type 
	也是通过一系列的模板特化来处理Y是 const *, *const， const *const等多种const修饰类型。

dyn_cast的实现比较简单，先通过isa<X>(Val)来判断是否有继承关系，如果是向下转化“Down-Casting”或者是向上转化“Up-Casting”，
	则通过cast<X>(Val)进行转化，否则返回空指针。isa的实现也用了很多的模板特化来处理const修饰，我们只看下最关键的两个模板函数：
```
template <typename To, typename From, typename Enabler = void>
struct isa_impl {
  static inline bool doit(const From &Val) {
	return To::classof(&Val);
  }
};

template <typename To, typename From>
struct isa_impl<To, From,
				typename enable_if<
				  llvm::is_base_of<To, From>
				>::type
			   > {
  static inline bool doit(const From &) { return true; }
```

这段代码中包括两个模板函数，下面是上面的部分特化版本，这里使用了C++模板的一个技巧SFINAE(Substitution Fail Is Not An Error)。
首先，模板的静态推导会尝试特化版本，通过enable_if<llvm::is_base_of<To, From> >::type来判断是否是一个向上转化“Up-Casting”，如果是（llvm::is_base_of<To, From>为true），则enable_if<...>::type是有定义的，且默认为void类型，则推导成功，直接返回true；如果不是llvm::is_base_of<To, From>为false），enable_if<...>::type是无定义的，则推导失败。这个时候编译器并不直接报错，而是模板实例化尝
试使用上面的非特化版本，通过调用目标类的classof静态成员函数来判断是否具有继承关系。

因此，LLVM的这种类型系统使用需要的用户在编写类的时候额外提供classof静态成员函数来判断是否具有继承关系，例子如下：
```
class Sharp {
public:
  Sharp(Kind k) : k_(k) {}
  enum Kind {
	CIRCLE = 0,
  };
  Kind getkind() const { return k_; }
private:
  Kind k_;
};

class Circle : public Base {
public:
  Circle() : Base(CIRCLE) {}
  
  static bool classof(const Base* b) {
	return b->getkind() == CIRCLE;
  }
};

Sharp* s = new Circle();
Circle *c = dyn_cast<Circle>(s);
```
	

基类中需要记录类型枚举，每个派生类在创建的时候指定该类型，通过保存的这个类型枚举在运行时判断是否具有继承关系。

这种类型系统的优势有几个方面：

1) 不依赖编译器实现：不需要依赖vtable中的typeinfo，即便禁用了RTTI，也不影响该机制；

2) 效率高：空间上的开销是每个类对象大小多个4bytes，运行时只有一个简单的判断，getkind通常都会被inline优化；

3) 编译期推断：向上转化“Up-Casting”在编译器就可以完成；而对于向下转化“Down-Casting”，如果开发者忘记在类中提供classof静态成员函数，
则编译的时候就会报错，将出错的地方提前到编译期，这个是非常重要的。

缺点：

1) 大量的模板实例化和部分实例化带来一定的代码膨胀，不过由于这些函数模板都很小，实际上的膨胀也不严重；

2) 一些模板部分实例化以及SFINAE用法在版本较老的GCC编译器上支持还不成熟，某些单板上没法用。

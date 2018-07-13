## static 类成员
对于特定类类型的全体对象而言，访问一个 **`全局对象`** 有时是必要的。也许，
在程序的任意点需要统计已创建的特定类类型对象的数量；或者，全局对象可能
是指向类的错误处理例程的一个指针；或者，它是指向类类型对象的内在自由存
储区的一个指针。
然而，**`全局对象会破坏封装`**：对象需要支持特定类抽象的实现。如果对象是
全局的，一般的用户代码就可以修改这个值。类可以定义类 静态成员，而不是
定义一个可普遍访问的全局对象。
通常，非 static 数据成员存在于类类型的每个对象中。不像普通的数据成
员，static 数据成员独立于该类的任意对象而存在； **每个 static 数据成员是
与类关联的对象，并不与该类的对象相关联。**
正如类可以定义共享的 static 数据成员一样，类也可以定义 static 成员
函数。static 成员函数没有 this 形参， **它可以直接访问所属类的 static 成
员，但不能直接使用非 static 成员**。

### 使用类的 static 成员的优点
使用 static 成员而不是全局对象有三个优点。
- 1. static 成员的名字是在类的作用域中，因此可以避免与其他类的成员或
全局对象名字冲突。
- 2. 可以实施封装。static 成员可以是私有成员，而全局对象不可以。
- 3. 通过阅读程序容易看出 static 成员是与特定类关联的。这种可见性可清
晰地显示程序员的意图。

## 定义 static 成员
在成员声明前加上关键字 static 将成员设为 static。static 成员遵循正
常的公有／私有访问规则。
例如，考虑一个简单的表示银行账户的类。每个账户具有余额和拥有者，并
且按月

```
class Account {
public:
// interface functions here
void applyint() { amount += amount * interestRate; }
static double rate() { return interestRate; }
static void rate(double); // sets a new rate
private:
std::string owner;
double amount;
static double interestRate;
static double initRate();
};
```
这个类的每个对象具有两个数据成员：owner 和 amount。对象没有与
static 数据成员对应的数据成员，但是，存在一个单独的 interestRate 对象，
由 Account 类型的全体对象共享。

### 使用类的 static 成员
可以通过作用域操作符从类直接调用 static 成员，或者通过对象、引用或
指向该类类型对象的指针间接调用。

```
Account ac1;
Account *ac2 = &ac1;
// equivalent ways to call the static member rate function
double rate;
rate = ac1.rate(); // through an Account object or reference
rate = ac2->rate(); // through a pointer to an Account object
rate = Account::rate(); // directly from the class using the scope
operator
```

像使用其他成员一样，类成员函数可以不用作用域操作符来引用类的

```
static 成员：
class Account {
public:
// interface functions here
void applyint() { amount += amount * interestRate; }
};
```

注：此出处为C++Primer中文版第四版

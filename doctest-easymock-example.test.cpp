// https://github.com/matepek/catch2-with-gmock

// NOTE that next line includes gmock and doctest headers automatically.
#include "ThirdParty/DOCTestWithMainAndGMock.hpp"
#include "ThirdParty/MockScopeGuard.hpp"
#include "easymock.h"
//实际上对 easymock 的实现做了微调，以支持新版本的gmock。

using namespace std;

///

class Foo {
 public:
  virtual ~Foo() {}
  virtual bool Bar() const = 0;
};

class MockFoo : public Foo {
 public:
  MOCK_METHOD(bool, Bar, (), (const, override));
};

///

const char* func() {
    return "Non mocked.";
}

TEST_CASE("example #1") {
  ::testing::StrictMock<MockFoo> mock1, mock2;
  Foo& foo1 = mock1;

  SUBCASE("success") {
    auto mockGuard = MockScopeGuard(mock1, mock2);

    EXPECT_CALL(mock1, Bar).WillOnce(::testing::Return(true));

    REQUIRE(true == foo1.Bar());
  }

  SUBCASE("failure #1") {
    auto mockGuard = MockScopeGuard(mock1, mock2);

    EXPECT_CALL(mock1, Bar).WillOnce(::testing::Return(true));
  }

  SUBCASE("failure #2") {
    auto mockGuard = MockScopeGuard(mock1, mock2);

    EXPECT_CALL(mock2, Bar).WillOnce(::testing::Return(true));
  }

  SUBCASE("静态函数的例子 1/3/8/9")
  {
    auto mocker = CREATE_MOCKER(func);
    //MOCK_FUNCTION 的括号内是有几个待定参数，里面有几个 _。没有参数的场合就是空的。
    //后面的其他部分遵循 gmock 的语法即可。
    EXPECT_CALL(*mocker, MOCK_FUNCTION()).Times(::testing::Exactly(1)).WillOnce(::testing::Return("Hello world."));
    REQUIRE("Hello world." == func());
    CLEAR_MOCKER(mocker);
  }

  


}

class MyClass
{
public:
    static int Method1(int val)
    {
        return val;
    }

    int Method2(int val)
    {
        return val;
    }
};

TEST_CASE("类打桩")
{
    SUBCASE("类静态函数")
    {
        /*
            在这个测试案例中，使用CREATE_MOCKER创建了一个指向MyClass类中Method1函数的指针，并使用EXPECT_CALL定义了该函数的期望行为。
            接下来，通过接口指针调用了该类的Method1静态函数。
            最后使用CLEAR_MOCKER清除了创建的模拟对象。
        */
        auto mocker = CREATE_MOCKER(MyClass::Method1);
        //MOCK_FUNCTION 的括号内是有几个待定参数，里面有几个 _。其中this也算是一个待定参数
        //后面的其他部分遵循 gmock 的语法即可。
        EXPECT_CALL(*mocker, MOCK_FUNCTION(testing::_)).Times(::testing::Exactly(1)).WillOnce(::testing::Return(114));
        REQUIRE(114 == MyClass::Method1(123));
        CLEAR_MOCKER(mocker);
    }

    SUBCASE("类成员函数")
    {
        /*
        在后面的测试中，使用了一个名为 CREATE_MOCKER 的函数来创建 MyClass::Method2 的模拟器，
        并使用 EXPECT_CALL 和 WillOnce 函数定义了该函数的行为，期望该函数在执行一次后返回 114。

        最后，使用 REQUIRE 函数检查函数的返回值是否与预期的 114 相等，
        并使用 CLEAR_MOCKER 函数清空模拟器。
        */
        auto mocker = CREATE_MOCKER(&MyClass::Method2);
        //MOCK_FUNCTION 的括号内是有几个待定参数，里面有几个 _。其中this也算是一个待定参数
        //后面的其他部分遵循 gmock 的语法即可。
        EXPECT_CALL(*mocker, MOCK_FUNCTION(testing::_, testing::_)).Times(::testing::Exactly(1)).WillOnce(::testing::Return(114));
        MyClass obj;
        REQUIRE(114 == obj.Method2(123));
        CLEAR_MOCKER(mocker);
        ::testing::Mock::VerifyAndClear(&*mocker);//如果未来还需要对相关的函数进行mock，那么请显式调用这个函数进行Clear操作。
    }

    SUBCASE("类成员函数-复杂返回值")
    {
        /*
        这段代码使用了 MockScopeGuard，它是一个 RAII 类，用于在作用域结束时自动调用 ::testing::Mock::VerifyAndClear() 函数来验证所有的 mock 预期和清除它们。
        与调用 VerifyAndClear 函数相比，这种方式更加安全，因为它可以在更多的异常情况下正常工作。
        使用这个类需要在作用域开始时实例化对象，传入 CREATE_MOCKER 返回的 mocker 对象，然后在作用域结束时自动清理 mocker。

        具体地，这段代码先创建了 mocker 对象，然后使用 MockScopeGuard 对象来进行mock预期的验证和清理。
        接下来，ENABLE_MOCKER 用于允许使用 mocker 对象进行 mock 预期，EXPECT_CALL 宏用于设置预期行为，这里的预期行为是将方法的输入参数乘以 2 并返回，
        这个预期行为会在 Method2 方法被调用时被执行。然后，创建 MyClass 对象 obj 并调用其 Method2 方法。
        然后，REQUIRE 宏用于检查调用 Method2 方法后的返回值是否为 228。
        最后，使用 CLEAR_MOCKER 清空 mocker 对象的所有预期，并使用 MockScopeGuard 对象来验证所有预期和清除它们。
        */
        auto mocker = CREATE_MOCKER(&MyClass::Method2);
        //::testing::Mock::VerifyAndClear(&*mocker); 也可以用下面的语句代替：
        auto mockGuard = MockScopeGuard(*mocker);
        ENABLE_MOCKER(mocker);//重复对同一个函数进行mock的场合，如果上一次 CLEAR 了，那么需要重新ENABLE
        EXPECT_CALL(*mocker, MOCK_FUNCTION(testing::_, testing::_)).Times(::testing::Exactly(1)).WillOnce(testing::Invoke([&](const void* this_ptr, int input){
                return input * 2;
        }));
        MyClass obj;
        REQUIRE(228 == obj.Method2(114));//此处检查函数的行为被替换为了返回双倍的结果。
        CLEAR_MOCKER(mocker);
        
    }

    SUBCASE("类成员函数-检查参数")
    {
        /*
        这段代码中的 SUBCASE 测试了一个名为 Method2 的类成员函数，并检查它的参数是否正确。
        在测试过程中，使用 CREATE_MOCKER 创建了 MyClass::Method2 的模拟器，并使用 ENABLE_MOCKER 函数启用它。
        接下来，使用 EXPECT_CALL 定义了期望函数的参数，期望参数值等于 123，函数被调用一次，并且返回值为 114。
        接着，创建了一个 MyClass 的实例对象，并调用了它的 Method2 函数，但是参数值为 514，不符合期望值。因此，根据EXPECT_CALL 中的检查项，直接报错.
        最后，使用 CLEAR_MOCKER 清除模拟器，并使用 ::testing::Mock::VerifyAndClear 函数对模拟器进行验证和清除。
        */
        auto mocker = CREATE_MOCKER(&MyClass::Method2);
        ENABLE_MOCKER(mocker);//重复对同一个函数进行mock的场合，如果上一次 CLEAR 了，那么需要重新ENABLE
        EXPECT_CALL(*mocker, MOCK_FUNCTION(testing::_, testing::Eq(123))).Times(::testing::Exactly(1)).WillOnce(::testing::Return(114));
        MyClass obj;
        REQUIRE(514 != obj.Method2(514));//不符合参数条件，直接报错
        CLEAR_MOCKER(mocker);
        ::testing::Mock::VerifyAndClear(&*mocker);
    }

    
}

class Interface
{
public:
    virtual int Func(int b) = 0;
};

class Impl : public Interface
{
public:
    int Func(int b) override
    {
        return 888 + b;
    }
};

TEST_CASE("特定的虚函数")
{

    SUBCASE("类成员函数（虚）")
    {
        /*
            在这个测试案例中，使用CREATE_MOCKER创建了一个指向Impl类中Func函数的指针，并使用EXPECT_CALL定义了该函数的期望行为。
            接下来，创建了一个Impl对象，并通过接口指针调用了该对象的Func函数。
            最后使用CLEAR_MOCKER清除了创建的模拟对象。
        */
        auto mocker = CREATE_MOCKER(&Impl::Func);
        //MOCK_FUNCTION 的括号内是有几个待定参数，里面有几个 _。其中this也算是一个待定参数
        EXPECT_CALL(*mocker, MOCK_FUNCTION(testing::_, testing::Eq(123))).Times(::testing::Exactly(1)).WillOnce(testing::Invoke([&](const void* this_ptr, int i){
                return 114;
        }));
        Impl obj;
        Interface* ptr = &obj;

        REQUIRE(114 == ptr->Func(123));
        CLEAR_MOCKER(mocker);
    }
    //gmock也支持虚函数的mock操作，不过这里只演示使用easymock的。使用gmock的特性也可以做到同样的事情。
    //对重载函数的打桩主要是指定要选择的重载。此处可参考：https://github.com/vesavlad/easymock/wiki/Mocking-overloaded-methods
}
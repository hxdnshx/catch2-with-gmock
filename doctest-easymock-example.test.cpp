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
        auto mocker = CREATE_MOCKER(MyClass::Method1);
        //MOCK_FUNCTION 的括号内是有几个待定参数，里面有几个 _。其中this也算是一个待定参数
        //后面的其他部分遵循 gmock 的语法即可。
        EXPECT_CALL(*mocker, MOCK_FUNCTION(testing::_)).Times(::testing::Exactly(1)).WillOnce(::testing::Return(114));
        REQUIRE(114 == MyClass::Method1(123));
        CLEAR_MOCKER(mocker);
    }

    SUBCASE("类成员函数")
    {
        auto mocker = CREATE_MOCKER(&MyClass::Method2);
        //MOCK_FUNCTION 的括号内是有几个待定参数，里面有几个 _。其中this也算是一个待定参数
        //后面的其他部分遵循 gmock 的语法即可。
        EXPECT_CALL(*mocker, MOCK_FUNCTION(testing::_, testing::_)).Times(::testing::Exactly(1)).WillOnce(::testing::Return(114));
        MyClass obj;
        REQUIRE(114 == obj.Method2(123));
        CLEAR_MOCKER(mocker);
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
        auto mocker = CREATE_MOCKER(&Impl::Func);
        //MOCK_FUNCTION 的括号内是有几个待定参数，里面有几个 _。其中this也算是一个待定参数
        EXPECT_CALL(*mocker, MOCK_FUNCTION(testing::_, testing::_)).Times(::testing::Exactly(1)).WillOnce(::testing::Return(114));
        Impl obj;
        Interface* ptr = &obj;

        REQUIRE(114 == ptr->Func(123));
        CLEAR_MOCKER(mocker);
    }
    //gmock也支持虚函数的mock操作，不过这里只演示使用easymock的。使用gmock的特性也可以做到同样的事情。
    //对重载函数的打桩主要是指定要选择的重载。此处可参考：https://github.com/vesavlad/easymock/wiki/Mocking-overloaded-methods
}
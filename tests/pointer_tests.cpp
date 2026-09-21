#include "SharedPtr.hpp"
#include "UniquePtr.hpp"

#include <gtest/gtest.h>

struct DeleteInt {
  int *calls;

  void operator()(int *ptr) const noexcept {
    ++(*calls);
    delete ptr;
  }
};

TEST(UniquePtrTest, MoveAndRelease) {
  UniquePtr<int> owner;
  EXPECT_FALSE(owner);
  owner.reset(new int(7));
  UniquePtr<int> moved(Move(owner));
  EXPECT_FALSE(owner);
  ASSERT_NE(moved.get(), nullptr);
  EXPECT_EQ(*moved, 7);

  owner.reset(new int(9));
  owner = Move(moved);
  EXPECT_FALSE(moved);
  ASSERT_NE(owner.get(), nullptr);
  EXPECT_EQ(*owner, 7);

  int *raw = owner.release();
  ASSERT_FALSE(owner);
  ASSERT_NE(raw, nullptr);
  EXPECT_EQ(*raw, 7);
  delete raw;
}

TEST(UniquePtrTest, SelfMoveAndReset) {
  UniquePtr<int> owner(new int(7));
  owner.reset(owner.get());
  UniquePtr<int> &same = owner;
  owner = Move(same);

  ASSERT_NE(owner.get(), nullptr);
  EXPECT_EQ(*owner, 7);
}

TEST(UniquePtrTest, Array) {
  UniquePtr<int[]> array(new int[3]{1, 2, 3});
  ASSERT_NE(array.get(), nullptr);
  array[1] = 7;
  UniquePtr<int[]> moved(Move(array));
  EXPECT_FALSE(array);
  ASSERT_NE(moved.get(), nullptr);
  EXPECT_EQ(moved[1], 7);

  array.reset(new int[1]{9});
  array = Move(moved);
  EXPECT_FALSE(moved);
  ASSERT_NE(array.get(), nullptr);
  EXPECT_EQ(array[1], 7);
}

TEST(UniquePtrTest, CustomDeleter) {
  int firstCalls = 0;
  int secondCalls = 0;
  {
    UniquePtr<int, DeleteInt> first(new int(1), {&firstCalls});
    UniquePtr<int, DeleteInt> second(new int(2), {&secondCalls});
    first = Move(second);
    EXPECT_EQ(firstCalls, 1);
    EXPECT_EQ(secondCalls, 0);

    first.swap(second);
    EXPECT_FALSE(first);
    EXPECT_EQ(second.get_deleter().calls, &secondCalls);
  }
  EXPECT_EQ(secondCalls, 1);
}

TEST(UniquePtrTest, DerivedToBase) {
  struct Base {
    virtual ~Base() = default;
  };

  struct Derived : Base {
    bool *deleted;

    Derived(bool *flag) : deleted(flag) {}

    ~Derived() override { *deleted = true; }
  };

  bool deleted = false;
  UniquePtr<Derived> derived(new Derived(&deleted));
  UniquePtr<Base> base(Move(derived));
  EXPECT_FALSE(derived);
  base.reset();
  EXPECT_TRUE(deleted);

  deleted = false;
  base = UniquePtr<Derived>(new Derived(&deleted));
  base.reset();
  EXPECT_TRUE(deleted);
}

TEST(SharedPtrTest, Copy) {
  SharedPtr<int> first;
  EXPECT_FALSE(first);
  EXPECT_EQ(first.use_count(), 0);
  first.reset(new int(10));
  SharedPtr<int> second(first);

  ASSERT_NE(first.get(), nullptr);
  ASSERT_EQ(second.get(), first.get());
  *second = 20;
  EXPECT_EQ(*first, 20);
  EXPECT_EQ(first.use_count(), 2);
  EXPECT_EQ(second.use_count(), 2);
}

TEST(SharedPtrTest, Assignment) {
  SharedPtr<int> first(new int(20));
  SharedPtr<int> second(first);
  SharedPtr<int> third(new int(30));
  third = second;
  EXPECT_EQ(first.use_count(), 3);
  first.reset();

  second = Move(third);
  EXPECT_FALSE(third);
  EXPECT_EQ(second.use_count(), 1);
  ASSERT_NE(second.get(), nullptr);
  EXPECT_EQ(*second, 20);

  second.reset();
  EXPECT_FALSE(second);
  EXPECT_EQ(second.use_count(), 0);
}

TEST(SharedPtrTest, SelfAssignment) {
  SharedPtr<int> owner(new int(20));
  SharedPtr<int> &same = owner;
  owner = same;
  owner = Move(same);
  owner.reset(owner.get());

  ASSERT_NE(owner.get(), nullptr);
  EXPECT_EQ(*owner, 20);
  EXPECT_EQ(owner.use_count(), 1);
}

TEST(SharedPtrTest, Array) {
  SharedPtr<int[]> array(new int[2]{4, 5});
  SharedPtr<int[]> copy(array);
  ASSERT_NE(array.get(), nullptr);
  ASSERT_EQ(copy.get(), array.get());
  copy[1] = 8;
  EXPECT_EQ(array[1], 8);

  array = copy;
  array = Move(copy);
  EXPECT_FALSE(copy);
  EXPECT_EQ(array.use_count(), 1);
  ASSERT_NE(array.get(), nullptr);
  EXPECT_EQ(array[1], 8);
}

TEST(SharedPtrTest, CustomDeleter) {
  int deleted = 0;
  {
    SharedPtr<int> first(new int(3), DeleteInt{&deleted});
    SharedPtr<int> last(first);
    first.reset();
    EXPECT_EQ(deleted, 0);
  }
  EXPECT_EQ(deleted, 1);
}

TEST(SharedPtrTest, DerivedDeleter) {
  struct Base {
    int value = 7;
  };

  struct Derived : Base {
    bool *deleted;

    Derived(bool *flag) : deleted(flag) {}

    ~Derived() { *deleted = true; }
  };

  bool deleted = false;
  {
    SharedPtr<Derived> owner(new Derived(&deleted));
    SharedPtr<Base> copy(owner);
    SharedPtr<const Base> view;
    view = owner;
    ASSERT_NE(view.get(), nullptr);
    EXPECT_EQ(view->value, 7);

    copy = Move(owner);
    EXPECT_FALSE(owner);
    EXPECT_EQ(view.use_count(), 2);
    copy.reset();
    EXPECT_FALSE(deleted);
  }
  EXPECT_TRUE(deleted);

  deleted = false;
  { SharedPtr<Base> direct(new Derived(&deleted)); }
  EXPECT_TRUE(deleted);
}

TEST(PointerTest, ArrayDestruction) {
  struct Element {
    int *deleted;

    ~Element() { ++(*deleted); }
  };

  int deleted = 0;
  {
    UniquePtr<Element[]> unique(new Element[2]{{&deleted}, {&deleted}});
    SharedPtr<Element[]> owner(new Element[2]{{&deleted}, {&deleted}});
    SharedPtr<Element[]> last(owner);
    owner.reset();
    EXPECT_EQ(deleted, 0);
  }
  EXPECT_EQ(deleted, 4);
}

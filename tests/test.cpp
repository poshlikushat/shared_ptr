#include <gtest/gtest.h>
#include "../src/shared_ptr.hpp"

struct Counter {
    static int alive;
    int v;
    explicit Counter(int x = 0) : v(x) { ++alive; }
    ~Counter() { --alive; }
    int inc() { return ++v; }
    [[nodiscard]] int get() const { return v; }
};
int Counter::alive = 0;

TEST(SharedPtr, DefaultConstruct) {
    SharedPtr<int> p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p.get_count(), 0);
}

TEST(SharedPtr, MakeSharedBasic) {
    auto p = make_shared<int>(42);
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(*p, 42);
    EXPECT_EQ(p.get_count(), 1);
}

TEST(SharedPtr, OperatorArrowAndStar) {
    auto p = make_shared<Counter>(7);
    EXPECT_EQ(p->get(), 7);
    EXPECT_EQ((*p).get(), 7);
    EXPECT_EQ(p.get_count(), 1);
    EXPECT_EQ(p->inc(), 8);
    EXPECT_EQ((*p).get(), 8);
}

TEST(SharedPtr, CopyIncrementsCount) {
    auto p1 = make_shared<int>(5);
    EXPECT_EQ(p1.get_count(), 1);
    {
        SharedPtr<int> p2 = p1;
        EXPECT_EQ(p1.get(), p2.get());
        EXPECT_EQ(p1.get_count(), 2);
        EXPECT_EQ(p2.get_count(), 2);
    }
    EXPECT_EQ(p1.get_count(), 1);
}

TEST(SharedPtr, CopyAssignment) {
    const auto p1 = make_shared<int>(10);
    auto p2 = make_shared<int>(20);
    EXPECT_EQ(p1.get_count(), 1);
    EXPECT_EQ(p2.get_count(), 1);
    p2 = p1;
    EXPECT_EQ(p1.get_count(), 2);
    EXPECT_EQ(p2.get_count(), 2);
    EXPECT_EQ(*p2, 10);
}

TEST(SharedPtr, MoveConstructorTransfersOwnership) {
    auto src = make_shared<int>(77);
    EXPECT_EQ(src.get_count(), 1);
    const SharedPtr<int> dst(std::move(src));
    EXPECT_EQ(src.get(), nullptr);
    EXPECT_EQ(src.get_count(), 0);
    EXPECT_NE(dst.get(), nullptr);
    EXPECT_EQ(dst.get_count(), 1);
    EXPECT_EQ(*dst, 77);
}

TEST(SharedPtr, MoveAssignmentReleasesOldAndTakesNew) {
    auto a = make_shared<int>(1);
    auto b = make_shared<int>(2);
    EXPECT_EQ(a.get_count(), 1);
    EXPECT_EQ(b.get_count(), 1);
    a = std::move(b);
    EXPECT_EQ(b.get(), nullptr);
    EXPECT_EQ(b.get_count(), 0);
    EXPECT_NE(a.get(), nullptr);
    EXPECT_EQ(*a, 2);
    EXPECT_EQ(a.get_count(), 1);
}

TEST(SharedPtr, ResetToNullReleases) {
    EXPECT_EQ(Counter::alive, 0);
    SharedPtr<Counter> p = make_shared<Counter>(9);
    EXPECT_EQ(Counter::alive, 1);
    p.reset();
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p.get_count(), 0);
    EXPECT_EQ(Counter::alive, 0);
}

TEST(SharedPtr, ResetToNewPointer) {
    EXPECT_EQ(Counter::alive, 0);
    {
        SharedPtr<Counter> p = make_shared<Counter>(1);
        EXPECT_EQ(Counter::alive, 1);
        p.reset(new Counter(5));
        EXPECT_EQ(Counter::alive, 1);
        EXPECT_EQ(p->get(), 5);
        EXPECT_EQ(p.get_count(), 1);
    }
    EXPECT_EQ(Counter::alive, 0);
}

TEST(SharedPtr, SwapExchangesControlBlocks) {
    auto p1 = make_shared<int>(100);
    auto p2 = make_shared<int>(200);
    size_t c1 = p1.get_count();
    size_t c2 = p2.get_count();
    p1.swap(p2);
    EXPECT_EQ(*p1, 200);
    EXPECT_EQ(*p2, 100);
    EXPECT_EQ(p1.get_count(), c2);
    EXPECT_EQ(p2.get_count(), c1);
}

TEST(SharedPtr, RefCountAcrossCopiesAndResets) {
    SharedPtr<int> a = make_shared<int>(3);
    SharedPtr<int> b = a;
    SharedPtr<int> c = b;
    EXPECT_EQ(a.get_count(), 3);
    b.reset();
    EXPECT_EQ(a.get_count(), 2);
    c.reset();
    EXPECT_EQ(a.get_count(), 1);
}

TEST(SharedPtr, NullDereferenceThrows) {
    SharedPtr<int> p;
    EXPECT_THROW(*p, std::runtime_error);
    EXPECT_THROW(p.operator->(), std::runtime_error);
}

TEST(SharedPtr, DestructionHappensOnce) {
    EXPECT_EQ(Counter::alive, 0);
    {
        auto p1 = make_shared<Counter>(10);
        {
            auto p2 = p1;
            EXPECT_EQ(Counter::alive, 1);
            EXPECT_EQ(p1.get_count(), 2);
            EXPECT_EQ(p2.get_count(), 2);
        }
        EXPECT_EQ(Counter::alive, 1);
    }
    EXPECT_EQ(Counter::alive, 0);
}

TEST(SharedPtr, GetReturnsRawPointer) {
    const auto p = make_shared<int>(9);
    int* raw = p.get();
    EXPECT_NE(raw, nullptr);
    EXPECT_EQ(*raw, 9);
}

TEST(SharedPtr, WorksWithConstObjects) {
    const auto p = make_shared<const Counter>(4);
    EXPECT_EQ(p->get(), 4);
}

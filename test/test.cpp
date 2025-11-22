#include <gtest/gtest.h>
#include "../include/fixed_block_memory_resource.h"
#include "../include/stack.h"

class StackTest : public ::testing::Test {
protected:
    FixedBlockMemoryResource* resource;

    void SetUp() override {
        resource = new FixedBlockMemoryResource(4096);
    }

    void TearDown() override {
        delete resource;
    }
};

// Тесты для стека с int
TEST_F(StackTest, EmptyStackTest) {
    Stack<int> stack(resource);
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
}

TEST_F(StackTest, PushTest) {
    Stack<int> stack(resource);
    stack.push(10);
    
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.top(), 10);
}

TEST_F(StackTest, MultiplePushTest) {
    Stack<int> stack(resource);
    stack.push(10);
    stack.push(20);
    stack.push(30);
    
    EXPECT_EQ(stack.size(), 3);
    EXPECT_EQ(stack.top(), 30);
}

TEST_F(StackTest, PopTest) {
    Stack<int> stack(resource);
    stack.push(10);
    stack.push(20);
    
    stack.pop();
    EXPECT_EQ(stack.size(), 1);
    EXPECT_EQ(stack.top(), 10);
}

TEST_F(StackTest, PopEmptyStackTest) {
    Stack<int> stack(resource);
    stack.pop(); // Не должно вызывать ошибку
    EXPECT_TRUE(stack.empty());
}

TEST_F(StackTest, TopEmptyStackTest) {
    Stack<int> stack(resource);
    EXPECT_THROW(stack.top(), std::out_of_range);
}

TEST_F(StackTest, MemoryReuseTest) {
    Stack<int> stack(resource);
    
    // Добавляем элементы
    for (int i = 0; i < 10; ++i) {
        stack.push(i);
    }
    
    // Удаляем половину
    for (int i = 0; i < 5; ++i) {
        stack.pop();
    }
    
    // Добавляем снова - должна переиспользоваться память
    for (int i = 0; i < 5; ++i) {
        stack.push(i + 100);
    }
    
    EXPECT_EQ(stack.size(), 10);
}

// Тесты итератора
TEST_F(StackTest, IteratorTest) {
    Stack<int> stack(resource);
    stack.push(1);
    stack.push(2);
    stack.push(3);
    
    int expected[] = {3, 2, 1};
    int i = 0;
    for (auto val : stack) {
        EXPECT_EQ(val, expected[i++]);
    }
}

TEST_F(StackTest, EmptyIteratorTest) {
    Stack<int> stack(resource);
    int count = 0;
    for (auto val : stack) {
        (void)val;
        count++;
    }
    EXPECT_EQ(count, 0);
}

// Тесты со сложными типами
struct TestStruct {
    int x;
    double y;
    
    bool operator==(const TestStruct& other) const {
        return x == other.x && y == other.y;
    }
};

TEST_F(StackTest, ComplexTypeTest) {
    Stack<TestStruct> stack(resource);
    
    TestStruct s1{10, 1.5};
    TestStruct s2{20, 2.5};
    
    stack.push(s1);
    stack.push(s2);
    
    EXPECT_EQ(stack.top().x, 20);
    EXPECT_EQ(stack.top().y, 2.5);
    
    stack.pop();
    EXPECT_EQ(stack.top().x, 10);
    EXPECT_EQ(stack.top().y, 1.5);
}

// Тесты Memory Resource
TEST(MemoryResourceTest, AllocationTest) {
    FixedBlockMemoryResource mr(1024);
    
    void* ptr1 = mr.allocate(32);
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = mr.allocate(64);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_NE(ptr1, ptr2);
    
    mr.deallocate(ptr1, 32);
    mr.deallocate(ptr2, 64);
}

TEST(MemoryResourceTest, MemoryReuseTest) {
    FixedBlockMemoryResource mr(512);
    
    void* ptr1 = mr.allocate(64);
    mr.deallocate(ptr1, 64);
    
    void* ptr2 = mr.allocate(64);
    // Должна переиспользоваться та же область памяти
    EXPECT_EQ(ptr1, ptr2);
}

TEST(MemoryResourceTest, OutOfMemoryTest) {
    FixedBlockMemoryResource mr(128);
    
    void* ptr;
    EXPECT_THROW( (ptr = mr.allocate(256), true), std::bad_alloc);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
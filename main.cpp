#include "include/fixed_block_memory_resource.h"
#include "include/stack.h"
#include <iostream>
#include <string>

struct Person {
    int id;
    double salary;
    char name[20];

    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        return os << "{id:" << p.id << ", salary:" << p.salary 
                  << ", name:" << p.name << "}";
    }
};

int main() {
    try {
        FixedBlockMemoryResource my_resource(2048);

        //Тест 1: Стек с целыми числами
        std::cout << "Test 1: Integer Stack" << std::endl;
        Stack<int> int_stack(&my_resource);

        int_stack.push(10);
        int_stack.push(20);
        int_stack.push(30);
        int_stack.push(40);

        std::cout << "Stack contents: ";
        for (auto val : int_stack) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        std::cout << "Top element: " << int_stack.top() << std::endl;
        
        int_stack.pop();
        std::cout << "After pop: ";
        for (auto val : int_stack) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        int_stack.push(50);
        std::cout << "After pushing 50: ";
        for (auto val : int_stack) {
            std::cout << val << " ";
        }
        std::cout << std::endl << std::endl;

        // Тест 2: Стек со сложными типами
        std::cout << "Test 2: Complex Types Stack" << std::endl;
        Stack<Person> person_stack(&my_resource);

        Person p1 = {1, 50000.0, "Ivan"};
        Person p2 = {2, 60000.0, "Maria"};
        Person p3 = {3, 75000.0, "Alexey"};

        person_stack.push(p1);
        person_stack.push(p2);
        person_stack.push(p3);

        std::cout << "Employees in stack:" << std::endl;
        for (const auto& person : person_stack) {
            std::cout << "  " << person << std::endl;
        }

        std::cout << "\nTop element: " << person_stack.top() << std::endl;
        
        person_stack.pop();
        std::cout << "After pop: " << std::endl;
        for (const auto& person : person_stack) {
            std::cout << "  " << person << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#include <iostream>
#include "shared_ptr.hpp"

int main() {
	auto p1 = make_shared<int>(42);
	std::cout << "p1 count: " << p1.get_count() << ", value: " << *p1 << '\n';

	SharedPtr<int> p2 = p1;
	std::cout << "p1 count after copy: " << p1.get_count() << '\n';
	std::cout << "p2 count after copy: " << p2.get_count() << '\n';

	const SharedPtr<int> p3 = std::move(p1);
	std::cout << "p3 count after move: " << p3.get_count() << '\n';
	std::cout << "p1 count after move: " << p1.get_count() << '\n';

	p2.reset(new int(99));
	std::cout << "p2 reset to new value: " << *p2 << ", count: " << p2.get_count() << '\n';

	SharedPtr<int> p4 = std::move(p2);
	std::cout << "p4 after move-assign: " << *p4 << ", count: " << p4.get_count() << '\n';

	p4.reset();
	std::cout << "p4 after reset: " << p4.get_count() << '\n';
}


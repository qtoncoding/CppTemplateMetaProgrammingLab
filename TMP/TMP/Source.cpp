#include <tuple>
#include <iostream>
#include <numeric>
#include <utility>

// Compile with Clang with /std=c++1z for C++17 fold expression

///////////////////////////////////////////////////////////////
/// LAB 1: Variadic templates - Euclidean Distance on tuples
///////////////////////////////////////////////////////////////

// Fold expression C++17
template <typename... T>
auto fold_sum(T... s)
{
	return (... + s);
}

// Use index_sequence and template parameter unpack to get each element in tuple
// Calculate sqrt( sum( (tuple1[i] - tuple2[i])^2 for all i) ) )
template <typename Tuple1, typename Tuple2, std::size_t... index>
double euclidean_distance(Tuple1&& p1, Tuple2&& p2, std::index_sequence<index...>)
{
	return std::sqrt(fold_sum(std::pow((std::get<index>(p1) - std::get<index>(p2)),2) ...));
}

// Check if 2 tuples are the same size using std::tuple_size
// Make index_sequence using std::make_index_sequence
template <typename Tuple1, typename Tuple2>
double euclidean_distance(Tuple1&& p1, Tuple2&& p2)
{
	static_assert(std::tuple_size<Tuple1>::value == std::tuple_size<Tuple2>::value, "Tuples not same size");
	return euclidean_distance(p1, p2, std::make_index_sequence<std::tuple_size<Tuple2>::value>{});
}

///////////////////////////////////////////////////////////////
/// LAB 2: Compile time computation - get<T> for tuples
///////////////////////////////////////////////////////////////

// Type count
// Find number of occurence of type in type sequence

// Catch all case, will compile error if hit
template <typename ...>
struct typeCount;

// Specialize for end of list
template <typename T>
struct typeCount<T>
{
	static constexpr std::size_t value = 0;
};

// Peel off first type in list
// Check if the same type as T with std::is_same
template <typename T, typename Head, typename... Tail>
struct typeCount<T, Head, Tail...>
{
	static constexpr std::size_t value = (std::is_same<T, Head>::value ? 1 : 0) + typeCount<T, Tail...>::value;
};

// Type index
// Find first index of type in type sequence

// Catch all case, will compile error if hit
template <typename ...>
struct typeIndex;

// Specialized case for hit, quit recursion and return 0
template <typename T, typename... Tails>
struct typeIndex<T, T, Tails...>
{
	constexpr static std::size_t value = 0;
};

// Specialized case for not hit, add 1 to count, toss Head in type list and recurse
template <typename T, typename Head, typename... Tail>
struct typeIndex<T, Head, Tail...>
{
	constexpr static std::size_t value = 1 + typeIndex<T, Tail...>::value;
};

// Reimplement of std::get<T>, check if requested type appears only once in type list, if so return a reference to that element
template <typename T, typename... Ts>
T& myGet(std::tuple<Ts...>& tup)
{
	static_assert(typeCount<T, Ts...>::value == 1, "Type must appear once in tuple");
	return std::get<typeIndex<T, Ts...>::value>(tup);
}

///////////////////////////////////////////////////////////////
/// LAB 3: Type traits detection  - Detect operator==
///////////////////////////////////////////////////////////////

// This uses SFINAE to test if 2 types are equatable
// decltype(*expression list*) will return the last type of the list, which is true in this case
// if declval<T1>() == declval<T2>() is a valid expression, this function will compile and function will return the type of the last expression (true), which is bool
template <typename T1, typename T2>
constexpr auto equatable(int) -> decltype(std::declval<T1>() == std::declval<T2>(), true)
{
	return true;
}

// Catch all case, return false.
// This will be selected if the function above isn't included in compilation, when declval<T1>() == declval<T2>() is NOT a valid expression
template <typename T1, typename T2>
constexpr bool equatable(...)
{
	return false;
}

// Test classes
class equatableClass;
class equatableClass2;

class equatableClass
{
public:
	int member;
	bool operator==(const equatableClass2& other);
};

class equatableClass2
{
public:
	int member;
	bool operator==(const equatableClass& other);
};

bool equatableClass::operator==(const equatableClass2& other)
{
	return member == other.member;
}

bool equatableClass2::operator==(const equatableClass& other)
{
	return member == other.member;
}

class nonEquatableClass
{
public:
	int member;
};

int main()
{
	/// LAB 1
	auto distance = euclidean_distance(std::make_tuple(1, 2), std::make_tuple(4, 6));
	std::cout << distance;
	/// END LAB 1


	/// LAB 2
	auto tup = std::make_tuple(4.0, "Evelyn", 52);
	myGet<int>(tup) = 3;
	std::cout << myGet<int>(tup);
	/// END LAB 2

	/// LAB 3
	static_assert(equatable<equatableClass, equatableClass2>(0), "Classes are not equatable");
	//static_assert(equatable<equatableClass, nonEquatableClass>(0), "Classes are not equatable"); // Should fail to compile
	/// END LAB 3

	return 0;
}
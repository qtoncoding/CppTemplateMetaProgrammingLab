#include <tuple>
#include <iostream>
#include <numeric>
#include <utility>

///////////////////////////////////////////////////////////////
/// LAB 1: Variadic templates - Euclidean Distance on tuples
///////////////////////////////////////////////////////////////

template <typename... T>
auto fold_sum(T... s)
{
	return (... + s);
}

template <typename Tuple1, typename Tuple2, std::size_t... index>
double euclidean_distance(Tuple1&& p1, Tuple2&& p2, std::index_sequence<index...>)
{
	return std::sqrt(fold_sum(std::pow((std::get<index>(p1) - std::get<index>(p2)),2) ...));
}

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
template <typename ...>
struct typeCount;

template <typename T>
struct typeCount<T>
{
	static constexpr std::size_t value = 0;
};

template <typename T, typename Head, typename... Tail>
struct typeCount<T, Head, Tail...>
{
	static constexpr std::size_t value = (std::is_same<T, Head>::value ? 1 : 0) + typeCount<T, Tail...>::value;
};

// Type index
// Find first index of type in type sequence
template <typename ...>
struct typeIndex;

template <typename T, typename... Tails>
struct typeIndex<T, T, Tails...>
{
	constexpr static std::size_t value = 0;
};

template <typename T, typename Head, typename... Tail>
struct typeIndex<T, Head, Tail...>
{
	constexpr static std::size_t value = 1 + typeIndex<T, Tail...>::value;
};

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
constexpr auto hasEquatableOperator(int) -> decltype(std::declval<T1>() == std::declval<T2>(), true)
{
	return true;
}

// Catch all case, return false.
// This will be selected if the function above isn't included in compilation, when declval<T1>() == declval<T2>() is NOT a valid expression
template <typename T1, typename T2>
constexpr bool hasEquatableOperator(...)
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
	static_assert(hasEquatableOperator<equatableClass, equatableClass2>(0), "Classes must have operator==");
	//static_assert(hasEquatableOperator<equatableClass, nonEquatableClass>(0), "Classes must have operator=="); // Should fail to compile
	/// END LAB 3

	return 0;
}
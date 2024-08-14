#include <map>
#include <iostream>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#define print(x) std::cout << x << std::endl
template<typename K, typename V>
class interval_map {
	friend void IntervalMapTest(interval_map<K, V>& map);
	V m_valBegin;
	std::map<K, V> m_map;
public:
	// constructor associates whole range of K with val
	interval_map(V const& val)
		: m_valBegin(val)
	{}

	// Assign value val to interval [keyBegin, keyEnd).
	// Overwrite previous values in this interval.
	// Conforming to the C++ Standard Library conventions, the interval
	// includes keyBegin, but excludes keyEnd.
	// If !( keyBegin < keyEnd ), this designates an empty interval,
	// and assign must do nothing.
	void assign(K const& keyBegin, K const& keyEnd, V const& val) {
		if (!(keyBegin < keyEnd)) {
			return;
		}

		if (m_map.empty()) {
			if (val == m_valBegin) return;
			m_map.insert_or_assign(keyEnd, m_valBegin);
			m_map.insert_or_assign(keyBegin, val);
			return;
		}

		auto itBegin = m_map.lower_bound(keyBegin);

		if (itBegin == m_map.end()) {
			if ((--itBegin)->second == val) return;
			m_map.insert_or_assign(keyEnd, itBegin->second);
			m_map.insert_or_assign(keyBegin, val);
			return;
		}

		V* prevVal = &m_valBegin;
		if (itBegin != m_map.begin()) {
			auto itTmp = itBegin;
			prevVal = &((--itTmp)->second);
		}

		bool writeBegin = 0;
		if (!(*prevVal == val)) {
			writeBegin = 1; // call `m_map.insert_or_assign(keyBegin, val);` later
		}

		bool wipePrev = 0;
		auto itEnd = m_map.upper_bound(keyEnd);
		while (1) {
			if (itBegin == itEnd) {
				if (!(*prevVal == val)) {
					auto [it, inserted] = m_map.emplace(keyEnd, *prevVal);
					if (wipePrev && inserted && it != m_map.begin()) {
						m_map.erase(--it);
					}
					if (writeBegin) {
						m_map.insert_or_assign(keyBegin, val);
					}
					return;
				}
				if (wipePrev) {
					m_map.erase(--itBegin);
				}
				if (writeBegin) {
					m_map.insert_or_assign(keyBegin, val);
				}
				return;
			}
			else {
				prevVal = &(itBegin->second);
				if (wipePrev && itBegin != m_map.begin()) {
					itBegin = m_map.erase(--itBegin);
				}
				wipePrev = 1;
				itBegin++;
			}
		}
	}

	// look-up of the value associated with key
	V const& operator[](K const& key) const {
		auto it = m_map.upper_bound(key);
		if (it == m_map.begin()) {
			return m_valBegin;
		}
		else {
			return (--it)->second;
		}
	}
};


struct StructKey {
	int value;

	StructKey(int val) : value(val) {}

	bool operator<(const StructKey& other) const {
		return value < other.value;
	}
};


struct StructValue {
	int value;

	StructValue(int val) : value(val) {}

	bool operator==(const StructValue& other) const {
		return value == other.value;
	}
};

std::ostream& operator<<(std::ostream& stream, const StructKey& other) {
	stream << other.value;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const StructValue& other) {
	stream << other.value;
	return stream;
}

void IntervalMapTest(interval_map<StructKey, StructValue>& map) {
	print("Internal map state: ");
	for (const auto& pair : map.m_map) {
		std::cout << pair.first << ": " << pair.second << std::endl;
	}
	if (map.m_map.empty()) {
		print("empty map!");
	}
}


void randomTest(int testCount) {
	srand((unsigned int)time(nullptr)); // Seed the random number generator

	int valInt = rand() % 50; // Generate values 'A' to 'Z' and 'a' to 'z'
	StructValue val = valInt; // Generate values 'A' to 'Z' and 'a' to 'z'
	interval_map<StructKey, StructValue> map = val;
	std::cout << "Initialized map with default value: " << valInt << std::endl;

	for (int i = 0; i < testCount; ++i) {
		StructKey keyBegin = rand() % 20 - 5; // Generate keys in the range [-5, 15)
		StructKey keyEnd = rand() % 20 + 15; // Generate keyEnd in the range [15, 35)
		valInt = rand() % 50; // Generate keyEnd in the range [0, 50)
		val = StructValue(valInt);
		std::cout << "Assigning interval [" << keyBegin << ", " << keyEnd << ") with value '" << val << "'" << std::endl;
		print(val);
		map.assign(keyBegin, keyEnd, val);
		IntervalMapTest(map);
	}
}


int main() {
	//map.assign(1, 5, 'B');
	//print("map.assign(1, 5, 'B');");
	//IntervalMapTest(map);

	randomTest(100);

	std::cin.get();
}

// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a test function that tests the functionality of
// the interval_map, for example using a map of int intervals to char.
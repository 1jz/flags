# Flags

Go inspired c++ header-only flag parsing library

## Demo 1

```cpp
#include <iostream>
#include "flags.h"

intptr_t value1;
Flag::FlagSet flags;

int main(int argc, char** argv) {

	flags = Flag::FlagSet(argc, argv);

	flags.Int(&value1, "i", "takes an int", 0);
	flags.Parse();

	Flag::Flag *f;

	if ((f = flags.getFlag("i")) && !f->parsed) {
		std::cout << f->command << ": please specify an integer" << std::endl;
	}

	std::cout << flags.getFlagValue<int>(f) << std::endl;
}
```

## Output

```
$ ./a.out -i 472
472
```

```
$ ./a.out -i
i: please specify an integer
0
```


## Demo 2

```cpp
#include <iostream>
#include "flags.h"

std::ostream& operator<<(std::ostream& os, std::vector<std::string> vec) {
	for (auto s : vec) {
		os << "[" << s << "] ";
	}
	return os;
}

intptr_t value1;
double value2;
std::string value3;
std::vector<std::string> value4;
bool value5;
bool value6;
bool value7;

Flag::FlagSet flags;

int main(int argc, char** argv) {

	flags = Flag::FlagSet(argc, argv);

	flags.Int(&value1, "i", "takes an int", 0);
	flags.Float64(&value2, "f", "takes an double", 0);
	flags.String(&value3, "s", "takes an string", "default");
	flags.StringList(&value4, "l", "takes multiple strings", { "one", "two" });
	flags.Bool(&value5, "b", "turns on value5");
	flags.Bool(&value6, "e", "turns on value6");
	flags.Bool(&value7, "x", "turns on value7");

	std::cout << value1 << std::endl;
	std::cout << value2 << std::endl;
	std::cout << value3 << std::endl;
	std::cout << value4 << std::endl;
	std::cout << std::boolalpha << value5 << std::endl;
	std::cout << std::boolalpha << value6 << std::endl;
	std::cout << std::boolalpha << value7 << std::endl;

	flags.Parse(); // pass true to throw errors 
	std::cout << std::endl << "args: ";
	for (int i = 1; i < argc; i++) { std::cout << argv[i] << " "; }
	std::cout << std::endl << std::endl;

	std::cout << value1 << std::endl;
	std::cout << value2 << std::endl;
	std::cout << value3 << std::endl;
	std::cout << value4 << std::endl;
	std::cout << std::boolalpha << value5 << std::endl;
	std::cout << std::boolalpha << value6 << std::endl;
	std::cout << std::boolalpha << value7 << std::endl;
}
```

## Output
```
$ ./a.out -i -i 35 -f -234.32 -s "test str" -l str1 str2 str3 -bx
0
0
default
[one] [two]
false
false
false

args: -i -i 35 -f -234.32 -s test str -l str1 str2 str3 -bx

35
-234.32
test str
[str1] [str2] [str3]
true
false
true
```
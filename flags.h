#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cstring>

namespace Flag {

	enum class Flag_type {
		INT_FLAG,
		FLOAT64_FLAG,
		STRING_FLAG,
		STRINGLIST_FLAG,
		BOOL_FLAG
	};

	struct Flag {
		std::string command;
		std::string desc;
		Flag_type type;
		bool parsed;
		void* value;
		intptr_t index;
	};

	class FlagSet {
		int argc;
		char** argv;
		std::unordered_map<std::string, Flag> flags;
		std::unordered_map<std::string, std::string> aliases;

	public:
		FlagSet() { argc = 1; argv = nullptr; };
		FlagSet(int argc, char** argv) {
			this->argc = argc;
			this->argv = argv;
		}

		void Int(intptr_t*value, std::string opt, std::string desc, intptr_t def) {
			*value = def;
			if (flags.find(opt) == flags.end()) {
				Flag f {
					opt,
					desc,
					Flag_type::INT_FLAG,
					false,
					value,
					-1
				};
				flags[opt] = f;
			}
		}

		void Float64(double* value, std::string opt, std::string desc, double def) {
			*value = def;
			if (flags.find(opt) == flags.end()) {
				Flag f {
					opt,
					desc,
					Flag_type::FLOAT64_FLAG,
					false,
					value,
					-1
				};
				flags[opt] = f;
			}
		}

		void String(std::string* value, std::string opt, std::string desc, std::string def) {
			*value = def;
			if (flags.find(opt) == flags.end()) {
				Flag f {
					opt,
					desc,
					Flag_type::STRING_FLAG,
					false,
					value,
					-1
				};
				flags[opt] = f;
			}
		}

		void StringList(std::vector<std::string>* value, std::string opt, std::string desc, std::vector<std::string> def) {
			*value = def;
			if (flags.find(opt) == flags.end()) {
				Flag f{
					opt,
					desc,
					Flag_type::STRINGLIST_FLAG,
					false,
					value,
					-1
				};
				flags[opt] = f;
			}
		}

		void Bool(bool* value, std::string opt, std::string desc, bool def = false) {
			*value = def;
			if (flags.find(opt) == flags.end()) {
				Flag f {
					opt,
					desc,
					Flag_type::BOOL_FLAG,
					false,
					value,
					-1
				};
				flags[opt] = f;
			}
		}

		Flag* getFlag(std::string opt) {
			if (flags.find(opt) != flags.end())
				return &flags[opt];
			return nullptr;
		}


		template <typename T>
		T getFlagValue(Flag* f) {
			if (f)
				return *((T*)(f->value));
			return T{};
		}

		template <typename T>
		T getFlagValue(std::string opt) {
			if (flags.find(opt) != flags.end())
				return *((T*)(flags[opt].value));
			return T{};
		}

		void Alias(std::string sOpt, std::string lOpt) {
			if (flags.find(sOpt) != flags.end()) {
				aliases[lOpt] = sOpt;
			}
		}

		void Parse(bool debug = false) {
			std::vector<std::string> errors;
			for (int i = 1; i < argc; i++) {
				if (strlen(argv[i]) > 2 && argv[i][0] == '-' && argv[i][1] == '-') {
					std::string flag(argv[i] + 2);
					if (flags.find(flag) != flags.end()) {
						Flag& f = flags[flag];
						f.index = i;
						parseFlag(f);
					} else if (aliases.find(flag) != aliases.end()) {
						std::string opt = aliases[flag];
						Flag& f = flags[opt];
						f.index = i;
						parseFlag(f);
					}
					else {
						errors.push_back(std::string(argv[i]) + " is not an argument.");
					}
				}
				else if (strlen(argv[i]) == 2 && argv[i][0] == '-') {
					std::string flag(argv[i] + 1);
					if (flags.find(flag) != flags.end()) {
						Flag& f = flags[flag];
						f.index = i;
						parseFlag(f);
					}
					else {
						errors.push_back(std::string(argv[i]) + " is not an argument.");
					}
				}
				else if (strlen(argv[i]) > 1 && argv[i][0] == '-') {
					std::vector<Flag*> booleans;
					for (auto j = 1; argv[i][j]; j++) {
						std::string flag({ argv[i][j] });
						if (flags.find(flag) != flags.end()) {
							Flag& f = flags[flag];
							if (f.type == Flag_type::BOOL_FLAG) {
								*((bool*)f.value) = true;
								f.index = i;
								f.parsed = true;
							}
							else {
								errors.push_back(std::string({ argv[i][j] }) + " is not a boolean.");
							}
						}
						else {
							errors.push_back(std::string({ argv[i][j] }) + " is not an option.");
						}
					}
				}
			}

			if (debug && !errors.empty()) {
				throw errors;
			}
		}

	private:
		void parseFlag(Flag &f) {
			if (checkArgs(f)) {
				switch (f.type) {
				case Flag_type::INT_FLAG: {
					std::stringstream ss(argv[f.index + 1]);
					ss >> *((intptr_t*)f.value);
					f.parsed = true;
					break;
				}

				case Flag_type::FLOAT64_FLAG: {
					std::stringstream ss(argv[f.index + 1]);
					ss >> *((double*)f.value);
					f.parsed = true;
					break;
				}

				case Flag_type::STRING_FLAG: {
					((std::string*)f.value)->assign(argv[f.index + 1]);
					f.parsed = true;
					break;
				}

				case Flag_type::STRINGLIST_FLAG: {
					std::vector<std::string>* list = ((std::vector<std::string>*)f.value);
					list->clear();
					for (auto i = f.index + 1; i < argc && argv[i][0] != '-'; i++) {
						list->push_back(argv[i]);
					}
					f.parsed = true;
					break;
				}

				case Flag_type::BOOL_FLAG: {
					*((bool*)f.value) = true;
					f.parsed = true;
					break;
				}

				default:
					break;
				}
			}
		}

		bool checkArgs(Flag &f) {
			bool valid = true;
			if (f.type != Flag_type::BOOL_FLAG) {
				if (f.index >= argc || !argv[f.index + 1]) {
					valid = false;
				}
				else if (strlen(argv[f.index + 1]) > 2 && argv[f.index + 1][0] == '-' && argv[f.index + 1][1] == '-') {
					std::string alias(argv[f.index + 1] + 2);
					if (aliases.find(alias) != aliases.end()) {
						valid = false;
					}
				}
				else if (strlen(argv[f.index + 1]) > 1 && argv[f.index + 1][0] == '-') {
					std::string flag("" + argv[f.index + 1][1]);
					if (flags.find(flag) != flags.end()) {
						valid = false;
					}
				}
			}
			return valid;
		}
	};
}

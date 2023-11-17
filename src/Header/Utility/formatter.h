#pragma once
//https://github.com/Yinpinghua/format
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace formatter
{
	class arg_base
	{
	public:
		arg_base() = default;
		virtual ~arg_base() = default;
		virtual void format(std::ostringstream& ss) = 0;
	};

	template <class T>
	class arg : public arg_base
	{
	public:
		arg(const T& arg) : arg_(arg) {}
		virtual ~arg() = default;
		virtual void format(std::ostringstream& ss) {
			ss << arg_;
		}
	private:
		T arg_;
	};

	class arg_array : public std::vector <std::shared_ptr<arg_base>>
	{
	public:
		arg_array() = default;
		~arg_array() {
			clear();
		}
	};

	static void format_item(std::ostringstream& ss, const std::string& item, const arg_array& args) {
		size_t index = strtol(&item[0], nullptr, 10);
		if (index < 0 || index >= args.size()) {
			return;
		}

		args[index]->format(ss);
	}

	template <class T>
	static void transfer(arg_array& argArray, T t) {
		argArray.push_back(std::make_shared<arg<T>>(t));
	}

	template<typename... Args>
	static void transfer(arg_array& argArray, Args&&... args) {
		(transfer(argArray, args), ...);
	}


	template <typename... Args>
	std::string format(const std::string& format_str, Args&&... args) {
		std::string str;
		if (sizeof...(args) == 0) {
			return format_str;
		}

		arg_array argArray;
		transfer(argArray, args...);
		size_t start = 0;
		size_t pos = 0;
		std::ostringstream ss;
		while (true) {
			pos = format_str.find('{', start);
			if (pos == std::string::npos) {
				ss << format_str.substr(start);
				break;
			}

			ss << format_str.substr(start, pos - start);
			if (format_str[pos + 1] == '{') {
				ss << '{';
				start = pos + 2;
				continue;
			}

			start = pos + 1;
			pos = format_str.find('}', start);
			if (pos == std::string::npos) {
				ss << format_str.substr(start - 1);
				break;
			}

			format_item(ss, format_str.substr(start, pos - start), argArray);
			start = pos + 1;
		}

		str = ss.str();
		return std::move(str);
	}
}
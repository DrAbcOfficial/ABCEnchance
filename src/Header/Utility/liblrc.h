#pragma once
#include <string>
#include <vector>

namespace lrc {
	using lrcattribute_t = struct lrcattribute_s
	{
		std::u8string Key = u8"";
		std::u8string Value = u8"";
	};
	class CLrcLine
	{
	public:
		std::u8string CurrentLyric = u8"";
		time_t StartTime = 0;
		time_t EndTime = 0;
		const char* GetLyric();
	};
	class CLrcCollection {
	public:
		std::vector<CLrcLine*> Lines = {};
		std::vector<lrcattribute_t*> Attributes = {};
		size_t Size();
		CLrcLine* LyricAt(time_t time);
		~CLrcCollection();
	};
	extern CLrcCollection* LrcParser(const char8_t* lrc);
}
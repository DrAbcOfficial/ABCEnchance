#include "liblrc.h"

#include <algorithm>
#include <sstream>

namespace lrc{
CLrcCollection* LrcParser(const char8_t* lrc) {
	 std::vector<std::u8string> splited;
	 std::u8string s = lrc;
	 std::u8string token;
	 std::basic_istringstream<char8_t> tokenStream(s);
	 while (std::getline(tokenStream, token, u8'\n')) {
		 splited.push_back(token);
	 }

	 CLrcCollection* collection = new CLrcCollection();
	 for (auto iter = splited.begin(); iter != splited.end(); iter++) {
		 std::u8string line = *iter;
		 if (*line.begin() != u8'[')
			 continue;
		 size_t tagend = line.rfind(u8']');
		 //wtf???
		 if (tagend == std::u8string::npos)
			 continue;
		 //Is Attribute
		 if (tagend == line.size() - 1) {
			 bool invalue = false;
			 lrcattribute_t* attribute = new lrcattribute_t();
			 for (size_t i = 1; i < line.size() - 1; i++) {
				 char8_t c = line[i];
				 if (c == u8':') {
					 invalue = true;
					 continue;
				 }
				 if (invalue)
					 attribute->Value += c;
				 else
					 attribute->Key += c;
			 }
			 collection->Attributes.push_back(attribute);
		 }
		 //Is Lyric
		 else {
			 std::u8string header = line.substr(0, tagend + 1);
			 std::u8string lrc = line.substr(tagend + 1);

			 std::u8string timestamp = u8"";
			 CLrcLine* lineitem = nullptr;
			 for (size_t i = 0; i < header.size(); i++) {
				 char8_t c = header[i];
				 switch (c)
				 {
				 case u8'[': {
					 lineitem = new CLrcLine();
					 break;
				 }
				 case u8']': {
					 if (lineitem) {
						 time_t m, s, ms;
						 char dem;
						 std::istringstream ss(reinterpret_cast<const char*>(timestamp.c_str()));
						 ss >> m >> dem >> s >> dem >> ms;
						 lineitem->StartTime = m * 60000 + s * 1000 + ms;
						 lineitem->CurrentLyric = lrc;
						 collection->Lines.push_back(lineitem);
						 lineitem = nullptr;
					 }
					 break;
				 }
				 default:
					 timestamp += c;
					 break;
				 }
			 }
			 //wtf line
			 if (lineitem)
				 delete lineitem;
		 }
	 }
	 std::sort(collection->Lines.begin(), collection->Lines.end(), [](const CLrcLine* a, const CLrcLine* b) {
		 return a->StartTime < b->StartTime;
	 });
	 if (collection->Lines.size() > 1) {
		 for (size_t i = 0; i < collection->Lines.size() - 1; ++i) {
			 collection->Lines[i]->EndTime = collection->Lines[i + 1]->StartTime;
		 }
	 }
	 return collection;
}

const char* CLrcLine::GetLyric() const{
	return reinterpret_cast<const char*>(CurrentLyric.c_str());
}

size_t CLrcCollection::Size() const{
	return Lines.size();
}

CLrcLine* CLrcCollection::LyricAt(time_t time){
	for (auto iter = Lines.rbegin(); iter != Lines.rend(); iter++) {
		auto line = (*iter);
		if (line->StartTime <= time)
			return line;
	}
	return nullptr;
}

CLrcCollection::~CLrcCollection() {
	for (auto iter = Lines.begin(); iter != Lines.end(); iter++) {
		delete (*iter);
	}
	for (auto iter = Attributes.begin(); iter != Attributes.end(); iter++) {
		delete (*iter);
	}
	Lines.clear();
	Attributes.clear();
}
}
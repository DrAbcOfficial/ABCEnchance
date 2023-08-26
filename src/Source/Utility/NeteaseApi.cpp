#include "NeteaseApi.h"
#include "crypto.h"
#include "curl.h"

#include <vector>
#include <map>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace netease {
	string icookie = "/tmp/neteaseapicookie", ocookie = "/tmp/neteaseapicookie";

	const std::map<char*, char*> header = {
			{"Accept", "*/*"},
			{"Accept-Language", "zh-CN,zh;q=0.8,gl;q=0.6,zh-TW;q=0.4"},
			{"Connection", "keep-alive"},
			{"Content-Type", "application/x-www-form-urlencoded"},
			{"Referer", "http://music.163.com"},
			{"Host", "music.163.com"},
			{"Cookie", "appver=2.7.1.198277; os=pc; "},
			{"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.45 Safari/537.36"}
		};

	size_t appendData(void* ptr, size_t size, size_t nmemb, void* user) {
		std::vector<char>* p = (std::vector<char>*)user;
		auto cs = p->size();
		p->resize(cs + size * nmemb);
		memcpy(p->data() + cs, ptr, size * nmemb);
		return size * nmemb;
	}

	string post(const Action& action) {
		std::vector<char> retdata;
		void* curl = curl_easy_init();
		curl_slist* header = 0;
		for (auto iter = netease::header.begin(); iter != netease::header.end(); iter++){
			header = curl_slist_append(header, (string(iter->first) + ": " + string(iter->second)).c_str());
		}
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_URL, action.url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, appendData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retdata);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, action.post.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, action.post.length());
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, icookie.c_str());
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, ocookie.c_str());
		auto result = curl_easy_perform(curl);
		curl_slist_free_all(header);
		curl_easy_cleanup(curl);
		retdata.push_back(0);
		return retdata.data();
	}

	inline string tostr(long long i) {
		char temp[22];
		sprintf(temp, "%I64d", i);
		return temp;
	}

	string tostr(const rapidjson::Document& doc) {
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		doc.Accept(writer);
		return sb.GetString();
	}

	inline void add(rapidjson::Document& doc, const char* key, long long i) {
		doc.AddMember(rapidjson::Value(key, doc.GetAllocator()), rapidjson::Value((int64_t)i), doc.GetAllocator());
	}

	inline void add(rapidjson::Document& doc, const char* key, int i) {
		doc.AddMember(rapidjson::Value(key, doc.GetAllocator()), rapidjson::Value(i), doc.GetAllocator());
	}

	inline void add(rapidjson::Document& doc, const char* key, const char* s) {
		doc.AddMember(rapidjson::Value(key, doc.GetAllocator()), rapidjson::Value(s, doc.GetAllocator()), doc.GetAllocator());
	}

	inline void add(rapidjson::Document& doc, const char* key, bool b) {
		doc.AddMember(rapidjson::Value(key, doc.GetAllocator()), rapidjson::Value(b), doc.GetAllocator());
	}

	inline void add(rapidjson::Document& doc, const char* key, const string& s) {
		add(doc, key, s.c_str());
	}

	template <typename Type>
	inline void build(rapidjson::Document& doc, const char* key, const Type& value) {
		add(doc, key, value);
	}

	template <typename Type, typename... Arg>
	inline void build(rapidjson::Document& doc, const char* key, const Type& value, const Arg&... arg) {
		add(doc, key, value);
		build(doc, arg...);
	}

	static string build_post(rapidjson::Document& doc) {
		string encText, encKey;
		Encrypt(tostr(doc), encText, encKey);
		return "params=" + UrlEncode(encText) + "&encSecKey=" + UrlEncode(encKey);
	}

	Action album(long long id) {
		rapidjson::Document data;
		data.SetObject();
		return Action("/v1/album/" + tostr(id), build_post(data));
	}

	Action artist(long long id) {
		rapidjson::Document data;
		data.SetObject();
		return Action("/v1/artist/" + tostr(id), build_post(data));
	}

	Action artist_albums(long long id, int limit, int offset) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "offset", offset, "limit", limit, "total", true);
		return Action("/artist/albums/" + tostr(id), build_post(data));
	}

	Action artist_introduction(long long id) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "id", id);
		return Action("/artist/introduction", build_post(data));
	}

	Action login_cellphone(long long phone, const string& pswd) {
		rapidjson::Document data;
		data.SetObject();
		char buffer[22];
		sprintf(buffer, "%lld", phone);
		build(data, "phone", buffer, "password", Md5Encode(pswd), "rememberLogin", "true");
		return Action("/login/cellphone", build_post(data));
	}

	Action music_url(long long id, int bitrate) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "ids", "[" + tostr(id) + "]", "br", bitrate);
		return Action("/song/enhance/player/url", build_post(data));
	}

	Action personal_fm() {
		rapidjson::Document data;
		data.SetObject();
		return Action("/v1/radio/get", build_post(data));
	}

	Action playlist_detail(long long id, int recent_collect) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "id", id, "n", 100000, "s", recent_collect);
		return Action("/v3/playlist/detail", build_post(data));
	}

	Action dailytask(DailyTaskType type) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "type", type);
		return Action("/point/dailyTask", build_post(data));
	}

	std::vector<std::shared_ptr<song_t>> search(const string& keyword, SearchType type, int limit, int offset) {
		string json = post(Action("/cloudsearch/pc", "type=" + std::to_string(type) + "&limit=" + std::to_string(limit) + "&offset=" + std::to_string(offset) + "&s=\"" + keyword + "\""));
		rapidjson::Document data;
		data.Parse(json.c_str());
		static std::vector<std::shared_ptr<song_t>> s_songs;
		s_songs.clear();
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			auto arr = data["result"]["songs"].GetArray();
			for (auto iter = arr.Begin(); iter != arr.End(); iter++) {
				std::shared_ptr<song_t> song = std::make_shared<song_t>();
				song->name = (*iter)["name"].GetString();
				song->id = (*iter)["id"].GetInt();
				auto ars = (*iter)["ar"].GetArray();
				for (auto iter2 = ars.Begin(); iter2 != ars.End(); iter2++) {
					std::shared_ptr<singer_t> singer = std::make_shared<singer_t>();
					singer->name = (*iter2)["name"].GetString();
					singer->id = (*iter2)["id"].GetInt();
					song->ar.push_back(singer);
				}
				song->al.name = (*iter)["al"]["name"].GetString();
				song->al.id = (*iter)["al"]["id"].GetInt();
				song->al.picUrl = (*iter)["al"]["picUrl"].GetString();
				s_songs.push_back(song);
			}
		}
		return s_songs;
	}

	std::shared_ptr<song_t> song_detail(long long id) {
		rapidjson::Document data;
		string json = post(Action("/v3/song/detail", "c=[{\"id\": " + std::to_string(id) + "}]"));
		data.Parse(json.c_str());
		std::shared_ptr<song_t> song = std::make_shared<song_t>();
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			auto arr = data["songs"].GetArray().Begin();
			song->name = (*arr)["name"].GetString();
			song->id = (*arr)["id"].GetInt();
			auto ars = (*arr)["ar"].GetArray();
			for (auto iter2 = ars.Begin(); iter2 != ars.End(); iter2++) {
				std::shared_ptr<singer_t> singer = std::make_shared<singer_t>();
				singer->name = (*iter2)["name"].GetString();
				singer->id = (*iter2)["id"].GetInt();
				song->ar.push_back(singer);
			}
			song->al.name = (*arr)["al"]["name"].GetString();
			song->al.id = (*arr)["al"]["id"].GetInt();
			song->al.picUrl = (*arr)["al"]["picUrl"].GetString();
		}
		return song;
	}

	Action user_detail(long long id) {
		rapidjson::Document data;
		data.SetObject();
		return Action("v1/user/detail/" + tostr(id), build_post(data));
	}

	Action user_playlist(long long id, int limit, int offset) {
		rapidjson::Document data;
		data.SetObject();
		build(data, "limit", limit, "offset", offset, "uid", id);
		return Action("/user/playlist", build_post(data));
	}
}
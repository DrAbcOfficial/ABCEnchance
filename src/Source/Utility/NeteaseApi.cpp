#include "NeteaseApi.h"
#include "crypto.h"
#include "curl.h"
#include "qrcodegen.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace netease {
	const std::map<char*, char*> header = {
		{"Accept", "*/*"},
		{"Accept-Language", "zh-CN,zh;q=0.8,gl;q=0.6,zh-TW;q=0.4"},
		{"Connection", "keep-alive"},
		{"Content-Type", "application/x-www-form-urlencoded"},
		{"Referer", "http://music.163.com"},
		{"Host", "music.163.com"},
		{"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.45 Safari/537.36"}
	};
	string buildparam(const std::map<string, string>& map) {
		string paramStr;
		for (auto iter = map.begin(); iter != map.end(); iter++) {
			paramStr += (*iter).first + "=" + (*iter).second;
			paramStr += "&";
		}
		paramStr.pop_back();
		return paramStr;
	}
	curl_slist* buildheader() {
		curl_slist* header = nullptr;
		for (auto iter = netease::header.begin(); iter != netease::header.end(); iter++) {
			string h = string(iter->first) + ": " + string(iter->second);
			header = curl_slist_append(header, h.c_str());
		}
		return header;
	}
	size_t append(void* ptr, size_t size, size_t nmemb, void* user) {
		std::vector<char>* p = (std::vector<char>*)user;
		auto cs = p->size();
		p->resize(cs + size * nmemb);
		memcpy(p->data() + cs, ptr, size * nmemb);
		return size * nmemb;
	}
	string post(const Action& action) {
		string postStr = buildparam(action.post);
		std::vector<char> retdata;
		void* curl = curl_easy_init();
		curl_slist* header = buildheader();
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_URL, action.url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &append);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retdata);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postStr.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postStr.length());
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, CNeteaseMusicAPI::CokkieInPath().c_str());
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, CNeteaseMusicAPI::CookieOutPath().c_str());
		auto result = curl_easy_perform(curl);
		curl_slist_free_all(header);
		curl_easy_cleanup(curl);
		retdata.push_back(0);
		return retdata.data();
	}
	CBase163Object::CBase163Object(rapidjson::Value& json) {
		if (json.HasMember("name") && json.HasMember("id")) {
			name = json["name"].GetString();
			id = json["id"].GetUint64();
		}
	}
	CArtist::CArtist(rapidjson::Value& json) : CBase163Object(json) {

	}
	CAlbum::CAlbum(rapidjson::Value& json) : CBase163Object(json) {
		if (json.HasMember("picUrl"))
			picUrl = json["picUrl"].GetString();
	}
	CMusic::CMusic(rapidjson::Value& json) : CBase163Object(json) {
		if (json.HasMember("fee"))
			copyright = (json["fee"].GetInt() & FeeType::FEE_VIP) != 0;
		rapidjson::Value alias;
		if (json.HasMember("alia"))
			alias = json["alia"].GetArray();
		else 
			alias = json["alias"].GetArray();
		if (alias.Size() > 0)
			aliasName = alias.Begin()->GetString();
		else
			aliasName = "";

		rapidjson::Value ars;
		if (json.HasMember("ar"))
			ars = json["ar"].GetArray();
		else
			ars = json["artists"].GetArray();
		for (auto iter2 = ars.Begin(); iter2 != ars.End(); iter2++) {
			ar.push_back(std::make_shared<CArtist>(*iter2));
		}
		if (json.HasMember("al"))
			al = std::make_shared<CAlbum>(json["al"]);
		else
			al = std::make_shared<CAlbum>(json["album"]);

		if (json.HasMember("dt"))
			duration = json["dt"].GetUint64();
		else
			duration = json["duration"].GetUint64();
	}
	std::optional<string> CMusic::GetPlayUrl(const char* quality, char* encode) {
		std::map<string, string> p = {
			{"ids", "[" + std::to_string(id) + "]"},
			{"level", quality},
			{"encodeType", encode}
		};
		Action act = Action("", p);
		act.url = "https://interface3.music.163.com/api/song/enhance/player/url/v1";
		string json = post(act);
		rapidjson::Document data;
		data.Parse(json.c_str());
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			auto arr = data["data"].GetArray();
			if (arr.Size() > 0) {
				if(!arr[0]["url"].IsNull())
					return arr[0]["url"].GetString();
				return std::nullopt;
			}
		}
		return std::nullopt;
	}
	string CMusic::GetArtists() {
		string str = "";
		for(auto iter = ar.begin(); iter != ar.end();iter++){
			str += (*iter)->name;
			if (iter != ar.end() - 1)
				str += ", ";
		}
		return str;
	}
	CDjMusic::CDjMusic(rapidjson::Value& json) : CMusic(json["mainSong"]) {
		//override
		if (json.HasMember("name") && json.HasMember("id")) {
			name = json["name"].GetString();
			id = json["id"].GetUint64();
		}
		if (json.HasMember("mainTrackId"))
			mainTrackId = json["mainTrackId"].GetUint64();
		if (json.HasMember("coverUrl"))
			coverUrl = json["coverUrl"].GetString();
		if (json.HasMember("listenerCount"))
			listenerCount = json["listenerCount"].GetUint64();
		if (json.HasMember("likedCount"))
			likedCount = json["likedCount"].GetUint64();
		if (json.HasMember("description"))
			description = json["description"].GetString();
	}
	std::optional<string> CDjMusic::GetPlayUrl(const char* quality, char* encode) {
		std::map<string, string> p = {
			{"ids", "[" + std::to_string(mainTrackId) + "]"},
			{"level", quality},
			{"encodeType", encode}
		};
		Action act = Action("", p);
		act.url = "https://interface3.music.163.com/api/song/enhance/player/url/v1";
		string json = post(act);
		rapidjson::Document data;
		data.Parse(json.c_str());
		if (data.HasMember("data") && data.HasMember("code") && data["code"].GetInt() == 200) {
			auto music = data["data"].GetArray().Begin();
			return (!(*music)["url"].IsNull()) ? std::make_optional<std::string>((*music)["url"].GetString()) : std::nullopt;
		}
		return std::nullopt;
	}
	CLyric::CLyric(rapidjson::Document& json) {
		if (json.HasMember("code") && json["code"].GetInt() == 200) {
			const static auto shouldSkip = [](string& raw) -> bool {
				//fuck trim
				raw.erase(raw.begin(), std::find_if(raw.begin(), raw.end(), [](unsigned char ch) {
					return ch != '\n' && !std::isspace(ch);
				}));
				raw.erase(std::find_if(raw.rbegin(), raw.rend(), [](unsigned char ch) {
					return ch != '\n' && !std::isspace(ch);
				}).base(), raw.end());
				return std::isdigit(raw[1]) == 0;
			};
			if (json.HasMember("lrc"))
				lyric = json["lrc"]["lyric"].GetString();
			if (json.HasMember("tlyric"))
				tlyric = json["tlyric"]["lyric"].GetString();
		}
	}
	CUser::CUser(rapidjson::Value& json) : CBase163Object(json) {
		if (json.HasMember("profile")) {
			rapidjson::Value profile = json["profile"].GetObject();
			if (profile.HasMember("userId"))
				id = profile["userId"].GetUint64();
			if (profile.HasMember("nickname"))
				name = profile["nickname"].GetString();
			if (profile.HasMember("nickname"))
				signature = profile["signature"].GetString();
			if (profile.HasMember("vipType"))
				vip = profile["vipType"].GetUint();
			if (profile.HasMember("playlistCount"))
				playlistCount = profile["playlistCount"].GetUint();
		}
		if (json.HasMember("listenSongs"))
			listenSongs = json["listenSongs"].GetUint64();
		if (json.HasMember("level"))
			level = json["level"].GetUint();
		if (json.HasMember("createTime"))
			createTime = json["createTime"].GetUint64();
		if (json.HasMember("createDays"))
			createDay = json["createDays"].GetUint64();
	}
	CMy::CMy(rapidjson::Value& json) : CUser(json) {

	}
	std::vector<neteaseid_t> CMy::GetDailyRecommend() {
		std::map<string, string> p = {};
		string json = post(Action("/v3/discovery/recommend/songs", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		std::vector<neteaseid_t> ret;
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			auto arr = data["data"]["dailySongs"].GetArray();
			for (auto iter = arr.Begin(); iter != arr.End(); iter++) {
				ret.push_back((*iter)["id"].GetUint64());
			}
		}
		return ret;
	}
	std::vector<neteaseid_t> CMy::GetFM(){
		std::map<string, string> p = {};
		string json = post(Action("/v1/radio/get", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		std::vector<neteaseid_t> ret;
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			auto arr = data["data"].GetArray();
			for (auto iter = arr.Begin(); iter != arr.End(); iter++) {
				ret.push_back((*iter)["id"].GetUint64());
			}
		}
		return ret;
	}
	CPlayList::CPlayList(rapidjson::Value& json) : CBase163Object(json) {
		rapidjson::Value playlist;
		if (json.HasMember("playlist"))
			playlist = json["playlist"];
		else
			playlist = json;

		coverUrl = playlist["coverImgUrl"].GetString();
		count = playlist["trackCount"].GetUint();
		playCount = playlist["playCount"].GetUint();
		creator = playlist["creator"]["nickname"].GetString();

		if (!playlist["description"].IsNull())
			description = playlist["description"].GetString();
		else
			description = "";
		if (!playlist["trackIds"].IsNull()) {
			auto arr = playlist["trackIds"].GetArray();
			for (auto iter = arr.Begin(); iter != arr.End(); iter++) {
				auto id = (*iter)["id"].GetUint64();
				mucics.push_back(id);
			}
		}
	}

	std::vector<std::shared_ptr<CMusic>> CNeteaseMusicAPI::GetAlbumSongs(neteaseid_t id) {
		std::map<string, string> p = {};
		string json = post(Action("/v1/album/" + std::to_string(id), p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		std::vector<std::shared_ptr<CMusic>> songs;
		if (data.HasMember("code") && data["code"].GetInt() == 200 && 
			data.HasMember("resourceState") && data["resourceState"].GetBool()) {
			auto arr = data["songs"].GetArray();
			for (auto iter = arr.Begin(); iter != arr.End(); iter++) {
				songs.push_back(std::make_shared<CMusic>(*iter));
			}
		}
		return songs;
	}
	std::vector<std::shared_ptr<CDjMusic>> CNeteaseMusicAPI::GetRadioSongs(neteaseid_t id) {
		std::map<string, string> p = {
			{"radioId", std::to_string(id)},
			{"limit", "32"},
			{"offset", "0"},
			{"asc", "0"}
		};
		string json = post(Action("/dj/program/byradio", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		std::vector<std::shared_ptr<CDjMusic>> songs;
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			auto arr = data["programs"].GetArray();
			for (auto iter = arr.Begin(); iter != arr.End(); iter++) {
				songs.push_back(std::make_shared<CDjMusic>(*iter));
			}
			//fuck, has more!
			size_t counter = 1;
			while (data.HasMember("code") && data["code"].GetInt() == 200 && data.HasMember("more") && data["more"].GetBool()) {
				p = {
					{"radioId", std::to_string(id)},
					{"limit", std::to_string(data["count"].GetInt() - 32 * counter)},
					{"offset", std::to_string(counter * 32)},
					{"asc", "0"}
				};
				json = post(Action("/dj/program/byradio", p));
				data.Parse(json.c_str());
				arr = data["programs"].GetArray();
				for (auto iter = arr.Begin(); iter != arr.End(); iter++) {
					songs.push_back(std::make_shared<CDjMusic>(*iter));
				}
				counter++;
			}
		}
		return songs;
	}
	std::shared_ptr<CArtist> CNeteaseMusicAPI::GetArtist(neteaseid_t id) {
		std::map<string, string> p = {
			{"id", std::to_string(id)}
		};
		string json = post(Action("/artist/head/info/get", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		std::shared_ptr<CArtist> artist = nullptr;
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			artist = std::make_shared<CArtist>((data["data"]["artist"]));
		}
		return artist;
	}

	std::vector<std::shared_ptr<CSearchResult>> CNeteaseMusicAPI::Search(const string& keyword, SearchType type ,int limit, int offset) {
		std::map<string, string> p = {
			{"type", std::to_string(type)},
			{"limit", std::to_string(limit)},
			{"offset", std::to_string(offset)},
			{"s", "\"" + keyword + "\""}
		};
		string json = post(Action("/cloudsearch/pc", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		std::vector<std::shared_ptr<CSearchResult>> ret;
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			size_t count = 0;
			switch (type){
			case netease::ST_LYRIC:
			case netease::ST_SONG: count = data["result"]["songCount"].GetUint(); break;
			case netease::ST_ALBUM: count = data["result"]["albumCount"].GetUint(); break;
			case netease::ST_ARTIST: count = data["result"]["artistCount"].GetUint(); break;
			case netease::ST_PLAYLIST: count = data["result"]["playlistCount"].GetUint(); break;
			case netease::ST_USER: count = data["result"]["userprofileCount"].GetUint(); break;
			case netease::ST_RADIO: count = data["result"]["djRadiosCount"].GetUint(); break;
			case netease::ST_MV: count = data["result"]["mvCount"].GetUint(); break;
			case netease::ST_VIDEO: count = data["result"]["videoCount"].GetUint(); break;
			}
			if (count == 0)
				return ret;
			rapidjson::Value arr;
			switch (type){
			case netease::ST_LYRIC:
				case netease::ST_SONG: arr = data["result"]["songs"].GetArray(); break;
				case netease::ST_ALBUM: arr = data["result"]["albums"].GetArray(); break;
				case netease::ST_ARTIST: arr = data["result"]["artists"].GetArray(); break;
				case netease::ST_PLAYLIST: arr = data["result"]["playlists"].GetArray(); break;
				case netease::ST_USER: arr = data["result"]["userprofiles"].GetArray(); break;
				case netease::ST_RADIO: arr = data["result"]["djRadios"].GetArray(); break;
				case netease::ST_MV: count = data["result"]["mvs"].GetUint(); break;
				case netease::ST_VIDEO: count = data["result"]["videos"].GetUint(); break;
			}
			if (!arr.IsNull()) {
				for (auto iter = arr.Begin(); iter != arr.End(); iter++) {
					auto p = std::make_shared<CSearchResult>();
					if (type == ST_VIDEO) {
						p->id = (*iter)["vid"].GetUint64();
						p->name = (*iter)["title"].GetString();
					}
					else if (type == ST_USER) {
						p->id = (*iter)["userId"].GetUint64();
						p->name = (*iter)["nickname"].GetString();
					}
					else {
						p->id = (*iter)["id"].GetUint64();
						p->name = (*iter)["name"].GetString();
					}
					switch (type) {
					case netease::ST_LYRIC:
					case netease::ST_SONG: {
						p->extra = (*(*iter)["ar"].GetArray().Begin())["name"].GetString();
						p->extra += " ";
						p->extra += (*iter)["al"]["name"].GetString();
						break;
					}
					case netease::ST_ALBUM: {
						p->extra = (*iter)["artist"]["name"].GetString();
						break;
					}
					case netease::ST_PLAYLIST: {
						p->extra = (*iter)["creator"]["nickname"].GetString();
						break;
					}
					case netease::ST_RADIO: {
						p->extra = (*iter)["dj"]["nickname"].GetString();
						p->extra += " - ";
						p->extra += (*iter)["category"].GetString();
						break;
					}
					case netease::ST_MV: {
						p->extra = (*iter)["artistName"].GetString();
						break;
					}
					case netease::ST_VIDEO:
					case netease::ST_ARTIST:
					case netease::ST_USER:
					default: break;
					}
					if ((*iter).HasMember("fee"))
						p->copyright = ((*iter)["fee"].GetInt() & FeeType::FEE_VIP) != 0;
					ret.push_back(p);
				}
			}
		}
		return ret;
	}
	std::shared_ptr<CMusic> CNeteaseMusicAPI::GetSongDetail(neteaseid_t id) {
		std::map<string, string> p = {
			{"c", "[{\"id\": " + std::to_string(id) + "}]"}
		};
		string json = post(Action("/v3/song/detail", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		std::shared_ptr<CMusic> song = nullptr;
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			auto arr = data["songs"].GetArray();
			if(arr.Size() > 0)
				song = std::make_shared<CMusic>(*(arr.Begin()));
		}	
		return song;
	}
	std::shared_ptr<CDjMusic> CNeteaseMusicAPI::GetDjSongDetail(neteaseid_t id) {
		std::map<string, string> p = {
			{"id", std::to_string(id)}
		};
		string json = post(Action("/dj/program/detail", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		std::shared_ptr<CDjMusic> dj = nullptr;
		if (data.HasMember("code") && data["code"].GetInt() == 200) {
			if (data.HasMember("program"))
				dj = std::make_shared<CDjMusic>(data["program"]);
		}
		return dj;
	}
	std::shared_ptr<CLyric> CNeteaseMusicAPI::GetLyric(neteaseid_t songid){
		std::map<string, string> p = {
			{"id", std::to_string(songid)},
			{"lv", "0"},
			{"tv", "0"}
		};
		string json = post(Action("/song/lyric", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		return std::make_shared<CLyric>(data);
	}
	std::shared_ptr<CMy> CNeteaseMusicAPI::GetMyself() {
		std::map<string, string> p = {};
		string json = post(Action("/w/nuser/account/get/", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		if (data.HasMember("profile") && !data["profile"].IsNull()) {
			json = post(Action("/v1/user/detail/" + std::to_string(data["profile"]["userId"].GetInt()), p));
			data.Parse(json.c_str());
			return std::make_shared<CMy>(data);
		}
		return std::shared_ptr<CMy>(nullptr);
	}
	std::shared_ptr<CUser> CNeteaseMusicAPI::GetUser(neteaseid_t userid) {
		std::map<string, string> p = {};
		string json = post(Action("/v1/user/detail/" + std::to_string(userid), p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		return std::make_shared<CUser>(data);
	}

	static string s_szCookie = "./neteaseapicookie";
	const std::string CNeteaseMusicAPI::CookieOutPath(){
		return s_szCookie;
	}
	const std::string CNeteaseMusicAPI::CokkieInPath(){
		return s_szCookie;
	}
	void CNeteaseMusicAPI::SetCookie(const char* cookie) {
		s_szCookie = cookie;
	}
	CLocalUser* CNeteaseMusicAPI::GetUser() {
		return &m_pUser;
	}
	std::shared_ptr<CPlayList> CNeteaseMusicAPI::GetPlayList(neteaseid_t listid) {
		std::map<string, string> p = {
			{"id", std::to_string(listid)},
			{"n", "100000"}
		};
		string json = post(Action("/v6/playlist/detail", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		return std::make_shared<CPlayList>(data);
	}

	void CLocalUser::SendCaptcha(neteaseid_t phone, int country){
		std::map<string, string> p = {
			{"ctcode", std::to_string(country)},
			{"cellphone", std::to_string(phone)}
		};
		post(Action("/sms/captcha/sent", p));
	}
	neteasecode_t CLocalUser::CellPhone(neteaseid_t phone, int captcha, int country){
		std::map<string, string> p = {
			{"countrycode", std::to_string(country)},
			{"captcha", std::to_string(captcha)},
			{"rememberLogin", "1"},
			{"phone", std::to_string(phone)}
		};
		return GetCookiePost(Action("/login/cellphone", p));
	}
	neteasecode_t CLocalUser::CellPhone(neteaseid_t phone, string& passwd, int country){
		std::map<string, string> p = {
			{"countrycode", std::to_string(country)},
			{"phone", std::to_string(phone)},
			{"rememberLogin", "1"},
			{"password", Md5Encode(passwd)}
		};
		return GetCookiePost(Action("/login/cellphone", p));
	}
	neteasecode_t CLocalUser::EMail(string& mail, string& passwd){
		std::map<string, string> p = {
			{"username",mail},
			{"rememberLogin", "1"},
			{"password", Md5Encode(passwd)}
		};
		return GetCookiePost(Action("/login", p));
	}
	string CLocalUser::RequestQRKey(){
		std::map<string, string> p = {
			{"type", "1"}
		};
		string json = post(Action("/login/qrcode/unikey", p));
		rapidjson::Document data;
		data.Parse(json.c_str());
		if (data.HasMember("unikey"))
			return data["unikey"].GetString();
		return "";
	}
	CLocalUser::QRStatue CLocalUser::QRCheck(string& qrKey){
		std::map<string, string> p = {
			{"type", "1"},
			{"key", qrKey}
		};
		return QRStatue(GetCookiePost(Action("/login/qrcode/client/login", p), 803));
	}
	CLocalUser::QRCode CLocalUser::GetQRCode(string& qrKey){
		QRCode out;
		const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(("https://music.163.com/login?codekey=" + qrKey).c_str(), qrcodegen::QrCode::Ecc::LOW);
		for (int y = 0; y < qr.getSize(); y++) {
			for (int x = 0; x < qr.getSize(); x++) {
				out.data.push_back(qr.getModule(x, y));
			}
		}
		out.size = qr.getSize();
		return out;
	}
	neteasecode_t CLocalUser::GetCookiePost(Action& action, int successcode){
		string str = post(action);
		rapidjson::Document data;
		data.Parse(str.c_str());
		neteasecode_t neteasecode = 0;
		if (data.HasMember("code"))
			neteasecode = data["code"].GetInt();
		return neteasecode;
	}
}
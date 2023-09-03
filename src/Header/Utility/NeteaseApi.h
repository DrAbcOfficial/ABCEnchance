#ifndef _NEATEASE_API_H_
#define _NEATEASE_API_H_

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>

#include <rapidjson/document.h>

#define NETEASEAPI_VERSION "1.0.0"

namespace netease {
	using std::string;

	struct Action {
		string url;
		std::map<string, string> post;
		Action(string u, std::map<string, string> p) : url("https://music.163.com/api" + u), post(p) {}
	};

	enum SearchType {
		ST_SONG = 1,
		ST_ALBUM = 10,
		ST_ARTIST = 100,
		ST_PLAYLIST = 1000,
		ST_USER = 1002
	};

	enum DailyTaskType {
		DTT_ANDROID = 0,
		DTT_WEB = 1
	};

	typedef long long neteaseid_t;
	typedef int neteasecode_t;

	class CBase163Object {
	public:
		string name;
		neteaseid_t id = 0;
		CBase163Object(rapidjson::Value& json);
	};
	class CArtist : public CBase163Object {
	public:
		CArtist(rapidjson::Value& json);
	};
	class CAlbum : public CBase163Object {
	public:
		string picUrl;
		CAlbum(rapidjson::Value& json);
	};
	class CMusic : public CBase163Object {
	public:
		std::vector<std::shared_ptr<CArtist>> ar;
		std::shared_ptr <CAlbum> al;

		string aliasName;
		// 0 1 = free
		// 2 = need money
		int copyright = 0;
		unsigned long duration = 0;
		CMusic(rapidjson::Value& json);

		string GetPlayUrl(const char* quality, char* encode);
	};
	class CLyricItem {
	public:
		std::chrono::milliseconds time;
		string text;
		CLyricItem(string raw);
	};
	class CLyric {
	public:
		std::vector<CLyricItem*> lyric;
		std::vector<CLyricItem*> tlyric;
		CLyric(rapidjson::Document& json);
		~CLyric();
	};
	class CUser : public CBase163Object {
	public:
		CUser(rapidjson::Value& json);
		string signature;
		unsigned int level = 0;
		unsigned int vip = 0;
		unsigned long listenSongs = 0;
		unsigned int playlistCount = 0;
		unsigned long createTime = 0;
		unsigned long createDay = 0;
		neteaseid_t likePlayListId = 0;
	};
	class CMy : public CUser {
	public:
		CMy(rapidjson::Value& json);
	};
	//Login
	class CLocalUser {
	public:
		//Captcha
		void SendCaptcha(neteaseid_t phone, int country);
		neteasecode_t CellPhone(neteaseid_t phone, int captcha, int country);
		neteasecode_t CellPhone(neteaseid_t phone, string& passwd, int country);
		//EMail
		neteasecode_t EMail(string& mail, string& passwd);
		//QRKey
		enum class QRStatue {
			INVALID = 800,
			WAITINGSCAN = 801,
			AUTHORIZING = 802,
			OK = 803
		};
		struct QRCode {
			size_t size;
			std::vector<bool> data;
		};
		string RequestQRKey();
		QRStatue QRCheck(string& qrKey);
		QRCode GetQRCode(string& qrKey);
	private:
		neteasecode_t GetCookiePost(Action& action, int successcode = 200);
	};

	class CNeteaseMusicAPI {
	public:
		std::shared_ptr<CArtist> GetArtist(neteaseid_t id);
		std::vector<std::shared_ptr<CMusic>> GetAlbumSongs(neteaseid_t id);
		std::vector<std::shared_ptr<CMusic>> SearchSongs(const string& keyword, int limit = 30, int offset = 0);
		std::shared_ptr<CMusic> GetSongDetail(neteaseid_t id);
		std::shared_ptr<CLyric> GetLyric(neteaseid_t songid);
		std::shared_ptr<CUser> GetUser(neteaseid_t userid);
		std::shared_ptr<CMy> GetMyself();

		static const std::string CookieOutPath();
		static const std::string CokkieInPath();

		CLocalUser* GetUser();
	private:
		CLocalUser m_pUser;
		string m_szIcookie = "/tmp/neteaseapicookie";
		string m_szOcookie = "/tmp/neteaseapicookie";
	};
}

#endif
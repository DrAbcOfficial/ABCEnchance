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
		ST_USER = 1002,
		ST_MV = 1004,
		ST_LYRIC = 1006,
		ST_RADIO = 1009,
		ST_VIDEO = 1014
	};

	enum DailyTaskType {
		DTT_ANDROID = 0,
		DTT_WEB = 1
	};

	typedef unsigned long long neteaseid_t;
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
		// 0 = free
		// 1 = need money
		bool copyright = false;
		unsigned long duration = 0;
		CMusic(rapidjson::Value& json);
		virtual string GetPlayUrl(const char* quality, char* encode);
	};
	class CDjMusic : public CMusic {
	public:
		neteaseid_t mainTrackId;
		string coverUrl;
		unsigned long listenerCount;
		unsigned long likedCount;
		string description;
		CDjMusic(rapidjson::Value& json);
		string GetPlayUrl(const char* quality, char* encode) override;
	};
	class CLyric {
	public:
		std::string lyric;
		std::string tlyric;
		CLyric(rapidjson::Document& json);
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
		std::vector<neteaseid_t> GetDailyRecommend();
		std::vector<neteaseid_t> GetFM();
	};
	class CPlayList : public CBase163Object {
	public:
		CPlayList(rapidjson::Value& json);
		std::vector<neteaseid_t> mucics;
		string coverUrl;
		unsigned int count = 0;
		unsigned long playCount = 0;
		std::string creator;
		std::string description;
	};

	class CSearchResult {
	public:
		string name;
		neteaseid_t id = 0;
		string extra;
		bool copyright = false;
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
		std::vector<std::shared_ptr<CDjMusic>> GetRadioSongs(neteaseid_t id);
		std::vector<std::shared_ptr<CSearchResult>> Search(const string& keyword, SearchType type, int limit = 30, int offset = 0);
		std::shared_ptr<CMusic> GetSongDetail(neteaseid_t id);
		std::shared_ptr<CDjMusic> GetDjSongDetail(neteaseid_t id);
		std::shared_ptr<CLyric> GetLyric(neteaseid_t songid);
		std::shared_ptr<CUser> GetUser(neteaseid_t userid);
		std::shared_ptr<CMy> GetMyself();
		std::shared_ptr<CPlayList> GetPlayList(neteaseid_t listid);

		static const std::string CookieOutPath();
		static const std::string CokkieInPath();
		static void SetCookie(const char* cookie);

		CLocalUser* GetUser();
	private:
		CLocalUser m_pUser;
	};
}

#endif
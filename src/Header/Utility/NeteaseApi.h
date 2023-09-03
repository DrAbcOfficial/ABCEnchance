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
		CMusic(rapidjson::Value& json);
	};
	class CLyricItem {
	public:
		std::chrono::milliseconds time;
		string text;
		CLyricItem(string& raw);
	};
	class CLyric {
	public:
		std::vector<std::shared_ptr<CLyricItem>> lyric;
		std::vector<std::shared_ptr<CLyricItem>> tlyric;
		CLyric(rapidjson::Document& json);
	};
	//Login
	class CLocalUser {
	public:
		bool IsLogin() {
			return m_bLogined;
		}
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
		bool m_bLogined;
	};

	class CNeteaseMusicAPI {
	public:
		std::shared_ptr<CArtist> GetArtist(neteaseid_t id);
		std::vector<std::shared_ptr<CMusic>> GetAlbumSongs(neteaseid_t id);
		std::vector<std::shared_ptr<CMusic>> SearchSongs(const string& keyword, int limit = 30, int offset = 0);
		std::shared_ptr<CMusic> GetSongDetail(neteaseid_t id);
		std::shared_ptr<CLyric> GetLyric(neteaseid_t songid);

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
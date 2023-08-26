#ifndef _NEATEASE_API_H_
#define _NEATEASE_API_H_

#include <string>
#include <vector>
#include <memory>

using std::string;

#define NETEASEAPI_VERSION "1.0.0"

namespace netease {
	struct Action {
		string url;
		string post;

		Action(string u, string p) : url("https://music.163.com/api" + u), post(p) {}
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

	struct singer_t {
		string name;
		int id;
	};

	struct album_t {
		string name;
		int id;
		string picUrl;
	};

	struct song_t {
		string name;
		int id;
		std::vector<std::shared_ptr<singer_t>> ar;
		album_t al;
	};

	/*Action album(long long id);
	Action artist(long long id);
	Action artist_albums(long long id, int limit = 30, int offset = 0);
	Action artist_introduction(long long id);
	Action login_cellphone(long long phone, const string& pswd);
	Action music_url(long long id, int bitrate = 999000);
	Action personal_fm();
	Action playlist_detail(long long id, int recent_collect = 8);
	Action dailytask(DailyTaskType type = DTT_WEB);*/
	std::vector<std::shared_ptr<song_t>> search(const string& keyword, SearchType type, int limit = 30, int offset = 0);
	std::shared_ptr<song_t> song_detail(long long id);
	/*Action user_detail(long long id);
	Action user_playlist(long long id, int limit = 30, int offset = 0);*/

}

#endif
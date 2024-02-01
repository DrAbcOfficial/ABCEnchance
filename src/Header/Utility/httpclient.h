#include <string>
#include <vector>
#include <functional>

#include <IUtilHTTPClient.h>

enum class HTTPCLIENT_FAILED_CODE {
	REQUEST_ERROR,
	RESPONSE_ERROR
};
enum class HTTPCLIENT_STATE {
	INVALID,
	PENDING,
	STARTED,
	RESPONDED,
	FINISHED,
	EXCEPTIONED,
	DESTORYED
};

class CHttpCookieJar {
public:
	CHttpCookieJar(const char* path);
	void Save();
	std::string Get();
	void Set(const char* cookie);
private:
	std::string m_szPath;
	std::string m_szCookie;
};

typedef struct httpContext_s {
	std::string url;
	UtilHTTPMethod method;
	CHttpCookieJar* cookie;
} httpContext_t;

class CHttpClientItem : IUtilHTTPCallbacks {
public:
	CHttpClientItem(httpContext_s* ctx);
	/// <param name="func">first args must be IUtilHTTPResponse*</param>
	/// <param name="...args"></param>
	/// <returns></returns>
	template <typename F, typename... Args>
	CHttpClientItem* OnRespond(F&& func, Args&&... args) {
		m_pOnResponse = std::bind(std::forward<F>(func), std::placeholders::_1, std::forward<Args>(args)...);
		return this;
	}
	template <typename F, typename... Args>
	CHttpClientItem* OnDestroy(F&& func, Args&&... args) {
		m_pOnDestory = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
		return this;
	}
	template <typename F, typename... Args>
	CHttpClientItem* OnFinish(F&& func, Args&&... args) {
		m_pOnFinish = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
		return this;
	}
	/// <param name="func">first args must be HTTPCLIENT_FAILED_CODE</param>
	/// <param name="...args"></param>
	/// <returns></returns>
	template <typename F, typename... Args>
	CHttpClientItem* OnFailed(F&& func, Args&&... args) {
		m_pOnFailed = std::bind(std::forward<F>(func), std::placeholders::_1, std::forward<Args>(args)...);
		return this;
	}

	CHttpClientItem* Create(bool async);

	CHttpClientItem* Start();
	IUtilHTTPResponse* StartSync();

	CHttpClientItem* SetFeild(const char* key, const char* var);
	HTTPCLIENT_STATE GetState() const;

	UtilHTTPRequestId_t GetId();
	bool Interrupt();
protected:
	virtual void Destroy();
	virtual void OnResponseComplete(IUtilHTTPRequest* RequestInstance, IUtilHTTPResponse* ResponseInstance);
	virtual void OnUpdateState(UtilHTTPRequestState NewState);
private:
	std::function<void(IUtilHTTPResponse*)> m_pOnResponse = nullptr;
	std::function<void()> m_pOnFinish = nullptr;
	std::function<void()> m_pOnDestory = nullptr;
	std::function<void(HTTPCLIENT_FAILED_CODE)> m_pOnFailed = nullptr;

	CHttpCookieJar* m_pCookieJar = nullptr;

	bool m_bAsync;
	UtilHTTPRequestId_t m_pId;
	//Only for sync
	IUtilHTTPRequest* m_pSyncReq;
	
	httpContext_s m_hContext;
	HTTPCLIENT_STATE m_iStatue;
};

class CHttpClient {
public:
	static void Init();
	static void RunFrame();
	static void ShutDown();

	void CheckAll();
	void ClearAll();
	CHttpClientItem* Fetch(const char* url, UtilHTTPMethod method);
	CHttpClientItem* Fetch(httpContext_s* ctx);
	bool Interrupt(UtilHTTPRequestId_t id);
private:
	std::vector<CHttpClientItem*> m_aryItems;
};

CHttpClient* GetHttpClient();
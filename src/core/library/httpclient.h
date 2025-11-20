#include <string>
#include <vector>
#include <cstddef>
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
	CHttpCookieJar();
	CHttpCookieJar(const char* path);
	void Load(const char* path);
	void Save();
	std::string Get();
	void Set(const char* cookie);
	void Set(const std::string& cookie);
	size_t Size();
private:
	std::string m_szPath;
	std::string m_szCookie;
};

typedef struct httpContext_s {
	std::string url;
	UtilHTTPMethod method;
	bool async;
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

	CHttpClientItem* Start();
	IUtilHTTPResponse* StartSync();

	CHttpClientItem* SetFeild(const char* key, const char* var);
	CHttpClientItem* SetPostBody(const char* contentType, const char* payload, size_t payloadSize);
	CHttpClientItem* SetCookieJar(CHttpCookieJar* jar);
	HTTPCLIENT_STATE GetState() const;
	bool Interrupt();
protected:
	virtual void Destroy() override;
	virtual void OnResponseComplete(IUtilHTTPRequest* RequestInstance, IUtilHTTPResponse* ResponseInstance) override;
	virtual void OnUpdateState(IUtilHTTPRequest* RequestInstance, IUtilHTTPResponse* ResponseInstance, UtilHTTPRequestState NewState) override;
	//Called when receive chunked payload data
	virtual void OnReceiveData(IUtilHTTPRequest* RequestInstance, IUtilHTTPResponse* ResponseInstance, const void* pData, size_t cbSize) override;
private:
	std::function<void(IUtilHTTPResponse*)> m_pOnResponse = nullptr;
	std::function<void()> m_pOnFinish = nullptr;
	std::function<void()> m_pOnDestory = nullptr;
	std::function<void(HTTPCLIENT_FAILED_CODE)> m_pOnFailed = nullptr;

	CHttpCookieJar* m_pCookieJar = nullptr;

	std::vector<std::byte> m_aryReciveData = {};

	bool m_bAsync = false;
	IUtilHTTPRequest* m_pRequest = nullptr;
	
	httpContext_s m_hContext;
	HTTPCLIENT_STATE m_iStatue;
};

class CHttpClient
{
public:
	void Init();
	void RunFrame();
	void ShutDown();

	void CheckAll();
	void ClearAll();
	CHttpClientItem* Fetch(const char* url, UtilHTTPMethod method, bool async = true);
	CHttpClientItem* Fetch(httpContext_s* ctx);
	bool Interrupt(CHttpClientItem* pDestory);
private:
	std::vector<CHttpClientItem*> m_aryItems;
};

CHttpClient* GetHttpClient();
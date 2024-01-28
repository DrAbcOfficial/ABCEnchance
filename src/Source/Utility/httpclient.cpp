#include <plugins.h>

#include "httpclient.h"


static HINTERFACEMODULE g_hUtilHTTPClient;
static IUtilHTTPClient* g_pUtilHTTPClient;
static CHttpClient g_pHttpClient;

void CHttpClient::Init(){
	g_hUtilHTTPClient = Sys_LoadModule("UtilHTTPClient_SteamAPI.dll");
	if (!g_hUtilHTTPClient){
		SYS_ERROR("Could not load UtilHTTPClient_SteamAPI.dll");
		return;
	}
	auto factory = Sys_GetFactory(g_hUtilHTTPClient);
	if (!factory){
		SYS_ERROR("Could not get factory from UtilHTTPClient_SteamAPI.dll");
		return;
	}
	g_pUtilHTTPClient = (decltype(g_pUtilHTTPClient))factory(UTIL_HTTPCLIENT_STEAMAPI_INTERFACE_VERSION, NULL);
	if (!g_pUtilHTTPClient){
		SYS_ERROR("Could not get UtilHTTPClient from UtilHTTPClient_SteamAPI.dll");
		return;
	}
	if (g_pUtilHTTPClient){
		CUtilHTTPClientCreationContext context;
		context.m_bUseCookieContainer = true;
		context.m_bAllowResponseToModifyCookie = true;
		g_pUtilHTTPClient->Init(&context);
	}
}
void CHttpClient::RunFrame(){
	if (g_pUtilHTTPClient)
		g_pUtilHTTPClient->RunFrame();
	g_pHttpClient.CheckAll();
}
void CHttpClient::ShutDown(){
	if (g_pUtilHTTPClient){
		g_pUtilHTTPClient->Shutdown();
		g_pUtilHTTPClient->Destroy();
		g_pUtilHTTPClient = nullptr;
	}
	if (g_hUtilHTTPClient){
		Sys_FreeModule(g_hUtilHTTPClient);
		g_hUtilHTTPClient = nullptr;
	}
	g_pHttpClient.ClearAll();
}
void CHttpClient::CheckAll(){
	for (auto iter = m_aryItems.begin(); iter != m_aryItems.end();) {
		auto item = *iter;
		if (item->GetState() == HTTPCLIENT_STATE::DESTORYED) {
			delete item;
			iter = m_aryItems.erase(iter);
		}
		else
			iter++;
	}
}
void CHttpClient::ClearAll(){
	for (auto iter = m_aryItems.begin(); iter != m_aryItems.end();) {
		auto item = *iter;
		delete item;
	}
	m_aryItems.clear();
}
CHttpClientItem* CHttpClient::Fetch(const char* url, UtilHTTPMethod method){
	httpContext_t ctx = {
		url,
		method
	};
	CHttpClientItem* item = new CHttpClientItem(&ctx);
	m_aryItems.push_back(item);
	return item;
}
CHttpClientItem* CHttpClient::Fetch(httpContext_s* ctx){
	CHttpClientItem* item = new CHttpClientItem(ctx);
	m_aryItems.push_back(item);
	return item;
}

CHttpClientItem::CHttpClientItem(httpContext_s* ctx) : IUtilHTTPCallbacks(){
	m_hContext.url = ctx->url;
	m_hContext.method = ctx->method;
	m_iStatue = HTTPCLIENT_STATE::INVALID;
}
CHttpClientItem* CHttpClientItem::Create(bool async){
	m_iStatue = HTTPCLIENT_STATE::PENDING;
	if (async)
		m_pId = g_pUtilHTTPClient->CreateAsyncRequest(m_hContext.url.c_str(), m_hContext.method, this)->GetRequestId();
	else
		m_pSyncReq = g_pUtilHTTPClient->CreateSyncRequest(m_hContext.url.c_str(), m_hContext.method, this);
	m_bAsync = async;
	return this;
}
CHttpClientItem* CHttpClientItem::Start(){
	if (m_bAsync) {
		auto req = g_pUtilHTTPClient->GetRequestById(m_pId);
		if (req)
			req->SendAsyncRequest();
	}
	return this;
}
IUtilHTTPResponse* CHttpClientItem::StartSync(){
	if (!m_bAsync) {
		m_pSyncReq->WaitForResponse();
		return m_pSyncReq->GetResponse();
	}
	return nullptr;
}
CHttpClientItem* CHttpClientItem::SetFeild(const char* key, const char* var){
	auto req = m_bAsync ? g_pUtilHTTPClient->GetRequestById(m_pId) : m_pSyncReq;
	if (req)
		req->SetField(key, var);
	return this;
}

HTTPCLIENT_STATE CHttpClientItem::GetState() const{
	return m_iStatue;
}
void CHttpClientItem::Destroy() {
	m_iStatue = HTTPCLIENT_STATE::DESTORYED;
	if (m_pOnDestory)
		std::invoke(m_pOnDestory);
}
void CHttpClientItem::OnResponseComplete(IUtilHTTPRequest* RequestInstance, IUtilHTTPResponse* ResponseInstance) {
	if (!RequestInstance->IsRequestSuccessful()){
		m_iStatue = HTTPCLIENT_STATE::EXCEPTIONED;
		if (m_pOnFailed)
			std::invoke(m_pOnFailed, HTTPCLIENT_FAILED_CODE::REQUEST_ERROR);
		return;
	}
	if (ResponseInstance->IsResponseError()){
		m_iStatue = HTTPCLIENT_STATE::EXCEPTIONED;
		if (m_pOnFailed)
			std::invoke(m_pOnFailed, HTTPCLIENT_FAILED_CODE::RESPONSE_ERROR);
		return;
	}
	m_iStatue = HTTPCLIENT_STATE::RESPONDED;
	if (m_pOnResponse)
		std::invoke(m_pOnResponse, ResponseInstance);
}
void CHttpClientItem::OnUpdateState(UtilHTTPRequestState NewState) {
	switch (NewState)
	{
	case UtilHTTPRequestState::Idle:
		break;
	case UtilHTTPRequestState::Requesting:
		break;
	case UtilHTTPRequestState::Responding:
		break;
	case UtilHTTPRequestState::Finished: {
		m_iStatue = HTTPCLIENT_STATE::FINISHED;
		if (m_pOnFinish)
			std::invoke(m_pOnFinish);
		break;
	}
	default:
		break;
	}
}

CHttpClient* GetHttpClient(){
	return &g_pHttpClient;
}
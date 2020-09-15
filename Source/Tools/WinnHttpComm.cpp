#include "../../Include/Tools/WinnHttpComm.hpp"
#include <windows.h>
#include <winhttp.h>

namespace WinnHttpComm
{
    std::wstring StringToWide(const std::string& s)
    {
        int len;
        int slength = (int)s.length() + 1;
        len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
        wchar_t* buf = new wchar_t[len];
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
        std::wstring r(buf);
        delete[] buf;
        return r;
    }

    Response Handle(const Request& Req)
    {
        Response Result = Response();
        if (Req.Host.empty()) return Result;
        if (Req.Method.empty()) return Result;

        std::wstring HostTemp = StringToWide(Req.Host);
        std::wstring MethodTemp = StringToWide(Req.Method);
        std::wstring URITemp = StringToWide(Req.URI);
        LPCWSTR HostWide = HostTemp.c_str();
        LPCWSTR MethodWide = MethodTemp.c_str();
        LPCWSTR URIWide = URITemp.c_str();

        BOOL Responded = FALSE;
        HINTERNET Session = nullptr, Connect = nullptr, Request = nullptr;

        Session = WinHttpOpen(L"WinHTTP/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);// Use WinHttpOpen to obtain a session handle.
        if (Session) Connect = WinHttpConnect(Session, HostWide, static_cast<INTERNET_PORT>((Req.HTTPS) ? (INTERNET_DEFAULT_HTTPS_PORT) : (INTERNET_DEFAULT_HTTP_PORT)), 0); // Specify an HTTP server.
        if (Connect) Request = WinHttpOpenRequest(Connect, MethodWide, (wcslen(URIWide) > 0) ? (URIWide) : (nullptr), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, (Req.HTTPS) ? (WINHTTP_FLAG_SECURE) : (0)); // Create an HTTP request handle.

        if (Request && !Req.Headers.empty()) // Add headers
        {
            for (const auto& [Header, Value] : Req.Headers)
            {
                std::string FullStr = Header + ":" + Value + "\r\n";
                std::wstring FullStrTemp = StringToWide(FullStr);
                LPCWSTR FullStrWide = FullStrTemp.c_str();
                if (!WinHttpAddRequestHeaders(Request, FullStrWide, (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD))
                {
                    if (!WinHttpAddRequestHeaders(Request, FullStrWide, (ULONG)-1L, WINHTTP_ADDREQ_FLAG_REPLACE)) // Header already existed (?) if it can't replace it, bail
                    {
                        if (Request) WinHttpCloseHandle(Request);
                        if (Connect) WinHttpCloseHandle(Connect);
                        if (Session) WinHttpCloseHandle(Session);
                        return Result;
                    }
                }
            }
        }

        if (Request)
        {
            if (!Req.Content.empty()) // Add content
            {
                const char* ContentBuffer = Req.Content.c_str();
                void* BodyData = (void*) ContentBuffer;
                DWORD BodyLength = strlen(ContentBuffer);

                std::string ContentTypeStr = "content-type:application/x-www-form-urlencoded";
                if (Req.Headers.count("content-type"))
                    ContentTypeStr = "content-type:" + Req.Headers.at("content-type") + "\r\n";
                std::wstring ContentTypeTemp = StringToWide(ContentTypeStr);
                LPCWSTR ContentTypeWide = ContentTypeTemp.c_str();
                Responded = WinHttpSendRequest(Request, ContentTypeWide, -1, BodyData, BodyLength, BodyLength, 0); // Send a request.
            } else
                Responded = WinHttpSendRequest(Request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0); // Send a request.
        }

        if (Responded) Responded = WinHttpReceiveResponse(Request, nullptr); // End the request.

        if (Responded)
        {
            DWORD Size = 0;
            DWORD Downloaded = 0;

            std::string ResponseString;
            DWORD StatusCode = 0;
            DWORD StatusCodeSize = sizeof(StatusCode);
            WinHttpQueryHeaders(Request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &StatusCode, &StatusCodeSize, WINHTTP_NO_HEADER_INDEX);
            do
            {
                Size = 0;
                WinHttpQueryDataAvailable(Request, &Size);// Check for available data.
                auto Buffer = new char[Size+1];
                ZeroMemory(Buffer, Size+1);
                if (WinHttpReadData(Request, (LPVOID)Buffer, Size, &Downloaded))
                    ResponseString += std::string(Buffer);
                delete [] Buffer;

            } while (Size > 0);
            Result = { std::move(ResponseString), StatusCode };
        }

        if (Request) WinHttpCloseHandle(Request);
        if (Connect) WinHttpCloseHandle(Connect);
        if (Session) WinHttpCloseHandle(Session);
        return Result;
    }

} // WinnHttpComm
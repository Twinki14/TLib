#ifndef WINNHTTPCOMM_HPP_INCLUDED
#define WINNHTTPCOMM_HPP_INCLUDED

#include <cstdint>
#include <string>
#include <map>

namespace WinnHttpComm
{
    typedef struct Response
    {
        std::string Content;
        std::uint32_t Status = 0;
    } Response;

    typedef struct Request
    {
        std::string Host;
        std::string Method;
        std::string URI;
        std::map<std::string, std::string> Headers;
        std::string Content;
        bool HTTPS = true; // False fort port 80, true for port 443 + WINHTTP_FLAG_SECURE
    } Request;

    Response Handle(const Request& Req);

} // WinnHttpComm


#endif // WINNHTTPCOMM_HPP_INCLUDED
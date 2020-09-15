#ifndef TGOTIFY_HPP_INCLUDED
#define TGOTIFY_HPP_INCLUDED

#include "WinnHttpComm.hpp"
#include "json11.hpp"
#include <string>
#include <cstdint>
#include <iostream>
#include <utility>
#include "../TScript.hpp"

namespace TGotify
{
    class Application
    {
    public:
        Application() = default;
        Application(std::string Host, std::string Token) : Host(std::move(Host)), Token(std::move(Token)) { };

        WinnHttpComm::Response Post(const std::string& Message, const std::string& Title, std::int32_t Priority, bool Markdown = false) const
        {
            if (!*this || Message.empty())
                return WinnHttpComm::Response();

            WinnHttpComm::Request Req;
            Req.Host = Host;
            Req.Method = "POST";
            Req.URI = "/message";
            Req.URI += "?token=" + Token;

            json11::Json::object Body;
            Body.emplace("message", Message);
            if (!Title.empty()) Body.emplace("title", Title);
            if (Priority >= 0) Body.emplace("priority", Priority);
            if (Markdown)
            {
                json11::Json::object Extras;
                json11::Json::object ClientDisplay;
                ClientDisplay.emplace("contentType", "text/markdown");
                Extras.emplace("client::display", ClientDisplay);
                Body.emplace("extras", std::move(Extras));
            }
            Req.Content = json11::Json(Body).dump();
            Req.Headers["content-type"] = "application/json";
            return WinnHttpComm::Handle(Req);
        }

        WinnHttpComm::Response Post(const std::string& Message, bool Markdown = false) const { return Post(Message, "", -1, Markdown); };
        WinnHttpComm::Response Post(const std::string& Message, const std::string& Title, bool Markdown = false) const { return Post(Message, Title, -1, Markdown); };
        operator bool() const { return !Host.empty() && !Token.empty(); };
    private:
        std::string Host;
        std::string Token;
    };

    class Plugin
    {
    public:
        Plugin() = default;
        Plugin(std::string Host, std::string Token, std::uint32_t ID) : Host(std::move(Host)), Token(std::move(Token)),  ID(ID) { };

        std::string GetHost() const { return Host; };
        std::string GetToken() const { return Token; };
        std::uint32_t GetID() const { return ID; };

        std::string ConstructURI() const
        {
            if (*this)
                return "/plugin/" + std::to_string(ID) + "/custom/" + Token;
            return std::string();
        };

        operator bool() const { return !Host.empty() && !Token.empty() && ID  > 0; };
    private:
        std::string Host;
        std::string Token;
        std::uint32_t ID = 0;
    };

    class Authenticator
    {
    public:
        Authenticator() = default;
        Authenticator(TGotify::Plugin TScriptAuthPlugin, std::string HeaderToken, std::string Script, std::string Token)
                    : TScriptAuthPlugin(std::move(TScriptAuthPlugin)), HeaderToken(std::move(HeaderToken)), Script(std::move(Script)), Token(std::move(Token)) { };

        void SetHeaderToken(const std::string& HeaderToken) { this->HeaderToken = HeaderToken; };
        void SetScript(const std::string& Script) { this->Script = Script; };
        void SetToken(const std::string& Token) { this->Script = Token; };

        bool Authenticate(WinnHttpComm::Response* OutResponse = nullptr) const
        {
            if (!*this)
                return false;

            WinnHttpComm::Request Req;
            Req.Host = TScriptAuthPlugin.GetHost();
            Req.Method = "GET";
            Req.URI = TScriptAuthPlugin.ConstructURI();
            Req.URI += "/verify?token=" + Token;
            Req.URI += "&script=" + Script;
            Req.Headers["TScriptAuthenticator"] = HeaderToken;
            const auto Resp = WinnHttpComm::Handle(Req);
            if (OutResponse) *OutResponse = Resp;
            if (Resp.Status == 202)
            {
                std::string Err;
                json11::Json Json = json11::Json::parse(Resp.Content, Err);
                if (!Json.is_null())
                {
                    bool MatchedScript = false;
                    bool MatchedToken = false;
                    for (const auto& Object : Json.object_items())
                    {
                        if (!Object.second.is_null() && Object.second.is_string())
                        {
                            if (Object.second.string_value() == Script) MatchedScript = true;
                            if (Object.second.string_value() == Token) MatchedToken = true;
                        }
                    }
                    return MatchedScript && MatchedToken;
                }
            }
            return false;
        }

       operator bool() const { return TScriptAuthPlugin; };
    private:
        TGotify::Plugin TScriptAuthPlugin;
        std::string HeaderToken;
        std::string Script;
        std::string Token;
    };
};

#endif // TGOTIFY_HPP_INCLUDED
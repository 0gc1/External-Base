// zeugenschutz base | join discord.gg/x3solutions
#include "offsets.h"
#include <windows.h>
#include <wininet.h>
#include <regex>
#include <cstdio>

#pragma comment(lib, "wininet.lib")

namespace off {

static std::string http_get(const std::string& url, const std::string& headers) {
    HINTERNET inet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
    if (!inet) return {};

    HINTERNET req = InternetOpenUrlA(
        inet, url.c_str(),
        headers.empty() ? nullptr : headers.c_str(),
        headers.empty() ? 0 : (DWORD)headers.size(),
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);

    std::string body;
    if (req) {
        char buf[4096];
        DWORD got = 0;
        while (InternetReadFile(req, buf, sizeof(buf), &got) && got > 0) {
            body.append(buf, got);
        }
        InternetCloseHandle(req);
    }
    InternetCloseHandle(inet);
    return body;
}
static void parse(const std::string& src)
{
    map.clear();
    std::regex re(R"(inline constexpr uintptr_t (\w+)\s*=\s*(0x[0-9A-Fa-f]+);)");
    auto it  = std::sregex_iterator(src.begin(), src.end(), re);
    auto end = std::sregex_iterator();
    for (; it != end; ++it) {
        map[(*it)[1].str()] = std::stoull((*it)[2].str(), nullptr, 16);
    }
}


bool fetch(const std::string& ver) {
    version = ver;
    std::string url = "https://rbxoffsets.xyz/api/offset/" + ver + "/raw";
    std::string hdr = "rbxoffsets.xyz: apiv1\r\n";

    std::string body = http_get(url, hdr);
    if (body.empty()) { printf("[offsets] empty response\n"); return false; }


    std::string lower;
    lower.reserve(body.size());
    for (char c : body) lower.push_back((char)tolower((unsigned char)c));
    if (lower.find("<!doctype html") != std::string::npos ||
        lower.find("offsets not found") != std::string::npos) {
        printf("[offsets] no offsets for %s on live rbxoffsets\n", ver.c_str());
        return false;
    }

    parse(body);
    if (map.empty()) { printf("[offsets] parser returned 0\n"); return false; }
    bind();
    return true;
}

static uintptr_t get(const char* name) {
    auto it = map.find(name);
    return it == map.end() ? 0 : it->second;
}

void bind()
{
    FakeDataModelPtr = get("FakeDataModelPointer");
    FakeToReal       = get("FakeDataModelToDataModel");

    Name         = get("Name");
    Parent       = get("Parent");
    Children     = get("Children");
    ChildrenEnd  = get("ChildrenEnd");
    ClassDesc    = get("ClassDescriptor");
    ClassName    = get("ClassDescriptorToClassName");

    Workspace   = get("Workspace");
    Camera      = get("Camera");
    CameraPos   = get("CameraPos");
    ViewMatrix  = get("viewmatrix");
    ViewportSz  = get("ViewportSize");
    LocalPlayer   = get("LocalPlayer");
    ModelInstance = get("ModelInstance");
    Team          = get("Team");
    TeamColor     = get("TeamColor");
    UserId        = get("UserId");
    DisplayName   = get("DisplayName");

    Humanoid   = get("HumanoidRootPartRef");
    Health     = get("Health");
    MaxHealth  = get("MaxHealth");
    WalkSpeed  = get("WalkSpeed");

    Primitive = get("Primitive");
    Position  = get("Position");


    VisualEnginePtr = get("VisualEnginePointer");
    VisualEngine    = get("VisualEngine");
    RenderView      = get("RenderView");
}

}

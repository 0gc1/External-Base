// zeugenschutz base | join discord.gg/x3solutions
#include "config.h"
#include "esp.h"
#include "watermark.h"

#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <sstream>
#include <string>

#pragma comment(lib, "shell32.lib")


namespace cfg {

static std::string file_path()
{
    char roam[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, roam) != S_OK)
        return {};

    std::string dir = std::string(roam) + "\\zsbase";
    CreateDirectoryA(dir.c_str(), NULL);
    return dir + "\\config.txt";
}

static bool as_bool(const std::string& v) { return v == "1" || v == "true"; }


void save()
{
    auto path = file_path();
    if (path.empty()) return;
    std::ofstream o(path);
    if (!o) return;

    o << "esp_enabled=" << esp::enabled  << "\n";
    o << "esp_self="    << esp::self     << "\n";
    o << "esp_box="     << esp::box      << "\n";
    o << "esp_name="    << esp::name_tag << "\n";
    o << "esp_dist="    << esp::max_dist << "\n";
    o << "watermark="   << wm::enabled   << "\n";
}


void load() {
    auto path = file_path();
    if (path.empty()) return;

    std::ifstream in(path);
    if (!in) return;

    std::string ln;
    while (std::getline(in, ln)) {
        auto eq = ln.find('=');
        if (eq == std::string::npos) continue;
        auto k = ln.substr(0, eq);
        auto v = ln.substr(eq + 1);

        if      (k == "esp_enabled") esp::enabled  = as_bool(v);
        else if (k == "esp_self")    esp::self     = as_bool(v);
        else if (k == "esp_box")     esp::box      = as_bool(v);
        else if (k == "esp_name")    esp::name_tag = as_bool(v);
        else if (k == "esp_dist")    { try { esp::max_dist = std::stof(v); } catch (...) {} }
        else if (k == "watermark")   wm::enabled   = as_bool(v);
    }
}

}

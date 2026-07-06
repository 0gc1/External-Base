// zeugenschutz base | join discord.gg/x3solutions
#include "rbx.h"
#include "mem.h"
#include "offsets.h"
#include <unordered_map>


namespace rbx {

static std::unordered_map<uintptr_t, std::vector<uintptr_t>> kid_cache;
static std::unordered_map<uintptr_t, std::string>            name_cache;
static std::unordered_map<uintptr_t, std::string>            class_cache;

static const std::vector<uintptr_t>& kids(uintptr_t inst);
static const std::string&            cname(uintptr_t inst);
static const std::string&            cclass(uintptr_t inst);


bool refresh()
{
    kid_cache.clear();
    name_cache.clear();
    class_cache.clear();

    if (!mem::base || !off::FakeDataModelPtr) return false;
    uintptr_t fdm = mem::read<uintptr_t>(mem::base + off::FakeDataModelPtr);
    if (!fdm) return false;

    datamodel = mem::read<uintptr_t>(fdm + off::FakeToReal);
    if (!datamodel) return false;

    workspace = find_child_byclass(datamodel, "Workspace");
    if (workspace) camera = mem::read<uintptr_t>(workspace + off::Camera);

    uintptr_t players_svc = find_child_byclass(datamodel, "Players");
    if (players_svc) local_player = mem::read<uintptr_t>(players_svc + off::LocalPlayer);


    if (off::VisualEnginePtr) {
        uintptr_t fresh = mem::read<uintptr_t>(mem::base + off::VisualEnginePtr);
        if (fresh) visual_eng = fresh;
    }
    return true;
}


static const std::vector<uintptr_t>& kids(uintptr_t inst) {
    auto it = kid_cache.find(inst);
    if (it != kid_cache.end()) return it->second;
    return kid_cache[inst] = children(inst);
}
static const std::string& cname(uintptr_t inst) {
    auto it = name_cache.find(inst);
    if (it != name_cache.end()) return it->second;
    return name_cache[inst] = name_of(inst);
}

static const std::string& cclass(uintptr_t inst)
{
    auto it = class_cache.find(inst);
    if (it != class_cache.end()) return it->second;
    return class_cache[inst] = classname(inst);
}


std::string name_of(uintptr_t inst) {
    if (!inst) return {};
    uintptr_t str = mem::read<uintptr_t>(inst + off::Name);
    if (!str) return {};
    return mem::read_lenstr(str);
}

std::string classname(uintptr_t inst)
{
    if (!inst) return {};
    uintptr_t desc = mem::read<uintptr_t>(inst + off::ClassDesc);
    if (!desc) return {};
    uintptr_t cn = mem::read<uintptr_t>(desc + off::ClassName);
    if (!cn) return {};
    return mem::read_string(cn);
}

std::vector<uintptr_t> children(uintptr_t inst) {
    std::vector<uintptr_t> out;
    if (!inst) return out;

    uintptr_t list = mem::read<uintptr_t>(inst + off::Children);
    if (!list) return out;
    uintptr_t start = mem::read<uintptr_t>(list);
    uintptr_t end   = mem::read<uintptr_t>(list + off::ChildrenEnd);
    if (!start || end <= start) return out;

    size_t span = end - start;
    if (span > 4096 * 16) return out;

    out.reserve(span / 16);
    for (uintptr_t cur = start; cur < end; cur += 16) {
        uintptr_t c = mem::read<uintptr_t>(cur);
        if (c) out.push_back(c);
    }
    return out;
}


uintptr_t find_child(uintptr_t inst, const char* name) {
    for (auto c : kids(inst))
        if (cname(c) == name) return c;
    return 0;
}
uintptr_t find_child_byclass(uintptr_t inst, const char* cls) {
    for (auto c : kids(inst))
        if (cclass(c) == cls) return c;
    return 0;
}


std::vector<Player> players()
{
    std::vector<Player> out;
    uintptr_t svc = find_child_byclass(datamodel, "Players");
    if (!svc) return out;

    int my_team = local_player ? mem::read<int>(local_player + off::TeamColor) : 0;

    for (auto p : kids(svc)) {
        if (cclass(p) != "Player") continue;
        Player pl{};
        pl.addr = p;
        pl.character = mem::read<uintptr_t>(p + off::ModelInstance);
        pl.name = cname(p);
        pl.friendly = (my_team != 0) && (mem::read<int>(p + off::TeamColor) == my_team);

        if (pl.character) {
            pl.hrp = find_child(pl.character, "HumanoidRootPart");
            pl.humanoid = find_child_byclass(pl.character, "Humanoid");
            if (pl.humanoid) {
                pl.health = mem::read<float>(pl.humanoid + off::Health);
                pl.max_health = mem::read<float>(pl.humanoid + off::MaxHealth);
            }
            if (pl.hrp) {
                uintptr_t prim = mem::read<uintptr_t>(pl.hrp + off::Primitive);
                if (prim) pl.pos = mem::read<Vec3>(prim + off::Position);
            }
        }

        out.push_back(pl);
    }
    return out;
}

Mat4 view_matrix() {
    Mat4 m{};
    if (!visual_eng || !off::ViewMatrix) return m;
    return mem::read<Mat4>(visual_eng + off::ViewMatrix);
}


Vec2 viewport() {
    if (!visual_eng || !off::ViewportSz) return { 1920.f, 1080.f };
    return mem::read<Vec2>(visual_eng + off::ViewportSz);
}

bool w2s(const Vec3& w, Vec2& s, const Mat4& mv, float sw, float sh)
{
    float tw_ = mv.m[12] * w.x + mv.m[13] * w.y + mv.m[14] * w.z + mv.m[15];
    if (tw_ <= 0.001f) return false;

    float tx = mv.m[0]*w.x + mv.m[1]*w.y + mv.m[2]*w.z + mv.m[3];
    float ty = mv.m[4]*w.x + mv.m[5]*w.y + mv.m[6]*w.z + mv.m[7];

    float inv = 1.0f / tw_;
    s.x = (sw * 0.5f) * (tx * inv + 1.0f);
    s.y = (sh * 0.5f) * (1.0f - ty * inv);
    return true;
}

}

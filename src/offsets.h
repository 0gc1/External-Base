// zeugenschutz base | join discord.gg/x3solutions
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace off {

    inline std::string       version;
    inline std::unordered_map<std::string, uintptr_t> map;

    inline uintptr_t FakeDataModelPtr = 0;
    inline uintptr_t FakeToReal       = 0;

    inline uintptr_t Name         = 0;
    inline uintptr_t Parent       = 0;
    inline uintptr_t Children     = 0;
    inline uintptr_t ChildrenEnd  = 0;
    inline uintptr_t ClassDesc    = 0;
    inline uintptr_t ClassName    = 0;

    inline uintptr_t Workspace   = 0;
    inline uintptr_t Camera      = 0;
    inline uintptr_t CameraPos   = 0;
    inline uintptr_t ViewMatrix  = 0;
    inline uintptr_t ViewportSz  = 0;

    inline uintptr_t LocalPlayer   = 0;
    inline uintptr_t ModelInstance = 0;
    inline uintptr_t Team          = 0;
    inline uintptr_t TeamColor     = 0;
    inline uintptr_t UserId        = 0;
    inline uintptr_t DisplayName   = 0;

    inline uintptr_t Humanoid       = 0;
    inline uintptr_t Health         = 0;
    inline uintptr_t MaxHealth      = 0;
    inline uintptr_t WalkSpeed      = 0;

    inline uintptr_t Primitive = 0;
    inline uintptr_t Position  = 0;

    inline uintptr_t VisualEnginePtr = 0;
    inline uintptr_t VisualEngine    = 0;
    inline uintptr_t RenderView      = 0;

    bool fetch(const std::string& ver);
    void bind();
}

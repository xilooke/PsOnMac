#ifndef PLATFORM_MACOS_METALSURFACE_HPP
#define PLATFORM_MACOS_METALSURFACE_HPP

#include <cstdint>

struct SDL_Window;

namespace Platform::MacOs {

class MetalSurface {
public:
    static void* CreateMetalView(SDL_Window* window);
    static void DestroyMetalView(void* view);
    static void* GetMetalLayer(void* view);
    static void GetDrawableSize(SDL_Window* window, std::uint32_t& width, std::uint32_t& height);
};

} // namespace Platform::MacOs

#endif

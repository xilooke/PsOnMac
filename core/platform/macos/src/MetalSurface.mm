#include <platform/macos/MetalSurface.hpp>

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#include <SDL.h>
#include <SDL_metal.h>
#endif

namespace Platform::MacOs {

void* MetalSurface::CreateMetalView(SDL_Window* window) {
#ifdef __APPLE__
    if (!window) return nullptr;
    return SDL_Metal_CreateView(window);
#else
    (void)window;
    return nullptr;
#endif
}

void MetalSurface::DestroyMetalView(void* view) {
#ifdef __APPLE__
    if (!view) return;
    SDL_Metal_DestroyView(view);
#else
    (void)view;
#endif
}

void* MetalSurface::GetMetalLayer(void* view) {
#ifdef __APPLE__
    if (!view) return nullptr;
    return SDL_Metal_GetLayer(view);
#else
    (void)view;
    return nullptr;
#endif
}

void MetalSurface::GetDrawableSize(SDL_Window* window, std::uint32_t& width, std::uint32_t& height) {
#ifdef __APPLE__
    if (!window) {
        width = 0;
        height = 0;
        return;
    }
    int w = 0;
    int h = 0;
    SDL_Metal_GetDrawableSize(window, &w, &h);
    width = w > 0 ? static_cast<std::uint32_t>(w) : 0;
    height = h > 0 ? static_cast<std::uint32_t>(h) : 0;
#else
    (void)window;
    width = 0;
    height = 0;
#endif
}

} // namespace Platform::MacOs

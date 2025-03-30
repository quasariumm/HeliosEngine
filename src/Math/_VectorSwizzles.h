#pragma once

#define SWIZZLE2(T, x, y, r, g) \
    vec2t<T> x##y() noexcept { return { x, y }; } \
    vec2t<T> r##g() noexcept { return { r, g }; }

#define SWIZZLE3(T, x, y, z, r, g, b) \
    vec3t<T> x##y##z() noexcept { return { x, y, z }; } \
    vec3t<T> r##g##b() noexcept { return { r, g, b }; }

#define SWIZZLE4(T, x, y, z, w, r, g, b, a) \
    vec4t<T> x##y##z##w() noexcept { return { x, y, z, w }; } \
    vec4t<T> r##g##b##a() noexcept { return { r, g, b, a }; }


#define SWIZZLES_XY(T) \
    SWIZZLE2(T, x, x, r, r) \
    SWIZZLE2(T, y, y, g, g) \
    SWIZZLE2(T, x, y, r, g) \
    SWIZZLE2(T, y, x, g, r) \
    SWIZZLE3(T, x, x, x, r, r, r) \
    SWIZZLE3(T, y, y, y, g, g, g) \
    SWIZZLE3(T, y, x, x, g, r, r) \
    SWIZZLE3(T, x, y, x, r, g, r) \
    SWIZZLE3(T, x, x, y, r, r, g) \
    SWIZZLE3(T, y, y, x, g, g, r) \
    SWIZZLE3(T, x, y, y, r, g, g) \
    SWIZZLE3(T, y, x, y, g, r, g) \
    SWIZZLE4(T, x, x, x, x, r, r, r, r) \
    SWIZZLE4(T, y, y, y, y, g, g, g, g) \
    SWIZZLE4(T, y, x, x, x, g, r, r, r) \
    SWIZZLE4(T, x, y, x, x, r, g, r, r) \
    SWIZZLE4(T, x, x, y, x, r, r, g, r) \
    SWIZZLE4(T, x, x, x, y, r, r, r, g) \
    SWIZZLE4(T, y, y, x, x, g, g, r, r) \
    SWIZZLE4(T, y, x, y, x, g, r, g, r) \
    SWIZZLE4(T, y, x, x, y, g, r, r, g) \
    SWIZZLE4(T, x, y, y, x, r, g, g, r) \
    SWIZZLE4(T, x, y, x, y, r, g, r, g) \
    SWIZZLE4(T, x, x, y, y, r, r, g, g) \
    SWIZZLE4(T, x, y, y, y, r, g, g, g) \
    SWIZZLE4(T, y, x, y, y, g, r, g, g) \
    SWIZZLE4(T, y, y, x, y, g, g, r, g) \
    SWIZZLE4(T, y, y, y, x, g, g, g, r)


#define SWIZZLES_XYZ(T) \
    SWIZZLE2(T, z, z, b, b) \
    SWIZZLE2(T, x, z, r, b) \
    SWIZZLE2(T, z, x, b, r) \
    SWIZZLE2(T, y, z, g, b) \
    SWIZZLE2(T, z, y, b, g) \
    SWIZZLE3(T, z, x, x, b, r, r) \
    SWIZZLE3(T, x, z, x, r, b, r) \
    SWIZZLE3(T, x, x, z, r, r, b) \
    SWIZZLE3(T, z, z, x, b, b, r) \
    SWIZZLE3(T, x, z, z, r, b, b) \
    SWIZZLE3(T, z, x, z, b, r, b) \
    SWIZZLE3(T, z, y, y, b, g, g) \
    SWIZZLE3(T, y, z, y, g, b, g) \
    SWIZZLE3(T, y, y, z, g, g, b) \
    SWIZZLE3(T, z, z, y, b, b, g) \
    SWIZZLE3(T, y, z, z, g, b, b) \
    SWIZZLE3(T, z, y, z, b, g, b) \
    SWIZZLE3(T, z, z, z, b, b, b) \
    SWIZZLE3(T, x, y, z, r, g, b) \
    SWIZZLE3(T, x, z, y, r, b, g) \
    SWIZZLE3(T, y, x, z, g, r, b) \
    SWIZZLE3(T, y, z, x, g, b, r) \
    SWIZZLE3(T, z, x, y, b, r, g) \
    SWIZZLE3(T, z, y, x, b, g, r) \
    SWIZZLE4(T, x, x, x, z, r, r, r, b) \
    SWIZZLE4(T, x, x, z, x, r, r, b, r) \
    SWIZZLE4(T, x, z, x, x, r, b, r, r) \
    SWIZZLE4(T, z, x, x, x, b, r, r, r) \
    SWIZZLE4(T, y, y, y, z, g, g, g, b) \
    SWIZZLE4(T, y, y, z, y, g, g, b, g) \
    SWIZZLE4(T, y, z, y, y, g, b, g, g) \
    SWIZZLE4(T, z, y, y, y, b, g, g, g) \
    SWIZZLE4(T, z, z, z, z, b, b, b, b) \
    SWIZZLE4(T, x, y, z, x, r, g, b, r) \
    SWIZZLE4(T, x, y, z, y, r, g, b, g) \
    SWIZZLE4(T, x, y, z, z, r, g, b, b) \
    SWIZZLE4(T, x, z, y, x, r, b, g, r) \
    SWIZZLE4(T, x, z, y, y, r, b, g, g) \
    SWIZZLE4(T, x, z, y, z, r, b, g, b) \
    SWIZZLE4(T, y, x, z, x, g, r, b, r) \
    SWIZZLE4(T, y, x, z, y, g, r, b, g) \
    SWIZZLE4(T, y, x, z, z, g, r, b, b) \
    SWIZZLE4(T, y, z, x, x, g, b, r, r) \
    SWIZZLE4(T, y, z, x, y, g, b, r, g) \
    SWIZZLE4(T, y, z, x, z, g, b, r, b) \
    SWIZZLE4(T, z, x, y, x, b, r, g, r) \
    SWIZZLE4(T, z, x, y, y, b, r, g, g) \
    SWIZZLE4(T, z, x, y, z, b, r, g, b) \
    SWIZZLE4(T, z, y, x, x, b, g, r, r) \
    SWIZZLE4(T, z, y, x, y, b, g, r, g) \
    SWIZZLE4(T, z, y, x, z, b, g, r, b)


#define SWIZZLES_XYZW(T) \
    SWIZZLE2(T, w, w, a, a) \
    SWIZZLE2(T, x, w, r, a) \
    SWIZZLE2(T, w, x, a, r) \
    SWIZZLE2(T, y, w, g, a) \
    SWIZZLE2(T, w, y, a, g) \
    SWIZZLE2(T, z, w, b, a) \
    SWIZZLE2(T, w, z, a, b) \
    SWIZZLE3(T, x, x, w, r, r, a) \
    SWIZZLE3(T, x, w, x, r, a, r) \
    SWIZZLE3(T, w, x, x, a, r, r) \
    SWIZZLE3(T, y, y, w, g, g, a) \
    SWIZZLE3(T, y, w, y, g, a, g) \
    SWIZZLE3(T, w, y, y, a, g, g) \
    SWIZZLE3(T, z, z, w, b, b, a) \
    SWIZZLE3(T, z, w, z, b, a, b) \
    SWIZZLE3(T, w, z, z, a, b, b) \
    SWIZZLE3(T, w, w, x, a, a, r) \
    SWIZZLE3(T, w, w, y, a, a, g) \
    SWIZZLE3(T, w, w, z, a, a, b) \
    SWIZZLE3(T, w, x, w, a, r, a) \
    SWIZZLE3(T, w, y, w, a, g, a) \
    SWIZZLE3(T, w, z, w, a, b, a) \
    SWIZZLE3(T, x, y, w, r, g, a) \
    SWIZZLE3(T, x, z, w, r, b, a) \
    SWIZZLE3(T, y, x, w, g, r, a) \
    SWIZZLE3(T, y, z, w, g, b, a) \
    SWIZZLE3(T, z, x, w, b, r, a) \
    SWIZZLE3(T, z, y, w, b, g, a) \
    SWIZZLE3(T, w, x, y, a, r, g) \
    SWIZZLE3(T, w, x, z, a, r, b) \
    SWIZZLE3(T, w, y, x, a, g, r) \
    SWIZZLE3(T, w, y, z, a, g, b) \
    SWIZZLE3(T, w, z, x, a, b, r) \
    SWIZZLE3(T, w, z, y, a, b, g) \
    SWIZZLE4(T, w, w, x, y, a, a, r, g) \
    SWIZZLE4(T, w, w, x, z, a, a, r, b) \
    SWIZZLE4(T, w, w, y, x, a, a, g, r) \
    SWIZZLE4(T, w, w, y, z, a, a, g, b) \
    SWIZZLE4(T, w, w, z, x, a, a, b, r) \
    SWIZZLE4(T, w, w, z, y, a, a, b, g) \
    SWIZZLE4(T, w, x, w, y, a, r, a, g) \
    SWIZZLE4(T, w, x, w, z, a, r, a, b) \
    SWIZZLE4(T, w, y, w, x, a, g, a, r) \
    SWIZZLE4(T, w, y, w, z, a, g, a, b) \
    SWIZZLE4(T, w, z, w, x, a, b, a, r) \
    SWIZZLE4(T, w, z, w, y, a, b, a, g) \
    SWIZZLE4(T, x, y, z, w, r, g, b, a) \
    SWIZZLE4(T, x, y, w, z, r, g, a, b) \
    SWIZZLE4(T, x, z, y, w, r, b, g, a) \
    SWIZZLE4(T, x, z, w, y, r, b, a, g) \
    SWIZZLE4(T, x, w, y, z, r, a, g, b) \
    SWIZZLE4(T, x, w, z, y, r, a, b, g) \
    SWIZZLE4(T, y, x, z, w, g, r, b, a) \
    SWIZZLE4(T, y, x, w, z, g, r, a, b) \
    SWIZZLE4(T, y, z, x, w, g, b, r, a) \
    SWIZZLE4(T, y, z, w, x, g, b, a, r) \
    SWIZZLE4(T, y, w, x, z, g, a, r, b) \
    SWIZZLE4(T, y, w, z, x, g, a, b, r) \
    SWIZZLE4(T, z, x, y, w, b, r, g, a) \
    SWIZZLE4(T, z, x, w, y, b, r, a, g) \
    SWIZZLE4(T, z, y, x, w, b, g, r, a) \
    SWIZZLE4(T, z, y, w, x, b, g, a, r) \
    SWIZZLE4(T, z, w, x, y, b, a, r, g) \
    SWIZZLE4(T, z, w, y, x, b, a, g, r) \
    SWIZZLE4(T, w, x, y, z, a, r, g, b) \
    SWIZZLE4(T, w, x, z, y, a, r, b, g) \
    SWIZZLE4(T, w, y, x, z, a, g, r, b) \
    SWIZZLE4(T, w, y, z, x, a, g, b, r) \
    SWIZZLE4(T, w, z, x, y, a, b, r, g) \
    SWIZZLE4(T, w, z, y, x, a, b, g, r)
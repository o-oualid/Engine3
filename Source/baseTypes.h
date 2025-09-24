#pragma once
#include <immintrin.h>
#include <xmmintrin.h>
typedef char int8;
typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef long long int64;
typedef unsigned int uint64;

typedef uint8 bool8;
typedef uint16 bool16;
typedef uint32 bool32;
typedef uint64 bool64;

typedef float real32;
typedef double real64;

typedef char char8;
typedef wchar_t char16;

const int32 MAX_INT32 = 0x7F'FF'FF'FF;
const int32 MIN_INT32 = 0xFF'FF'FF'FF;
const uint32 MAX_UINT = 0xFF'FF'FF'FF;

const real32 PI32 = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034f;
const real64 PI64 = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034;

inline real32 rad(real32 deg) {
    return deg * PI32 / 180;
}

inline real32 deg(real32 rad) {
    return rad * 180 / PI32;
}

#define unpack(x, i) ((real32 *)&x)[i]

inline real32 sin(real32 rad) {
    __m128 tempRad = _mm_set_ss(rad);
    // tempRad = _mm_sin_ps(tempRad);
    return _mm_cvtss_f32(tempRad);
}

inline real32 cos(real32 rad) {
    __m128 tempRad = _mm_set_ss(rad);
    // tempRad = _mm_cos_ps(tempRad);
    return _mm_cvtss_f32(tempRad);
}

inline real32 tan(real32 rad) {
    __m128 tempRad = _mm_set_ss(rad);
    //  tempRad = _mm_tan_ps(tempRad);
    return _mm_cvtss_f32(tempRad);
}

inline real32 asin(real32 rad) {
    __m128 tempRad = _mm_set_ss(rad);
    // tempRad = _mm_asin_ps(tempRad);
    return _mm_cvtss_f32(tempRad);
}

inline real32 acos(real32 rad) {
    __m128 tempRad = _mm_set_ss(rad);
    // tempRad = _mm_acos_ps(tempRad);
    return _mm_cvtss_f32(tempRad);
}

inline real32 atan(real32 rad) {
    __m128 tempRad = _mm_set_ss(rad);
    //  tempRad = _mm_atan_ps(tempRad);
    return _mm_cvtss_f32(tempRad);
}

inline real32 sqrt(real32 value) {
    __m128 temp = _mm_set_ss(value);
    temp = _mm_sqrt_ss(temp);
    return _mm_cvtss_f32(temp);
}

inline real32 min(real32 x, real32 y) {
    __m128 X = _mm_set_ss(x);
    __m128 Y = _mm_set_ss(y);
    X = _mm_min_ss(X, Y);
    return _mm_cvtss_f32(X);
}

inline real32 max(real32 x, real32 y) {
    __m128 X = _mm_set_ss(x);
    __m128 Y = _mm_set_ss(y);
    X = _mm_max_ss(X, Y);
    return _mm_cvtss_f32(X);
}

inline uint32 min(uint32 x, uint32 y) {
    __m128i VX = _mm_set_epi32(0, 0, 0, x);
    __m128i VY = _mm_set_epi32(0, 0, 0, y);

    VX = _mm_min_epu32(VX, VY);
    return _mm_cvtsi128_si32(VX);
}

inline uint32 max(uint32 x, uint32 y) {
    __m128i VX = _mm_set_epi32(0, 0, 0, x);
    __m128i VY = _mm_set_epi32(0, 0, 0, y);

    VX = _mm_max_epu32(VX, VY);
    return _mm_cvtsi128_si32(VX);
}

inline real32 clamp(real32 value, real32 min = 0.f, real32 max = 1.f) {

    __m128 tempValue = _mm_set_ss(value);
    __m128 tempMin = _mm_set_ss(min);
    __m128 tempMax = _mm_set_ss(max);
    tempValue = _mm_max_ss(_mm_min_ss(tempValue, tempMax), tempMin);
    return _mm_cvtss_f32(tempValue);
}

struct Vector2 {
    real32 x, y;
};

struct Vector2i {
    int32 x, y;
};

real32 lengthSquared(Vector2 vec) {
    return vec.x * vec.x + vec.y * vec.y;
}

union Vector3 {
    struct {
        real32 x, y, z;
    };
    real32 v[3];
    static const Vector3 Zero;
    static const Vector3 One;
    static const Vector3 X;
    static const Vector3 Y;
    static const Vector3 Z;
};

const Vector3 Vector3::Zero = Vector3{0.f, 0.f, 0.f};
const Vector3 Vector3::One = Vector3{1.f, 1.f, 1.f};
const Vector3 Vector3::X = Vector3{1.f, 0.f, 0.f};
const Vector3 Vector3::Y = Vector3{0.f, 1.f, 0.f};
const Vector3 Vector3::Z = Vector3{0.f, 0.f, 1.f};

inline bool operator==(Vector3 vec1, Vector3 vec2) {
    return vec1.x == vec2.x && vec2.y == vec2.y && vec2.z == vec2.z;
}
inline Vector3 operator*(Vector3 vec, real32 scalar) {
    __m128 tempV1 = _mm_load_ps1(&scalar);
    __m128 tempV2 = _mm_loadu_ps(vec.v);
    __m128 r = _mm_mul_ps(tempV1, tempV2);
    return Vector3{unpack(r, 0), unpack(r, 1), unpack(r, 2)};
}

inline Vector3 operator*(Vector3 vec1, Vector3 vec2) {
    __m128 tempV1 = _mm_loadu_ps(vec1.v);
    __m128 tempV2 = _mm_loadu_ps(vec2.v);
    __m128 r = _mm_mul_ps(tempV1, tempV2);
    return Vector3{unpack(r, 0), unpack(r, 1), unpack(r, 2)};
}
inline Vector3 operator/(Vector3 vec, real32 scalar) {
    __m128 tempV1 = _mm_load_ps1(&scalar);
    __m128 tempV2 = _mm_loadu_ps(vec.v);
    __m128 r = _mm_div_ps(tempV1, tempV2);
    return Vector3{unpack(r, 0), unpack(r, 1), unpack(r, 2)};
}

inline Vector3 operator/(Vector3 vec1, Vector3 vec2) {
    __m128 tempV1 = _mm_loadu_ps(vec1.v);
    __m128 tempV2 = _mm_loadu_ps(vec2.v);
    __m128 r = _mm_div_ps(tempV1, tempV2);
    return Vector3{unpack(r, 0), unpack(r, 1), unpack(r, 2)};
}

inline Vector3 operator+(Vector3 vector1, Vector3 vector2) {
    __m128 result = _mm_loadu_ps(vector1.v);
    __m128 tempV2 = _mm_loadu_ps(vector2.v);
    result = _mm_add_ps(result, tempV2);
    return Vector3{unpack(result, 0), unpack(result, 1), unpack(result, 2)};
}

inline Vector3 operator-(Vector3 vector1, Vector3 vector2) {
    __m128 result = _mm_loadu_ps(vector1.v);
    __m128 tempV2 = _mm_loadu_ps(vector2.v);
    result = _mm_sub_ps(result, tempV2);
    return Vector3{unpack(result, 0), unpack(result, 1), unpack(result, 2)};
}

inline Vector3 operator-(Vector3 vec) {
    return vec * -1;
}

inline Vector3 dot(Vector3 vec1, Vector3 vec2) {
    return Vector3::Zero;
}

inline real32 lengthSquared(Vector3 vector) {
    __m128 tempV = _mm_loadu_ps(vector.v);
    tempV = _mm_mul_ps(tempV, tempV);
    return unpack(tempV, 0) + unpack(tempV, 1) + unpack(tempV, 2);
}

inline real32 length(Vector3 vector) {
    return sqrt(lengthSquared(vector));
}

inline real32 distanceSquared(Vector3 vec1, Vector3 vec2) {
    return lengthSquared(vec1 - vec2);
}

inline real32 distance(Vector3 vec1, Vector3 vec2) {
    return length(vec1 - vec2);
}

inline Vector3 SquaredNormal(Vector3 vector) {
    Vector3 result{};

    return result;
}
inline Vector3 normal(Vector3 vector) {
    Vector3 result{};

    return result;
}

struct Color {
    real32 r, g, b;
};

struct Vertex {
    Vector2 pos;
    Color color;
    Vector2 uv;
};

struct Mat4 {
    real32 mat[16];
};

struct Quat {
    real32 x, y, z, w;
};

template <typename T, int32 Size>
struct Array {
    T data[Size];
    T &operator[](int32 i) {
        return data[i];
    }
    int32 size() const {
        return sizeof(data) / sizeof(T);
    }
};

void *Allocate(uint64 Size);

void DebugLog(const wchar_t *log);

#define AllocateType(type, count) (type *)Allocate((uint64(sizeof(type) * count)))

#define length(arr) (sizeof(arr) / sizeof((arr)[0]))

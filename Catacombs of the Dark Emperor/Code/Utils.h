#pragma once

inline bool GameState = false;          //true to pause

enum Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
};

inline int GetOppositeDir(int dir)
{
    switch (dir)
    {
        case LEFT: return RIGHT;
        case RIGHT: return LEFT;
        case UP: return DOWN;
        case DOWN: return UP;
    }

    return -1;
}

// Macro to make it easier to print to the Visual Studio console.
// Printf will not print to this console (we don't run via command line).
#define Print(...) { \
    char buf[256]; \
    snprintf(buf, sizeof(buf), __VA_ARGS__); \
    OutputDebugStringA(buf); \
}

inline float Square(float value)
{
    return value * value;
}

inline Vector2i TilePos(Vector2f pos)
{
    return Vector2i((int)floorf(pos.x), (int)floorf(pos.y));
}

// Vector math functions, since SFML lacks them.
inline Vector2f Mul(Vector2f a, Vector2f b)
{
    return Vector2f(a.x * b.x, a.y * b.y);
}

inline float Dot(Vector2f a, Vector2f b)
{
    Vector2f tmp = Mul(a, b);
    return tmp.x + tmp.y;
}

inline float Length2(Vector2f v)
{
    return Dot(v, v);
}

inline float Distance2(Vector2f a, Vector2f b)
{
    return Length2(b - a);
}

inline float Distance(Vector2f a, Vector2f b)
{
    return sqrtf(Distance2(a, b));
}

inline float InverseSqrt(float v)
{
    return 1.0f / sqrtf(v);
}

inline Vector2f Normalize(Vector2f v)
{
    return v * InverseSqrt(Dot(v, v));
}

inline int randomInRange(int a, int b)
{
    return (rand() % (b + 1 - a)) + a;
}

inline float randomUnit()
{
    return rand() / (float)RAND_MAX;
}

template <typename T>
inline void Shuffle(T* arr, int n)
{
    while (n > 1)
    {
        n--;
        int k = randomInRange(0, n);
        T value = arr[k];
        arr[k] = arr[n];
        arr[n] = value;
    }
}

inline bool getGameState()
{
    return GameState;
}

inline void setGameState(bool pause)
{
    GameState = pause;
}

// In order to use sf::Vector2i as a key in an std::unordered_map, 
// a hash function and comparison function must be provided.
// The Vector2iHash and Vector2iCompare structures provide this. 
// They are passed as template parameters when declaring the map.
struct Vector2iHash
{
    int operator()(Vector2i v) const
    {
        return v.y * 31 + v.x;
    }
};

struct Vector2iCompare
{
    bool operator()(Vector2i a, Vector2i b) const
    {
        return a.x == b.x && a.y == b.y;
    }
};

typedef std::unordered_set<Vector2i, Vector2iHash, Vector2iCompare> Vector2iSet;

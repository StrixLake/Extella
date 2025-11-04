#include <unordered_map>
#pragma once


// have to use a temp struct
// because XMFLOAT3 cannot be used 
// inside unordered_map
struct Float3{
    float x;
    float y;
    float z;

    bool operator==(const Float3 &other) const noexcept {
        return x == other.x && y == other.y && z == other.z;
    }

    Float3 operator/(int other){
        this->x /= other;
        this->y /= other;
        this->z /= other;
        return *this;
    }

    Float3 operator+(const Float3 &other){
        Float3 self = {this->x + other.x, this->y + other.y,this->z + other.z};
        return self;
    }

    Float3 operator-(const Float3 &other){
        Float3 out = {this->x - other.x, this->y - other.y,this->z - other.z};
        return out;
    }

    Float3 operator*(int other){
        Float3 self = {this->x*other, this->y*other, this->z*other};
        return self;
    }

    static Float3 cross(const Float3 &self, const Float3 &other){
        Float3 out;
        out.x = self.y*other.z - self.z*other.y;
        out.y = self.z*other.x - self.x*other.z;
        out.z = self.x*other.y - self.y*other.x;
        return out;
    }

    static Float3 normalize(const Float3 &other){
        float length = sqrt(other.x*other.x + other.y*other.y + other.z*other.z);
        Float3 out = other;
        out.x /= length;
        out.y /= length;
        out.z /= length;
        return out;
    }
};

struct Float2{
    float x;
    float y;

    bool operator==(const Float2 &other) const noexcept {
        return x == other.x && y == other.y;
    }
};

struct Norm{
    Float3 normal;
    int count;
};


namespace std {
    template<>
    struct hash<Float2> {
        std::size_t operator()(const Float2 &f) const noexcept {
            std::size_t hx = std::hash<float>{}(f.x);
            std::size_t hy = std::hash<float>{}(f.y);

            std::size_t seed = hx;
            seed ^= 3*hy << 3;
            return seed;
        }
        
    };
    template<>
    struct hash<Float3> {
        std::size_t operator()(const Float3 &f) const noexcept {
            std::size_t hx = std::hash<float>{}(f.x);
            std::size_t hy = std::hash<float>{}(f.y);
            std::size_t hz = std::hash<float>{}(f.z);

            std::size_t seed = hx;
            seed ^= hy << 1;
            seed ^= hz << 3;
            return seed;
        }
        
    };
}

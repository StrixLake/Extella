#pragma once
#include <boost/unordered_map.hpp> // IWYU pragma: keep
using boost::unordered_map;


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
        Float3 out = *this;
        out.x /= other;
        out.y /= other;
        out.z /= other;
        return out;
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


struct Float5{
    Float3 vertex;
    Float2 TexCord;

    bool operator==(const Float5& other) const noexcept {
        return vertex == other.vertex && TexCord == other.TexCord;
    }
};

struct Float6{
    Float3 vertex;
    Float3 normal;
    
    bool operator==(const Float6& other) const noexcept {
        return vertex == other.vertex && normal == other.normal;
    }
};

struct Float8{
    Float3 vertex;
    Float3 normal;
    Float2 TexCord;
    
    bool operator==(const Float8& other) const noexcept {
        return vertex == other.vertex && normal == other.normal && TexCord == other.TexCord;
    }
};

struct Vertex{
    Float3 vertex;

    bool operator==(const Vertex& other) const{
        return this->vertex == other.vertex;
    }
};

namespace boost {
    template<>
    struct hash<Float2> {
        size_t operator()(const Float2 &f) const noexcept {
            size_t hx = boost::hash<float>{}(f.x);
            size_t hy = boost::hash<float>{}(f.y);

            size_t seed = hx;
            seed ^= hy << 3;
            return seed;
        }
    };
    
    template<>
    struct hash<Float3> {
        size_t operator()(const Float3 &f) const noexcept {
            size_t hx = boost::hash<float>{}(f.x);
            size_t hy = boost::hash<float>{}(f.y);
            size_t hz = boost::hash<float>{}(f.z);

            size_t seed = hx;
            seed ^= hy << 1;
            seed ^= hz << 3;
            return seed;
        }
    };

    template<>
    struct hash<Vertex>{
        size_t operator()(const Vertex &f) const noexcept {
            return boost::hash<Float3>{}(f.vertex);
        }
    };

    template<>
    struct hash<Float5> {
        size_t operator()(const Float5& f) const noexcept {
            size_t hx = boost::hash<Float3>{}(f.vertex);
            size_t hy = boost::hash<Float2>{}(f.TexCord);
            

            size_t seed = hx;
            seed ^= hy << 2;
            return seed;
        }    
    };

    template<>
    struct hash<Float6> {
        size_t operator()(const Float6& f) const noexcept {
            size_t hx = boost::hash<Float3>{}(f.vertex);
            size_t hy = boost::hash<Float3>{}(f.normal);
            

            size_t seed = hx;
            seed ^= hy << 4;
            return seed;
        }    
    };

    template<>
    struct hash<Float8> {
        size_t operator()(const Float8& f) const noexcept {
            size_t hx = boost::hash<Float3>{}(f.vertex);
            size_t hy = boost::hash<Float3>{}(f.normal);
            size_t hz = boost::hash<Float2>{}(f.TexCord);

            size_t seed = hx;
            seed ^= hy << 2;
            seed ^= hz << 4;
            return seed;
        }    
    };

}

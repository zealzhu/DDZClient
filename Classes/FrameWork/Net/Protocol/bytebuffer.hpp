#ifndef _GPROTO_BYTEBUFFER_HPP
#define _GPROTO_BYTEBUFFER_HPP

#include <cassert>
#include <list>
#include <string>
#include <set>
#include <vector>
#include <memory>
#include <cstring>
#include <map>
#include <utility>

namespace proto {

class ByteBuffer {
public:
    static const size_t kDefaultSize = 64;

    ByteBuffer(): rpos_(0), wpos_(0) {
        storage_.reserve(kDefaultSize);
    }

    explicit ByteBuffer(size_t res): rpos_(0), wpos_(0) {
        storage_.reserve(res);
    }

    ByteBuffer(const ByteBuffer & buf):
        rpos_(buf.rpos_),
        wpos_(buf.wpos_),
        storage_(buf.storage_) {
    }

    virtual ~ByteBuffer() {
        Clear();
    }

    const uint8_t * Contents() const {
        return &storage_[0];
    }

    size_t Size() const {
        return storage_.size();
    }

    bool Empty() const {
        return storage_.empty();
    }

    void Resize(size_t new_size) {
        storage_.resize(new_size);
        rpos_ = 0;
        wpos_ = Size();
    }

    void Reserve(size_t re_size) {
        if (re_size > Size()) {
            storage_.reserve(re_size);
        }
    }

    void Clear() {
        storage_.clear();
        rpos_ = wpos_ = 0;
    }

    size_t GetRpos() {
        return rpos_;
    }

    void SetRpos(size_t pos) {
        rpos_ = pos;
    }

    size_t GetWpos() {
        return wpos_;
    }

    void SetWpos(size_t pos) {
        wpos_ = pos;
    }

    void Read(uint8_t* dest, size_t cnt) {
        if (rpos_ + cnt > Size()) {
            assert(false);
        }
        memcpy(dest, &storage_[rpos_], cnt);
        rpos_ += cnt;
    }

    template<typename T>
        T Read(size_t pos) const {
            if (pos + sizeof(T) > Size()) {
                assert(false);
            }
            return *(reinterpret_cast<T const*>(&storage_[pos]));
        }

    template<typename T>
        T Read() {
            T ret = Read<T>(rpos_);
            rpos_ += sizeof(T);
            return ret;
        }

    void Append(const uint8_t* src, size_t cnt) {
        if (!cnt) {
            return;
        }

        if (storage_.size() < wpos_ + cnt) {
            storage_.resize(wpos_ + cnt);
        }
        memcpy(&storage_[wpos_], src, cnt);
        wpos_ += cnt;
    }

    void Append(const char* src, size_t cnt) {
        return Append(reinterpret_cast<const uint8_t*>(src), cnt);
    }

    void Append(const std::string& str) {
        Append(reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        Append(static_cast<uint8_t>(0));
    }

    template<typename T>
        void Append(T value) {
            Append(reinterpret_cast<uint8_t*>(&value), sizeof(value));
        }

    void Put(size_t pos, const uint8_t* src, size_t cnt) {
        if (pos + cnt > Size()) {
            assert(false);
        }
        memcpy(&storage_[pos], src, cnt);
    }

    template<typename T>
        void Put(size_t pos, T value) {
            Put(pos, reinterpret_cast<uint8_t*>(&value), sizeof(value));
        }

    // read
    ByteBuffer& operator<<(bool value) {
        Append<char>(static_cast<char>(value));
        return *this;
    }
    ByteBuffer& operator<<(uint8_t value) {
        Append<uint8_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(uint16_t value) {
        Append<uint16_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(uint32_t value) {
        Append<uint32_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(uint64_t value) {
        Append<uint64_t>(value);
        return *this;
    }

    ByteBuffer& operator<<(int8_t value) {
        Append<int8_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(int16_t value) {
        Append<int16_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(int32_t value) {
        Append<int32_t>(value);
        return *this;
    }
    ByteBuffer& operator<<(int64_t value) {
        Append<int64_t>(value);
        return *this;
    }

    ByteBuffer& operator<<(float value) {
        Append<float>(value);
        return *this;
    }
    ByteBuffer& operator<<(double value) {
        Append<double>(value);
        return *this;
    }

    ByteBuffer& operator<<(const std::string &value) {
        Append(value);
        return *this;
    }
    ByteBuffer& operator<<(const char *str) {
        Append(std::string(str, str ? strlen(str) : 0));
        return *this;
    }

    ByteBuffer& operator<<(const ByteBuffer& rhs) {
        Append<uint16_t>(rhs.Size());
        Append(rhs.Contents(), rhs.Size());
        return *this;
    }

    // write
    ByteBuffer& operator>>(bool &value) {
        value = Read<char>() > 0 ? true : false;
        return *this;
    }
    ByteBuffer& operator>>(uint8_t &value) {
        value = Read<uint8_t>();
        return *this;
    }
    ByteBuffer& operator>>(uint16_t &value) {
        value = Read<uint16_t>();
        return *this;
    }
    ByteBuffer& operator>>(uint32_t &value) {
        value = Read<uint32_t>();
        return *this;
    }
    ByteBuffer& operator>>(uint64_t &value) {
        value = Read<uint64_t>();
        return *this;
    }

    ByteBuffer& operator>>(int8_t &value) {
        value = Read<int8_t>();
        return *this;
    }
    ByteBuffer& operator>>(int16_t &value) {
        value = Read<int16_t>();
        return *this;
    }
    ByteBuffer& operator>>(int32_t &value) {
        value = Read<int32_t>();
        return *this;
    }
    ByteBuffer& operator>>(int64_t &value) {
        value = Read<int64_t>();
        return *this;
    }

    ByteBuffer& operator>>(float &value) {
        value = Read<float>();
        return *this;
    }
    ByteBuffer& operator>>(double &value) {
        value = Read<double>();
        return *this;
    }

    ByteBuffer& operator>>(std::string& value) {
        value.clear();
        while (GetRpos() < Size()) {
            char c = Read<char>();
            if (c == 0)
                break;
            value += c;
        }
        return *this;
    }

    ByteBuffer& operator>>(ByteBuffer& value) {
        uint16_t size = Read<uint16_t>();
        value.Resize(size);
        Read(const_cast<uint8_t*>(value.Contents()), size);
        return *this;
    }

protected:
    size_t rpos_, wpos_;
    std::vector< uint8_t > storage_;
};

template <typename T>
ByteBuffer& operator<<(ByteBuffer& b, const std::vector<T>& v) {
    b << (uint16_t)v.size();
    for (auto& it : v) {
        b << it;
    }
    return b;
}

template <typename T>
ByteBuffer& operator>>(ByteBuffer& b, std::vector<T>& v) {
    uint16_t vsize;
    b >> vsize;
    v.clear();
    while (vsize--) {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename T>
ByteBuffer& operator<<(ByteBuffer& b, const std::list<T>& v) {
    b << (uint16_t)v.size();
    for (auto& it : v) {
        b << it;
    }
    return b;
}

template <typename T>
ByteBuffer& operator>>(ByteBuffer& b, std::list<T>& v) {
    uint16_t vsize;
    b >> vsize;
    v.clear();
    while (vsize--) {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename A, typename B>
ByteBuffer& operator<<(ByteBuffer& b, const std::pair<A, B>& p) {
    b << p.first << p.second;
    return b;
}

template <typename A, typename B>
ByteBuffer& operator>>(ByteBuffer& b, std::pair<A, B>& p) {
    b >> p.first >> p.second;
    return b;
}

template <typename K, typename V>
ByteBuffer& operator<<(ByteBuffer& b, const std::map<K, V> &m) {
    b << (uint16_t)m.size();
    for (auto& it : m) {
        b << it.first << it.second;
    }
    return b;
}

template <typename K, typename V>
ByteBuffer& operator>>(ByteBuffer& b, std::map<K, V>& m) {
    uint16_t msize;
    b >> msize;
    m.clear();
    while (msize--) {
        K k;
        V v;
        b >> k >> v;
        m.insert(std::make_pair(k, v));
    }
    return b;
}

template <typename T>
ByteBuffer& operator<<(ByteBuffer& b, const std::set<T>& s) {
    b << (uint16_t)s.size();
    for (auto& it : s) {
        b << it;
    }
    return b;
}

template <typename T>
ByteBuffer& operator>>(ByteBuffer& b, std::set<T>& s) {
    uint16_t ssize;
    b >> ssize;
    s.clear();
    while (ssize--) {
        T v;
        b >> v;
        s.insert(v);
    }
    return b;
}

}  // namespace proto

#endif  // _GPROTO_BYTEBUFFER_HPP

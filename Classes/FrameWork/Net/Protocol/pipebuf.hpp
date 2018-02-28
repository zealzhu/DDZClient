// copyright 2017 war10ck

#ifndef GPROTO_PIPE_HPP_
#define GPROTO_PIPE_HPP_

#include <functional>
#include <unordered_map>
#include <utility>
#include <memory>
#include <string>

#include "bytebuffer.hpp"
#include "protobuf.hpp"

namespace gdp {
namespace proto {

struct PipeHead {
    uint16_t command_id;
    uint32_t session_id;
    uint32_t pipe_id;

    PipeHead() = default;

    PipeHead(const PipeHead& head) {
        command_id = head.command_id;
        session_id = head.session_id;
        pipe_id = head.session_id;
    }

    friend ByteBuffer& operator<<( ByteBuffer &b, const PipeHead& h )
    {
        b << h.command_id << h.session_id << h.pipe_id;
        return b;
    }
    friend ByteBuffer& operator>>( ByteBuffer &b, PipeHead& h )
    {
        b >> h.command_id >> h.session_id >> h.pipe_id;
        return b;
    }
};

class Pipebuf: public ByteBuffer{
 public:
     Pipebuf() = default;

     explicit Pipebuf(PipeHead& head, size_t sz = 200)
         :ByteBuffer(sz), m_head(head) {
     }

     Pipebuf(const Pipebuf& pipe)
         :ByteBuffer(pipe), m_head(pipe.m_head) {
     }

     void initialize(PipeHead& head, size_t sz = 200) {
         clear();
         m_storage.reserve(sz);
         m_head = head;
     }

     Pipebuf& operator<<(const Protobuf& protobuf) {
         *this << m_head;

         append<uint16_t>(protobuf.size());
         append(protobuf.contents(), protobuf.size());
         return *this;
     }

     Pipebuf& operator>>(Protobuf& protobuf) {
         *this >> m_head;

         uint16_t size = read<uint16_t>();
         protobuf.resize(size);
         read(const_cast<uint8_t*>(protobuf.contents()), size);
         return *this;
     }

     void set_head(PipeHead& head) {
         m_head = head;
     }

     PipeHead get_head() const {
         return m_head;
     }

     PipeHead read_head() {
         *this >> m_head;
         set_rpos(0);
         return m_head;
     }

 private:
    PipeHead m_head; 
};

class IPipeHandler : public std::enable_shared_from_this<IPipeHandler> {
 public:
     IPipeHandler() = default;
     virtual ~IPipeHandler() = default;
     virtual void call(Pipebuf& pipe) = 0;
 protected:
     using std::enable_shared_from_this<IPipeHandler>::shared_from_this;
};

template<typename T>
class PipeHandler : public IPipeHandler {
    std::function<void(PipeHead&, T&)> m_f;

 public:
    explicit PipeHandler(std::function<void(PipeHead&, T&)> f):m_f(f) {
    }
    void call(Pipebuf& pipe) {
        Protobuf protobuf;

        pipe >> protobuf;
        T message;
        protobuf >> message;

        PipeHead head = pipe.get_head();
        m_f(head, message);
    }
};

class PipeProcessor {
 public:
    PipeProcessor() = default;
    virtual ~PipeProcessor() = default;

    template<typename Obj, typename Msg>
    bool add_handler(Obj* obj, uint16_t id, void (Obj::*func)(PipeHead&, Msg&)) {
        std::string desc = Msg::descriptor()->full_name();

        if (!m_msg_ids.insert(std::make_pair(desc, id)).second) {
            return false;
        }

        if (!m_handlers.insert(
            std::make_pair(
                id,
                std::make_shared<PipeHandler<Msg> >(
                    std::bind(
                        func,
                        obj,
                        std::placeholders::_1,
                        std::placeholders::_2)))).second) {
            return false;
        }
        return true;
    }

    bool dispatch(Pipebuf& pipe) {
        auto it = m_handlers.find(pipe.read_head().command_id);
        if (it != m_handlers.end()) {
            it->second->call(pipe);
            return true;
        }
        return false;
    }

 private:
    std::unordered_map<std::string, uint16_t> m_msg_ids;
    std::unordered_map<uint16_t, std::shared_ptr<IPipeHandler> > m_handlers;
};

}  // namespace proto
}  // namespace gdp

#endif  // GPROTO_PROTOBUF_HPP_

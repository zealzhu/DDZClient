#ifndef _EVENT_BUFF_HPP
#define _EVENT_BUFF_HPP

#include "bytebuffer.hpp"

#define CreateEvtBuf(eid) new proto::EventBuff(eid)

namespace proto {

class EventBuff : public ByteBuffer{
 public:
     EventBuff() = default;
     ~EventBuff() = default;

     explicit EventBuff(uint16_t id, size_t sz = 200):ByteBuffer(sz), id_(id) {
     }

     EventBuff(const EventBuff& protobuf)
         :ByteBuffer(protobuf), id_(protobuf.id_) {
     }

     void Initialize(uint16_t id, size_t sz = 200) {
         Clear();
         storage_.reserve(sz);
         id_ = id;
     }

     void SetId(uint16_t id) {
         id_ = id;
     }

     uint16_t GetId() const {
         return id_;
     }

private:
     uint16_t id_;
};

}  // namespace proto

#endif  // _GPROTO_PROTOBUF_HPP

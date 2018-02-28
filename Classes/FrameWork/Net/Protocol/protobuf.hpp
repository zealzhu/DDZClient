#ifndef _GPROTO_PROTOBUF_HPP
#define _GPROTO_PROTOBUF_HPP

#include <google/protobuf/message.h>

#include <functional>
#include <unordered_map>
#include <memory>
#include <utility>
#include <string>

#include "bytebuffer.hpp"

namespace proto {

class Protobuf : public ByteBuffer{
 public:
     Protobuf() = default;

     explicit Protobuf(uint16_t id, size_t sz = 200):ByteBuffer(sz), id_(id) {
     }

     Protobuf(const Protobuf& protobuf)
         :ByteBuffer(protobuf), id_(protobuf.id_) {
     }

     void Initialize(uint16_t id, size_t sz = 200) {
         Clear();
         storage_.reserve(sz);
         id_ = id;
     }

     Protobuf& operator<<(const google::protobuf::Message& m) {
         size_t data_size = m.ByteSize();

         Append<uint16_t>(id_);
         storage_.resize(data_size + sizeof(uint16_t));

         m.SerializeToArray(&(storage_[wpos_]), data_size);
         wpos_ += data_size;
         return *this;
     }

     Protobuf& operator>>(google::protobuf::Message& m) {
         id_ = Read<uint16_t>();

         size_t data_size = Size() - sizeof(uint16_t);

		 if (data_size != 0) {
			 m.ParseFromArray(&(storage_[rpos_]), data_size);
		 }
         rpos_ += data_size;
         return *this;
     }

     void SetId(uint16_t id) {
         id_ = id;
     }

     uint16_t GetId() const {
         return id_;
     }

     uint16_t ReadId() {
         size_t pos = GetRpos();
         SetRpos(0);
         id_ = Read<uint16_t>();
         SetRpos(pos);
         return id_;
     }

private:
     uint16_t id_;
};

}  // namespace proto

#endif  // _GPROTO_PROTOBUF_HPP

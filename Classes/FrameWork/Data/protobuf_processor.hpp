#ifndef _PROTOBUF_PROCESSOR_HPP
#define _PROTOBUF_PROCESSOR_HPP

#include "FrameWork/Net/Protocol/protobuf.hpp"
#include <unordered_map>

class IProtobufHandler : public std::enable_shared_from_this< IProtobufHandler > {
public:
     IProtobufHandler() = default;
     virtual ~IProtobufHandler() = default;
     virtual void Call(proto::Protobuf& protobuf) = 0;
protected:
     using std::enable_shared_from_this<IProtobufHandler>::shared_from_this;
};

template<typename T>
class ProtobufHandler : public IProtobufHandler {
public:
    explicit ProtobufHandler(std::function< void (T&) > f) : func_(f) {
    }
    virtual void Call(proto::Protobuf& protobuf) {
        T message;
        protobuf >> message;

        func_(message);
    }

private:
    std::function< void (T&) > func_;
};

class ProtobufProcessor {
public:
    template<typename Obj, typename Msg>
    bool RegistHandler(Obj* obj, uint16_t id, void (Obj::*func)(Msg&)) {
        if (!handlers_.insert(
                std::make_pair(
                    id,
                    std::make_shared<ProtobufHandler< Msg > >(
                        std::bind(func, obj,
                            std::placeholders::_1)))).second) {
            return false;
        }
        return true;
    }

    bool Dispatch(proto::Protobuf & protobuf) {
        auto it = handlers_.find(protobuf.GetId());
        if (it != handlers_.end()) {
            it->second->Call(protobuf);
            return true;
        }
        return false;
    }
	ProtobufProcessor() = default;
	virtual ~ProtobufProcessor() = default;
private:
    std::unordered_map<uint16_t, std::shared_ptr< IProtobufHandler > > handlers_;
};

#define RegProtoHandler(obj, pid, cb) ProtobufProcessor::Instance().RegistHandler(obj, pid, cb)
#endif

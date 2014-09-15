#ifndef _CLIENT_RPC_CHANNEL_H_
#define _CLIENT_RPC_CHANNEL_H_

#include <common/net/packets.h>
#include <common/net/connector.h>
#include <google/protobuf/service.h>

namespace sails {

class RpcChannelImp : public ::google::protobuf::RpcChannel
{
public:
    RpcChannelImp(std::string ip, int port);
	
    void CallMethod(const google::protobuf::MethodDescriptor* method,
		    google::protobuf::RpcController* controller,
		    const google::protobuf::Message* request,
		    google::protobuf::Message* response,
		    google::protobuf::Closure* done);
    
    int sync_call(const google::protobuf::MethodDescriptor *method,
		  google::protobuf::RpcController* controller,
		  const google::protobuf::Message* request,
		  google::protobuf::Message* response);

    static common::net::PacketCommon* parser(
	common::net::Connector *connector);
private:
    common::net::Connector connector;
    std::string ip;
    int port;
};

} // namespace sails

#endif /* _CLIENT_RPC_CHANNEL_H_ */


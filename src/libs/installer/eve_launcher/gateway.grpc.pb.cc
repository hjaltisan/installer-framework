// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: eve_launcher/gateway.proto

#include "eve_launcher/gateway.pb.h"
#include "eve_launcher/gateway.grpc.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace eve_launcher {
namespace gateway {

static const char* GatewayService_method_names[] = {
  "/eve_launcher.gateway.GatewayService/PublishEvent",
};

std::unique_ptr< GatewayService::Stub> GatewayService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< GatewayService::Stub> stub(new GatewayService::Stub(channel));
  return stub;
}

GatewayService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_PublishEvent_(GatewayService_method_names[0], ::grpc::internal::RpcMethod::CLIENT_STREAMING, channel)
  {}

::grpc::ClientWriter< ::eve_launcher::Event>* GatewayService::Stub::PublishEventRaw(::grpc::ClientContext* context, ::eve_launcher::gateway::DefaultResponse* response) {
  return ::grpc::internal::ClientWriterFactory< ::eve_launcher::Event>::Create(channel_.get(), rpcmethod_PublishEvent_, context, response);
}

::grpc::ClientAsyncWriter< ::eve_launcher::Event>* GatewayService::Stub::AsyncPublishEventRaw(::grpc::ClientContext* context, ::eve_launcher::gateway::DefaultResponse* response, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::eve_launcher::Event>::Create(channel_.get(), cq, rpcmethod_PublishEvent_, context, response, true, tag);
}

::grpc::ClientAsyncWriter< ::eve_launcher::Event>* GatewayService::Stub::PrepareAsyncPublishEventRaw(::grpc::ClientContext* context, ::eve_launcher::gateway::DefaultResponse* response, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncWriterFactory< ::eve_launcher::Event>::Create(channel_.get(), cq, rpcmethod_PublishEvent_, context, response, false, nullptr);
}

GatewayService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      GatewayService_method_names[0],
      ::grpc::internal::RpcMethod::CLIENT_STREAMING,
      new ::grpc::internal::ClientStreamingHandler< GatewayService::Service, ::eve_launcher::Event, ::eve_launcher::gateway::DefaultResponse>(
          std::mem_fn(&GatewayService::Service::PublishEvent), this)));
}

GatewayService::Service::~Service() {
}

::grpc::Status GatewayService::Service::PublishEvent(::grpc::ServerContext* context, ::grpc::ServerReader< ::eve_launcher::Event>* reader, ::eve_launcher::gateway::DefaultResponse* response) {
  (void) context;
  (void) reader;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace eve_launcher
}  // namespace gateway


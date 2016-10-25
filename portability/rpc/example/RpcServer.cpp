#include <iostream>

#include <gflags/gflags.h>

#include <wangle/service/Service.h>
#include <wangle/service/ExpiringFilter.h>
#include <wangle/service/ExecutorFilter.h>
#include <wangle/service/ServerDispatcher.h>
#include <wangle/bootstrap/ServerBootstrap.h>
#include <wangle/channel/AsyncSocketHandler.h>
#include <wangle/codec/LengthFieldBasedFrameDecoder.h>
#include <wangle/codec/LengthFieldPrepender.h>
#include <wangle/channel/EventBaseHandler.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>

#include <ServerSerializeHandler.h>

using namespace std;
using namespace folly;
using namespace wangle;

using thrift::test::Bonk;
using thrift::test::Xtruct;

using SerializePipeline = wangle::Pipeline<IOBufQueue&, Xtruct>;

DEFINE_int32(port, 8080, "test server port");

class RpcService : public Service<Bonk,Xtruct>
{
public:
	virtual Future<Xtruct> operator()(Bonk request) override {
		printf("Bonk: %s, %i\n", request.message.c_str(), request.type);
		return futures::sleep(std::chrono::seconds(request.type))
				.then([request]() {
					Xtruct response;
					response.string_thing = "Stop saying " + request.message + "!";
					response.i32_thing = request.type;
					return response;
				});
	}	

};

class RpcPipelineFactory : public PipelineFactory<SerializePipeline> {
public:
	SerializePipeline::Ptr newPipeline(std::shared_ptr<AsyncTransportWrapper> sock)
	{
		auto pipeline = SerializePipeline::create();
		pipeline->addBack(AsyncSocketHandler(sock));

		pipeline->addBack(EventBaseHandler());
		pipeline->addBack(LengthFieldBasedFrameDecoder());
		pipeline->addBack(LengthFieldPrepender());
		pipeline->addBack(ServerSerializeHandler());
		
		pipeline->addBack(MultiplexServerDispatcher<Bonk, Xtruct>(&service_));
		pipeline->finalize();
		return pipeline;
	}
private:
	ExecutorFilter<Bonk, Xtruct> service_{
		std::make_shared<CPUThreadPoolExecutor>(10),
		std::make_shared<RpcService>()};
};

int main(int argc,char** argv)
{
	google::ParseCommandLineFlags(&argc, &argv, true);

	ServerBootstrap<SerializePipeline> server;
	server.childPipeline(std::make_shared<RpcPipelineFactory>());
	server.bind(FLAGS_port);
	server.waitForStop();

	return 0;
}

#pragma once

#include <wangle/channel/Handler.h>

#include <ThriftTest.h>
#include <thrift/lib/cpp/util/ThriftSerializer.h>

class ServerSerializeHandler : public wangle::Handler<
	std::unique_ptr<folly::IOBuf>, thrift::test::Bonk,
	thrift::test::Xtruct, std::unique_ptr<folly::IOBuf>> 
{
public:
	virtual void read(Context* ctx, std::unique_ptr<folly::IOBuf> msg) override 
	{
		thrift::test::Bonk received;
		ser.deserialize<thrift::test::Bonk>(msg->moveToFbString(), &received);
		ctx->fireRead(received);
	}

	virtual folly::Future<folly::Unit> write(Context* ctx, thrift::test::Xtruct b) override
	{
		std::string out;
		ser.serialize<thrift::test::Xtruct>(b, &out);
		return ctx->fireWrite(folly::IOBuf::copyBuffer(out));
	}
private:
	apache::thrift::util::ThriftSerializerCompact<> ser;
}

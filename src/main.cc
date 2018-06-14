#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libplatform/libplatform.h>
#include <v8.h>
#include <iostream>

using namespace v8;

int main(int argc, char *argv[])
{
	V8::InitializeICUDefaultLocation(argv[0]);
	V8::InitializeExternalStartupData(argv[0]);

	Platform *platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

	Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	Isolate *isolate = Isolate::New(create_params);
	{
		Isolate::Scope isolate_scope(isolate);
		HandleScope handle_scope(isolate);

		Local<Context> context = Context::New(isolate);

		Context::Scope context_scope(context);

		Local<String> source =
				String::NewFromUtf8(isolate, "'Hello' + ', World!'",
														NewStringType::kNormal)
						.ToLocalChecked();

		Local<Script> script = Script::Compile(context, source).ToLocalChecked();
		Local<Value> result = script->Run(context).ToLocalChecked();
		String::Utf8Value utf8(result);
		printf("%s\n", *utf8);
	}

	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	delete platform;
	delete create_params.array_buffer_allocator;
	std::cin.get();
	return 0;
}
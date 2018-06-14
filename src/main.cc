#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libplatform/libplatform.h"
#include "v8.h"
#include <iostream>

#include "wtf_snapshot_gen.cc"

using namespace v8;
/*
class SnapshotGenerator
{
public:
	SnapshotGenerator(const char *name, const char *filename, v8::StartupData sd) : name_(name), filename_(filename), sd_(sd), file_(filename_, std::ios::binary) {}

	void Write()
	{
		file_ << "// Automatically generated file. Do not modify.\n\n";
		file_ << "#include <v8.h>\n\n";

		char buffer[500];

		snprintf(buffer, sizeof(buffer), "static const char %s_blob_data[] = {\n",
						 name_);
		file_ << buffer;

		char buffer2[5];
		for (int i = 0; i < sd_.raw_size; i++)
		{
			if ((i & 0x1F) == 0x1F)
				file_ << "\n";
			if (i > 0)
				file_ << ",";
			snprintf(buffer2, sizeof(buffer2), "%u",
							 static_cast<unsigned char>(sd_.data[i]));
			file_ << buffer2;
		}

		file_ << "\n";
		file_ << "};\n";
		snprintf(buffer, sizeof(buffer), "static const int %s_blob_size = %d;\n",
						 name_, sd_.raw_size);
		file_ << buffer;
		snprintf(buffer, sizeof(buffer), "static const v8::StartupData %s_blob =\n",
						 name_);
		file_ << buffer;
		snprintf(buffer, sizeof(buffer),
						 "{ (const char*) %s_blob_data, %s_blob_size };\n", name_, name_);
		file_ << buffer;

		snprintf(buffer, sizeof(buffer), "const v8::StartupData* StartupBlob_%s() {\n", name_);
		file_ << buffer;
		snprintf(buffer, sizeof(buffer), "  return &%s_blob;\n", name_);
		file_ << buffer;
		file_ << "}\n\n";
	}

private:
	const char *name_;
	const char *filename_;
	v8::StartupData sd_;
	std::ofstream file_;
};

void WriteFile(const char *fn, v8::StartupData startup_data)
{
	std::ofstream output(fn, std::ios::binary);
	output.write(startup_data.data, startup_data.raw_size);
	output.close();
	if (output.bad())
	{
		printf("Error writing %s\n", fn);
		exit(1);
	}
}
*/
int main(int argc, char *argv[])
{
	V8::InitializeICUDefaultLocation(argv[0]);
	V8::InitializeExternalStartupData(argv[0]);

	Platform *platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

	/*const char *source = "function f() {return 42;}";
	const char *name = "wtf";
	const char *filename = "wtf_snapshot_gen.cc";
	SnapshotGenerator *gen = new SnapshotGenerator(name, filename, data);
	gen->Write();
	*/

	//const v8::StartupData* fromSnapshot = StartupBlob_wtf();
	v8::StartupData data = wtf_blob;
	//data.data = fromSnapshot->data;
	//data.raw_size = fromSnapshot->raw_size;
	v8::ArrayBuffer::Allocator *ab_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	
	Isolate::CreateParams params;
	params.array_buffer_allocator = ab_allocator;
	params.snapshot_blob = &data;

	Isolate *isolate = Isolate::New(params);
	// isolated scope
	{
		Isolate::Scope isolate_scope(isolate);
		HandleScope handle_scope(isolate);

		Local<Context> context = Context::New(isolate);

		Context::Scope context_scope(context);

		Local<String> source =
			String::NewFromUtf8(isolate, "f()", NewStringType::kNormal).ToLocalChecked();

		Local<Script> script = Script::Compile(context, source).ToLocalChecked();
		Local<Value> result = script->Run(context).ToLocalChecked();
		String::Utf8Value utf8(result);
		printf("%s\n", *utf8);
	}

	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	delete platform;
	delete params.array_buffer_allocator;

	std::cin.get();
	return 0;
}
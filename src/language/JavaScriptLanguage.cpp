/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JavaScriptLanguage.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: skybt <skybt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 16:57:48 by skybt             #+#    #+#             */
/*   Updated: 2020/06/30 17:46:27 by skybt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "JavaScriptLanguage.hpp"

#include <v8.h>
#include <libplatform/libplatform.h>

// turns an int litteral into a string litteral
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// see https://v8.dev/docs/embed

static bool JAVASCRIPT_HEADERS_SENT = false;
static HTTPHeaders JAVASCRIPT_HEADERS = HTTPHeaders();
static bool JAVASCRIPT_BODY_EOF = false;

JavaScriptLanguage::JavaScriptLanguage() {}
JavaScriptLanguage::~JavaScriptLanguage() {}

static void HeaderFunction(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

    if (JAVASCRIPT_HEADERS_SENT) {
        isolate->ThrowException(
            v8::String::NewFromUtf8(isolate, "headers are already sent!")
                .ToLocalChecked());
        return;
    }

    if (info.Length() != 2) {
        isolate->ThrowException(
            v8::String::NewFromUtf8(isolate, "function must have 2 arguments!")
                .ToLocalChecked());
        return;
    }

    v8::Local<v8::String> strKey = info[0]->ToString(ctx).ToLocalChecked();
    v8::Local<v8::String> strValue = info[1]->ToString(ctx).ToLocalChecked();

    char* key = new char[strKey->Utf8Length(isolate) + 1];
    strKey->WriteUtf8(isolate, key);

    char* value = new char[strValue->Utf8Length(isolate) + 1];
    strValue->WriteUtf8(isolate, value);

    JAVASCRIPT_HEADERS[key] = value;
    delete[] key;
    delete[] value;
}

static void send_headers_s(int64_t status) {
    if (status != STATUS_OK)
        std::cout << "Status: " << status << "\r\n";

    std::cout << JAVASCRIPT_HEADERS << "\r\n" << std::flush;

    JAVASCRIPT_HEADERS_SENT = true;
}

static char const* send_headers(v8::Isolate* isolate) {
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> global = ctx->Global();

    if (JAVASCRIPT_HEADERS_SENT)
        return "headers are already sent!";

    v8::Local<v8::Value> statusVal;
    if (!global->Get(ctx,
                v8::String::NewFromUtf8(isolate, "STATUS").ToLocalChecked()
            ).ToLocal(&statusVal))
        return "STATUS global variable cannot be found!";

    v8::Local<v8::Integer> statusInteger;
    if (!statusVal->ToInteger(ctx).ToLocal(&statusInteger))
        return "STATUS must be an integer!";

    send_headers_s(statusInteger->Value());
    return NULL;
}

static void SendHeadersFunction(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    
    char const* err;
    if ((err = send_headers(isolate)))
        isolate->ThrowException(
            v8::String::NewFromUtf8(isolate, err)
                .ToLocalChecked());
}

static void WriteFunction(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

    char const* err;
    if (!JAVASCRIPT_HEADERS_SENT && (err = send_headers(isolate))) {
        if (err)
            isolate->ThrowException(
                v8::String::NewFromUtf8(isolate, err)
                    .ToLocalChecked());
        return;
    }

    v8::Local<v8::String> str;
    if (!info[0]->ToString(ctx).ToLocal(&str)) {
        isolate->ThrowException(
            v8::String::NewFromUtf8(isolate, "argument must be a string!")
                .ToLocalChecked());
        return;
    }

    char* buf = new char[str->Utf8Length(isolate) + 1];
    str->WriteUtf8(isolate, buf);
    std::cout << buf << std::flush;
    delete[] buf;
}

static void ReadFunction(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();

    char buffer[JAVASCRIPT_BUFFER_SIZE];

    if (JAVASCRIPT_BODY_EOF) {
        info.GetReturnValue().Set(v8::Null(isolate));
        return;
    }

    ssize_t len;
    if ((len = read(0, buffer, JAVASCRIPT_BUFFER_SIZE)) < 0) {
        isolate->ThrowException(
            v8::String::NewFromUtf8(isolate, "read error")
                .ToLocalChecked()
        );
        return;
    }

    if (len == 0) {
        JAVASCRIPT_BODY_EOF = true;
        info.GetReturnValue().Set(v8::Null(isolate));
        return;
    }

    v8::Local<v8::String> ret = v8::String::NewFromUtf8(isolate, buffer,
        v8::NewStringType::kNormal, JAVASCRIPT_BUFFER_SIZE)
        .ToLocalChecked();
    info.GetReturnValue().Set(ret);
}

static void PrintFunction(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

    for (int i = 0; i < info.Length(); i++) {
        v8::Local<v8::String> str;
        if (!info[i]->ToString(ctx).ToLocal(&str))
            continue;

        char* buf = new char[str->Utf8Length(isolate) + 1];
        str->WriteUtf8(isolate, buf);
        std::cerr << buf;
        delete[] buf;
        if (i != info.Length() - 1)
            std::cerr << " ";
    }
    std::cerr << std::endl;
}

void JavaScriptLanguage::execute(HTTPResponse const& res) {
    // Init
    v8::V8::InitializeICUDefaultLocation("webserv");
    v8::V8::InitializeExternalStartupData("webserv");
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    // Isolate
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
    v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);
        // Stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);

        // Globals

        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

        global->Set(v8::String::NewFromUtf8(isolate, "PATH_INFO").ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate, res.getCGIPathInfo().c_str()).ToLocalChecked());

        global->Set(v8::String::NewFromUtf8(isolate, "PATH_TRANSLATED").ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate, res.getCGIPathTranslated().c_str()).ToLocalChecked());

        global->Set(v8::String::NewFromUtf8(isolate, "QUERY_STRING").ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate, res.getCGIReq().getQueryString().c_str()).ToLocalChecked());

        global->Set(v8::String::NewFromUtf8(isolate, "REQUEST_METHOD").ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate, getHTTPMethodName(res.getCGIReq().getMethod())).ToLocalChecked());

        global->Set(v8::String::NewFromUtf8(isolate, "SCRIPT_NAME").ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate, res.getCGIScriptName().c_str()).ToLocalChecked());

        global->Set(v8::String::NewFromUtf8(isolate, "SERVER_NAME").ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate, res.getCGIServerName().c_str()).ToLocalChecked());

        global->Set(v8::String::NewFromUtf8(isolate, "SERVER_PORT").ToLocalChecked(),
                    v8::Number::New(isolate, res.getCGIServerPort()));

        global->Set(v8::String::NewFromUtf8(isolate, "SERVER_PROTOCOL").ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate, "HTTP/1.1").ToLocalChecked());

        global->Set(v8::String::NewFromUtf8(isolate, "SERVER_SOFTWARE").ToLocalChecked(),
                    v8::String::NewFromUtf8(isolate, "webserv/1.0").ToLocalChecked());

        v8::Local<v8::ObjectTemplate> headers = v8::ObjectTemplate::New(isolate);

        for (HTTPHeaders::const_iterator ite = res.getCGIReq().getHeaders().begin();
                ite != res.getCGIReq().getHeaders().end(); ++ite) {

            if (!strcasecmp(ite->first.c_str(), "Content-Length")) {
                global->Set(v8::String::NewFromUtf8(isolate, "CONTENT_LENGTH").ToLocalChecked(),
                            v8::String::NewFromUtf8(isolate, ite->second.c_str()).ToLocalChecked());
            } else if (!strcasecmp(ite->first.c_str(), "Content-Type")) {
                global->Set(v8::String::NewFromUtf8(isolate, "CONTENT_TYPE").ToLocalChecked(),
                            v8::String::NewFromUtf8(isolate, ite->second.c_str()).ToLocalChecked());
            }

            headers->Set(v8::String::NewFromUtf8(isolate, ite->first.c_str()).ToLocalChecked(),
                        v8::String::NewFromUtf8(isolate, ite->second.c_str()).ToLocalChecked());
        }

        global->Set(v8::String::NewFromUtf8(isolate, "HEADERS").ToLocalChecked(),
                    headers);

        global->Set(v8::String::NewFromUtf8(isolate, "STATUS").ToLocalChecked(),
                    v8::Number::New(isolate, STATUS_OK));

        global->Set(v8::String::NewFromUtf8(isolate, "BODY_EOF").ToLocalChecked(),
                    v8::False(isolate));

        // Functions

        global->Set(v8::String::NewFromUtf8(isolate, "header").ToLocalChecked(),
                    v8::FunctionTemplate::New(isolate, HeaderFunction));

        global->Set(v8::String::NewFromUtf8(isolate, "send_headers").ToLocalChecked(),
                    v8::FunctionTemplate::New(isolate, SendHeadersFunction));

        global->Set(v8::String::NewFromUtf8(isolate, "write").ToLocalChecked(),
                    v8::FunctionTemplate::New(isolate, WriteFunction));

        global->Set(v8::String::NewFromUtf8(isolate, "read").ToLocalChecked(),
                    v8::FunctionTemplate::New(isolate, ReadFunction));

        global->Set(v8::String::NewFromUtf8(isolate, "print").ToLocalChecked(),
                    v8::FunctionTemplate::New(isolate, PrintFunction));

        JAVASCRIPT_HEADERS["Content-Type"] = "text/html";
        JAVASCRIPT_HEADERS["X-Powered-By"] = "V8 v" STR(V8_MAJOR_VERSION) "." STR(V8_MINOR_VERSION);

        // Create the conttaxt and enter it

        v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);

        v8::Context::Scope context_scope(context);
        {
            // Get the source code
            std::ifstream t(res.getCGIFilePath());
            std::string code((std::istreambuf_iterator<char>(t)),
                            std::istreambuf_iterator<char>());
            t.close();

            v8::Local<v8::String> source =
                v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked();

            v8::TryCatch try_catch(isolate); // try {

            bool ok = true;

            // Compile the source code.
            v8::Local<v8::Script> script;
            if (!v8::Script::Compile(context, source).ToLocal(&script))
                ok = false;

            // Run the script to get the result.
            v8::Local<v8::Value> result;
            if (ok && !script->Run(context).ToLocal(&result))
                ok = false;

            char const* err = NULL;
            if (ok && !JAVASCRIPT_HEADERS_SENT && (err = send_headers(isolate)))
                ok = false;

            if (!ok) {
                if (err) {
                    std::cerr << err << std::endl;
                } else {
                    v8::String::Utf8Value error(isolate, try_catch.Exception()); // } catch {}
                    std::cerr << *error << std::endl;
                }
                if (!JAVASCRIPT_HEADERS_SENT)
                    send_headers_s(STATUS_INTERNAL_SERVER_ERROR);
                return;
            }
        }
    }

    std::cout << std::flush;

    JAVASCRIPT_HEADERS = HTTPHeaders();
    JAVASCRIPT_HEADERS_SENT = false;
    JAVASCRIPT_BODY_EOF = false;

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
}

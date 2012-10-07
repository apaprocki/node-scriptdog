#include <scriptdog.h>

#include <iostream>

Scriptdog::Scriptdog(int64_t timeout)
    : d_timeout(timeout)
    , d_created(false)
    , d_destroyed(false)
{
    d_loop = uv_loop_new();

    int rc = uv_thread_create(&d_thread, &Scriptdog::Run, this);
    if (rc) {
        std::cerr << "Scriptdog uv_thread_create, rc " << rc << std::endl;
    } else {
        d_created = true;
    }
}

Scriptdog::~Scriptdog()
{
    Destroy();
}

void
Scriptdog::Destroy(void)
{
    if (!d_destroyed) {
        if (d_created) {
            uv_timer_stop(&d_timer);
            uv_loop_delete(d_loop);
            uv_thread_join(&d_thread);
        }
        d_destroyed = true;
    }
}

void
Scriptdog::Run(void *arg)
{
    Scriptdog *sd = reinterpret_cast<Scriptdog *>(arg);

    int rc = uv_timer_init(sd->d_loop, &sd->d_timer);
    if (rc) {
        std::cerr << "Scriptdog uv_timer_init, rc " << rc << std::endl;
        return;
    }

    rc = uv_timer_start(&sd->d_timer, &Scriptdog::Timer, sd->d_timeout, 0);
    if (rc) {
        std::cerr << "Scriptdog uv_timer_start, rc " << rc << std::endl;
        return;
    }

    uv_run(sd->d_loop);
}

void
Scriptdog::Timer(uv_timer_t *timer, int status)
{
    V8::TerminateExecution();
}

Handle<Value>
Scriptdog::New(const Arguments& args)
{
    if (!args.IsConstructCall()) {
        return ThrowException(Exception::Error(String::New(
                    "Constructor called directly")));
    }

    if (1 != args.Length() || !args[0]->IsUint32()) {
        return ThrowException(Exception::Error(String::New(
                    "Expects millisecond timeout value")));
    }

    HandleScope scope;

    int64_t timeout = args[0]->ToUint32()->Value();

    Scriptdog *sd = new Scriptdog(timeout);
    sd->Wrap(args.This());

    return scope.Close(args.This());
}

Handle<Value>
Scriptdog::Dispose(const Arguments& args)
{
    Scriptdog *sd = ObjectWrap::Unwrap<Scriptdog>(args.This());
    sd->Destroy();

    return Undefined();
}

void
Scriptdog::Initialize(Handle<Object> target)
{
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(Scriptdog::New);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(String::NewSymbol("Scriptdog"));

    NODE_SET_PROTOTYPE_METHOD(t, "dispose", Dispose);

    target->Set(String::NewSymbol("Scriptdog"), t->GetFunction());
}

NODE_MODULE(scriptdog, Scriptdog::Initialize)

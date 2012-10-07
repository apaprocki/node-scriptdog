#include <v8.h>
#include <node.h>
#include <node_version.h>
#include <uv.h>

using namespace node;
using namespace v8;

class Scriptdog : public ObjectWrap {
  public:
    static void Initialize(Handle<Object> target);

  private:
    Scriptdog(int64_t timeout);
    Scriptdog(const Scriptdog&);
    Scriptdog& operator=(const Scriptdog&);
    ~Scriptdog();

    uv_thread_t d_thread;
    uv_loop_t *d_loop;
    uv_timer_t d_timer;
    int64_t d_timeout;
    bool d_created;
    bool d_destroyed;

    void Destroy();

    static void Run(void *arg);
    static Handle<Value> New(const Arguments& args);
    static Handle<Value> Dispose(const Arguments& args);
    static void Timer(uv_timer_t *timer, int status);
};

#include <napi.h>
#include <future>

using namespace Napi;

#define CHECK_ARGUMENT_1   if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) { \
    Object errObj = Object::New(info.Env()); \
    errObj.Set("message", "incorrect function arguments number or type"); \
    deferred.Reject(errObj); \
    return deferred.Promise(); \
  }

Value Multiply(const CallbackInfo& info) {
  Promise::Deferred deferred = Promise::Deferred::New(info.Env());
  CHECK_ARGUMENT_1  
  std::future<void> result(
    std::async(
      std::launch::async,
      [](Promise::Deferred deferred, uint32_t arg0, double arg1) {
        deferred.Resolve(Number::New(deferred.Env(), arg0 * arg1));
      },
      deferred,
      info[0].As<Number>().Uint32Value(),
      info[1].As<Number>().DoubleValue()
    )
  );
  return deferred.Promise();
}

Object Init(Env env, Object exports) {
  exports.Set("multiply", Function::New(env, Multiply));
  return exports;
}

NODE_API_MODULE(addon, Init)

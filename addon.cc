#include <napi.h>
#include <future>

using namespace Napi;

#define CHECK_ARGUMENT_1   if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) { \
    Promise::Deferred deferred(Promise::Deferred::New(info.Env())); \
    Object errObj = Object::New(info.Env()); \
    errObj.Set("message", "incorrect function arguments number or type"); \
    deferred.Reject(errObj); \
    return deferred.Promise(); \
  }

class MultiplyWorker : public AsyncWorker {
public:
  MultiplyWorker(Napi::Env &env, Number arg0, Number arg1) : AsyncWorker(env), deferred(Promise::Deferred::New(env)) {
    argi0 = arg0.Uint32Value();
    argf0 = arg1.DoubleValue();
  }
  
  ~MultiplyWorker(){}
  
  void Execute() { // this function runs in a seperate thread
    returnArrayBuffer = new uint8_t[16]; // this is to show how to return an array buffer
    returnArrayBuffer[0] = 1;
    returnArrayBuffer[1] = 2;
    returnArrayBuffer[2] = 3;
  }
  
  void OnOK() {
    Object resultObj = Object::New(deferred.Env());
    resultObj.Set("returnArrayBuffer", ArrayBuffer::New(deferred.Env(), returnArrayBuffer, 3, finalizeCallback));
    resultObj.Set("multiplyResult", Number::New(deferred.Env(), argi0 * argf0));
    deferred.Resolve(resultObj);
  }
  
  void OnError(Error const &error) {
    Object resultObj = Object::New(deferred.Env());
    resultObj.Set("returnArrayBuffer", ArrayBuffer::New(deferred.Env(), returnArrayBuffer, 3, finalizeCallback));
    resultObj.Set("message", error.Value().Get("message"));
    deferred.Reject(resultObj);
  }
  
  Promise GetPromise() {return deferred.Promise();}
  
private:
  Promise::Deferred deferred;
  uint32_t argi0;
  double argf0;
  uint8_t *returnArrayBuffer;
  
  static void finalizeCallback(Napi::Env env, void* data) {delete data;}
};

Value Multiply(const CallbackInfo& info) {
  CHECK_ARGUMENT_1
  Number arg0 = info[0].As<Number>();
  Number arg1 = info[1].As<Number>();
  
  MultiplyWorker *multiplyWorker = new MultiplyWorker(info.Env(), arg0, arg1);
  multiplyWorker->Queue();
  
  return multiplyWorker->GetPromise();
}

Object Init(Env env, Object exports) {
  exports.Set("multiply", Function::New(env, Multiply));
  return exports;
}

NODE_API_MODULE(addon, Init)
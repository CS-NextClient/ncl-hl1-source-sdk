#include "promise_like.h"

CefJsPromiseLike::CefJsPromiseLike(CefRefPtr<CefV8Value> resolveFunc, CefRefPtr<CefV8Value> rejectFunc) {
	resolveFunc_ = resolveFunc;
	rejectFunc_ = rejectFunc;
	shotedContext_ = CefV8Context::GetCurrentContext();
}

CefRefPtr<CefV8Context> CefJsPromiseLike::GetContext() {
	return shotedContext_;
}

void CefJsPromiseLike::Resolve(CefRefPtr<CefV8Value> value) {
	CefRefPtr<CefV8Value> retval;
    CefRefPtr<CefV8Exception> exception;

	resolveFunc_->ExecuteFunctionWithContext(
		shotedContext_, nullptr, CefV8ValueList({value}),
		retval, exception, true
	);
}

void CefJsPromiseLike::Reject(CefRefPtr<CefV8Value> reason) {
	CefRefPtr<CefV8Value> retval;
    CefRefPtr<CefV8Exception> exception;

	rejectFunc_->ExecuteFunctionWithContext(
		shotedContext_, nullptr, CefV8ValueList({reason}),
		retval, exception, true
	);
}
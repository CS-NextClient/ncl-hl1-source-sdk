#pragma once

#include <cef.h>

class CefJsPromiseLike {
	CefRefPtr<CefV8Value> resolveFunc_, rejectFunc_;
	CefRefPtr<CefV8Context> shotedContext_;
	
public:
	CefJsPromiseLike(CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Value> resolveFunc, CefRefPtr<CefV8Value> rejectFunc);

	void Resolve(CefRefPtr<CefV8Value> value);
	void Reject(CefRefPtr<CefV8Value> reason);
	CefRefPtr<CefV8Context> GetContext();
};
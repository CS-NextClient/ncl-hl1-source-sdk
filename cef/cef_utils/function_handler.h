#pragma once

#include <cef.h>
#include <functional>

class CefFunctionV8Handler : public CefV8Handler {
	IMPLEMENT_REFCOUNTING(CefFunctionV8Handler);

	typedef bool(ExecutionHadler)(
		const CefString&, CefRefPtr<CefV8Value>, 
		const CefV8ValueList&, CefRefPtr<CefV8Value>&, CefString&
	);

	std::function<ExecutionHadler> handler_;

public:
	CefFunctionV8Handler(std::function<ExecutionHadler> handler): handler_(handler) {};

	bool Execute(
		const CefString& name, CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception
	) { 
		return handler_(name, object, arguments, retval, exception); 
	}
};
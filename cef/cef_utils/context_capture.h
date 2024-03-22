#pragma once

#include <cef.h>

class CefV8ContextCapture {
	CefRefPtr<CefV8Context> context_;
public:
	CefV8ContextCapture(CefRefPtr<CefV8Context> context) {
		context_ = context;
		context_->Enter();
	}
	~CefV8ContextCapture() {
		context_->Exit();
	}
};
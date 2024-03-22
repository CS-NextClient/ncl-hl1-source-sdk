#pragma once

#include <cef.h>
#include <functional>

class CefFunctionTask : public CefTask {
	IMPLEMENT_REFCOUNTING(CefFunctionTask);
	std::function<void()> handler_;

public:
	CefFunctionTask(std::function<void()> handler)
		: handler_(handler) {};

	void Execute(CefThreadId threadId) { handler_(); }
};
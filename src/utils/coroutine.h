#pragma once
#include <iostream>
#include <coroutine>
#include <thread>

namespace coroutine {
	struct Task {
		struct promise_type {
			promise_type() {
			}

			Task get_return_object() {
				return { std::coroutine_handle<Task::promise_type>::from_promise(*this) };
			}

			std::suspend_always initial_suspend() {
				return {};
			}

			std::suspend_never final_suspend() noexcept {
				return {};
			}

			void return_void() {
			}

			void unhandled_exception() {}

			//template <class T >
			auto yield_value(int val) {
				return std::suspend_never();
			}
		};

		std::coroutine_handle<Task::promise_type> handle_;
	};

	template <class T>
	struct awaiter {
		bool await_ready() {
			return false;
		}

		void await_suspend(std::coroutine_handle<typename Task::promise_type> handle) {
		}

		//������await_resume��ֱ��ִ��Э�̺�����co_await��������
		void await_resume() {
			std::cout << "12.await_resume()\n";
		}
	};

	template <class Func, class... Args>
	Task CreateCoroutine(Func fn, Args&&... args) {
		fn(std::forward<Args>(args)...);
		co_return;
	}

	void SwitchToCoroutine(Task task) {
		task.handle_.resume();
	}
}

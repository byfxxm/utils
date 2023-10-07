#include <iostream>
#include <coroutine>
#include <thread>
#include <future>

namespace coroutine {
	struct Task {
		struct promise_type {
			promise_type() {
			}

			Task get_return_object() {
				return { std::coroutine_handle<Task::promise_type>::from_promise(*this) };
			}

			std::suspend_never initial_suspend() {
				return {};
			}

			std::suspend_never final_suspend() noexcept {
				return {};
			}

			void return_value(auto&& v) {
			}

			void unhandled_exception() {
			}
		};

		std::coroutine_handle<Task::promise_type> handle_;
	};

	struct Awaiter {
		bool await_ready() {
			return false;
		}

		void await_suspend(std::coroutine_handle<Task::promise_type> handle) {
			std::thread([handle]() {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				puts("sleep for 1s");
				handle();
				}).detach();
		}

		void await_resume() {
		}
	};
} // namespace Coroutine
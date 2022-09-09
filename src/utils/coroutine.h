#include <iostream>
#include <coroutine>
#include <thread>

namespace Coroutine {
    struct task {
        struct promise_type {
            promise_type() {
                std::cout << "1.task-promise_type():create promise object\n";
            }

            task get_return_object() {
                std::cout << "2.task-get_return_object():create coroutine return object, and the coroutine is created now\n";
                return { std::coroutine_handle<task::promise_type>::from_promise(*this) };
            }

            // initial_suspend()����Э�̳�ʼ�����Ǽ���ֱ�Ӽ���ִ��Э�̣����ǹ���Э�̷���caller
            // ����std::suspend_never����ʾ������Э�̣������ִ��Э�̺�����(coroutine body)
            // ����std::suspend_always����ʾ����Э�̣�����ȥִ��coroutine body�������ִ�з��ص�caller����
            std::suspend_never initial_suspend() {
                std::cout << "3.task-initial_suspend():do you want to susupend the current coroutine?\n";
                std::cout << "4.task-initial_suspend():don't suspend because return std::suspend_never, so continue to execute coroutine body\n";
                return {};
            }

            // ������void return_void()����void return_value(T v)�󣬾ͻ����final_suspend()
            // ���final_suspend����std::suspend_never��ʾ������Э�̣���ôЭ�̾ͻ��Զ����٣��Ⱥ�����promise, Э��֡�ϵò�����Э��֡��
            // �������std::suspend_always�򲻻��Զ�����Э�̣���Ҫ�û��ֶ�ȥɾ��Э�̡�
            std::suspend_never final_suspend() noexcept {
                std::cout << "15.task-final_suspend():coroutine body finished, do you want to susupend the current coroutine?\n";
                std::cout << "16.task-final_suspend():don't suspend because return std::suspend_never, and the continue will be automatically destroyed, bye\n";
                return {};
            }

            // ���Э����voidû�з���ֵ����ô����Ҫ����void return_void()
            // ����з���ֵ��ô�Ͷ���void return_value(T v),��������Э�̵ķ���ֵ
            // return_value����return_void������������ֻ�������һ��
            void return_void() {
                std::cout << "14.task-return_void():coroutine don't return value, so return_void is called\n";
            }

            void unhandled_exception() {}
        };

        std::coroutine_handle<task::promise_type> handle_;
    };

    struct awaiter {
        // ����co_wait awaiter{};ʱ����await_ready()
        // ��ʾ�Ƿ�׼���ã�Ҫ��Ҫ����Э��
        // await_ready()����falseһ���ʾҪ����Э�̣���ִ��await_suspend
        // ����true˵��Э���Ѿ�ִ�����ˣ���ʱ�����await_resume����Э�̵Ľ����
        bool await_ready() {
            std::cout << "6.await_ready():do you want to suspend current coroutine?\n";
            std::cout << "7.await_ready():yes, suspend becase awaiter.await_ready() return false\n";
            return false;
        }

        //await_suspend �ķ���ֵ�������Ƿ��� caller ���Ǽ���ִ�С�
        //����void��Э��ִ��Ȩ��������ǰЭ�̵�caller����ǰЭ����δ��ĳ��ʱ����resume֮��Ȼ��ִ��Э�̺�����co_await��������
        //����true��ͬ����void��
        //����false��ֱ��ִ��await_resume
        void await_suspend(std::coroutine_handle<task::promise_type> handle) {
            std::cout << "8.await_suspend(std::coroutine_handle<task::promise_type> handle):execute awaiter.await_suspend()\n";
            std::thread([handle]() mutable {
                std::cout << "11.lambada():resume coroutine to execute coroutine body\n";
                handle();//�ȼ���handle.resume();
                std::cout << "17.lambada():over\n";
                }).detach();
                std::cout << "9.await_suspend(std::coroutine_handle<task::promise_type> handle):a new thread lauched, and will return back to caller\n";
        }

        //������await_resume��ֱ��ִ��Э�̺�����co_await��������
        void await_resume() {
            std::cout << "12.await_resume()\n";
        }
    };

    task test() {
        std::cout << "5.test():begin to execute coroutine body, the thread id=" << std::this_thread::get_id() << ",and call co_await awaiter{};\n"; //#1
        co_await awaiter{};
        std::cout << "13.test():coroutine resumed, continue execute coroutine body now, the thread id=" << std::this_thread::get_id() << "\n"; //#3
    }

    template<typename T>
    struct lazy {
    public:
        struct promise_type;

        lazy(std::coroutine_handle<promise_type> handle) : m_handle(handle) {
            std::cout << "3.lazy(std::coroutine_handle<promise_type> handle):Construct a lazy object" << std::endl;
        }

        ~lazy() {
            std::cout << "15.~lazy():Destruct a lazy object " << std::endl;
            m_handle.destroy();
        }

        T get() {
            std::cout << "6.lazy.get():I want to execute the coroutine now. call m_handle.resume()" << std::endl;
            if (!m_handle.done()) {
                m_handle.resume();
            }
            std::cout << "13.lazy.get():We got the return value...:" << m_handle.promise().value << std::endl;
            return m_handle.promise().value;
        }

        struct promise_type {
            T value = {};

            promise_type() {
                std::cout << "1.lazy-promise_type():Promise created" << std::endl;
            }

            ~promise_type() {
                std::cout << "16.lazy- ~promise_type():Promise died" << std::endl;
            }

            auto get_return_object() {
                std::cout << "2.lazy-get_return_object():create coroutine return object, and the coroutine is created now" << std::endl;
                return lazy<T>{std::coroutine_handle<promise_type>::from_promise(*this)};
            }

            auto initial_suspend() {
                std::cout << "4.lazy-initial_suspend():Started the coroutine" << std::endl;
                return std::suspend_always{};
            }

            auto final_suspend() noexcept {
                std::cout << "12.lazy-final_suspend():Finished the coroutine" << std::endl;
                return std::suspend_always{};
            }

            void return_value(T v) {
                std::cout << "11.lazy-return_value(T v):Got coroutine result " << v << std::endl;
                value = v;
            }

            void unhandled_exception() {
                std::exit(1);
            }

            //Э�����е���co_yield xxx;��ʱ�����yield_value(T val)
            auto yield_value(T val) {
                std::cout << "9.lazy-yield_value(T val): " << val << std::endl;
                value = val;

                //�������ٹ���Э�̣�����ִ��
                return std::suspend_never();

                //                //������������Э��
                //                return std::suspend_always();
            }

        };

        std::coroutine_handle<promise_type> m_handle;
    };

    lazy<int> my_coroutine() {
        std::cout << "7.my_coroutine():Execute the coroutine function body" << std::endl;
        std::cout << "8.my_coroutine():call---co_yield 66;" << std::endl;
        co_yield 66;
        std::cout << "10.my_coroutine():call---co_return 88;" << std::endl;
        co_return 88;
    }
} // namespace Coroutine
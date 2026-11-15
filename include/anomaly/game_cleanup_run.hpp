#pragma once
#include "anomaly/sdk/services/game_cleanup.h"
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace anomaly {
// No plugin or world pointers are owned by this coordinator. The caller must
// retain the plugin generation if Run returns anything other than Complete.
class GameCleanupRun final {
public:
    using Clock = std::chrono::steady_clock;
    using Post = std::function<bool(std::function<void()>)>;
    using Invoke = std::function<uint32_t(uint32_t, uint32_t)>;
    enum class Result { Complete, Failed, TimedOut, DispatcherUnavailable };

    static Result Run(const Post& post, Invoke invoke, Clock::time_point deadline) {
        if (!post) return Result::DispatcherUnavailable;
        struct State {
            std::mutex mutex;
            std::condition_variable wake;
            bool closed{}, done{};
            uint32_t result{ANOMALY_GAME_CLEANUP_V1_FAILED};
        };
        auto state = std::make_shared<State>();
        for (uint32_t attempt = 1;; ++attempt) {
            if (Clock::now() >= deadline) {
                std::lock_guard lock(state->mutex);
                state->closed = true;
                return Result::TimedOut;
            }
            {
                std::lock_guard lock(state->mutex);
                state->done = false;
            }
            bool accepted{};
            try {
                accepted = post([state, invoke, deadline, attempt] {
                    {
                        std::lock_guard lock(state->mutex);
                        if (state->closed || Clock::now() >= deadline) return;
                    }
                    uint32_t result = ANOMALY_GAME_CLEANUP_V1_FAILED;
                    const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                        deadline - Clock::now()).count();
                    if (remaining > 0) {
                        try { result = invoke(static_cast<uint32_t>(remaining), attempt); }
                        catch (...) { result = ANOMALY_GAME_CLEANUP_V1_FAILED; }
                    }
                    {
                        std::lock_guard lock(state->mutex);
                        // A late COMPLETE never reverses an already returned timeout.
                        if (state->closed) return;
                        state->result = result;
                        state->done = true;
                    }
                    state->wake.notify_one();
                });
            } catch (...) { accepted = false; }
            std::unique_lock lock(state->mutex);
            if (!accepted) {
                state->closed = true;
                return Result::DispatcherUnavailable;
            }
            if (!state->wake.wait_until(lock, deadline, [&] { return state->done; }) ||
                Clock::now() >= deadline) {
                state->closed = true;
                return Result::TimedOut;
            }
            if (state->result != ANOMALY_GAME_CLEANUP_V1_PENDING) {
                state->closed = true;
                return state->result == ANOMALY_GAME_CLEANUP_V1_COMPLETE
                    ? Result::Complete : Result::Failed;
            }
            lock.unlock();
            // Backoff occurs on Lifecycle, never on the Game dispatcher.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
};
} // namespace anomaly

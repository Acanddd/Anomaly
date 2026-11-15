#pragma once
#include "anomaly/thread_local_value.hpp"
#include <algorithm>
#include <mutex>
#include <optional>
#include <vector>
namespace anomaly {
struct IndependentWindowOwner {
    uint64_t handle{}, generation{};
    bool operator==(const IndependentWindowOwner&) const = default;
};
inline ThreadLocalObject<IndependentWindowOwner> g_independent_window_owner;
class ScopedIndependentWindowOwner final {
    IndependentWindowOwner old_;
public:
    explicit ScopedIndependentWindowOwner(IndependentWindowOwner value)
        : old_(g_independent_window_owner.Get()) { g_independent_window_owner.Get() = value; }
    ~ScopedIndependentWindowOwner() { g_independent_window_owner.Get() = old_; }
};
// All state is host-owned; no ImGui objects cross threads. Resource revocation
// removes both published and pending bounds, including a drag/focus owned by A
// while an unrelated B remains open.
class IndependentWindowInput final {
public:
    struct Rect { float left, top, right, bottom; IndependentWindowOwner owner{}; };
    void BeginFrame() { std::lock_guard lock(mutex_); pending_.clear(); pending_focus_.reset(); }
    void Add(Rect rect) {
        std::lock_guard lock(mutex_);
        if (rect.right > rect.left && rect.bottom > rect.top) pending_.push_back(rect);
    }
    void RequestFocus(Rect rect) { std::lock_guard lock(mutex_); pending_focus_ = rect; }
    void EndFrame() {
        std::lock_guard lock(mutex_); published_ = pending_; focus_ = pending_focus_;
        if (drag_ && std::none_of(published_.begin(),published_.end(),[&](const Rect& r){return r.owner==*drag_;})) drag_.reset();
        hit_ = HitLocked(x_,y_);
    }
    std::optional<Rect> ConsumeFocus(bool foreground) {
        std::lock_guard lock(mutex_); auto result = focus_; focus_.reset();
        return foreground ? result : std::nullopt;
    }
    bool Contains(float x, float y) const { std::lock_guard lock(mutex_); return HitLocked(x,y).has_value(); }
    bool Any() const { std::lock_guard lock(mutex_); return !published_.empty(); }
    bool Captured() const { std::lock_guard lock(mutex_); return hit_.has_value() || drag_.has_value(); }
    void Pointer(float x,float y) { std::lock_guard lock(mutex_); x_=x;y_=y;hit_=HitLocked(x,y); }
    void Button(bool down) { std::lock_guard lock(mutex_); if(!down)drag_.reset(); else if(hit_)drag_=hit_; }
    void ResetPointer() { std::lock_guard lock(mutex_); hit_.reset();drag_.reset();x_=y_=-1e30F; }
    void Remove(IndependentWindowOwner owner) {
        std::lock_guard lock(mutex_);
        const auto match=[&](const Rect&r){return r.owner==owner;};
        std::erase_if(pending_,match);std::erase_if(published_,match);
        if(pending_focus_&&match(*pending_focus_))pending_focus_.reset();
        if(focus_&&match(*focus_))focus_.reset();
        if(hit_&&*hit_==owner)hit_.reset();if(drag_&&*drag_==owner)drag_.reset();
    }
    void Clear() {
        std::lock_guard lock(mutex_);pending_.clear();published_.clear();
        pending_focus_.reset();focus_.reset();hit_.reset();drag_.reset();x_=y_=-1e30F;
    }
private:
    std::optional<IndependentWindowOwner> HitLocked(float x,float y) const {
        for(auto it=published_.rbegin();it!=published_.rend();++it)
            if(x>=it->left && y>=it->top && x<it->right && y<it->bottom)return it->owner;
        return std::nullopt;
    }
    mutable std::mutex mutex_;
    std::vector<Rect> pending_,published_;
    std::optional<Rect> pending_focus_,focus_;
    std::optional<IndependentWindowOwner> hit_,drag_;
    float x_{-1e30F},y_{-1e30F};
};
inline IndependentWindowInput g_independent_windows;
inline IndependentWindowInput& IndependentWindows() { return g_independent_windows; }
} // namespace anomaly

#pragma once
#include <cstdint>

namespace anomaly {
// Only a completed reconcile consumes a registry revision. A mutation during
// snapshot construction remains dirty on the next Update, rather than lost.
struct NteEscMenuRevisionGate {
    uint64_t completed_revision{};
    bool Changed(uint64_t revision) const noexcept { return revision != completed_revision; }
    void Complete(uint64_t revision) noexcept { completed_revision = revision; }
    template<class Reconcile>
    void Run(uint64_t revision, bool& complete, Reconcile reconcile) {
        // Even a same-revision click/page request starts a fresh pass. An early
        // return while a rebuilt container stabilizes must not inherit success.
        complete = false;
        reconcile();
        if (complete) Complete(revision);
    }
};

// Return false without discarding ownership when the engine refuses removal.
// The caller must not append replacements until this returns true.
template<class Bindings, class Retain, class ChildAt, class Remove>
bool RemoveRetiredEscMenuBindings(Bindings& bindings, int32_t& children,
    Retain retain, ChildAt child_at, Remove remove) {
    bool complete = true;
    for (auto binding = bindings.begin(); binding != bindings.end();) {
        if (retain(binding->handle)) { ++binding; continue; }
        const auto index = binding->child_index;
        if (child_at(index) != binding->widget || !remove(index)) {
            complete = false;
            ++binding;
            continue;
        }
        --children;
        for (auto& entry : bindings) if (entry.child_index > index) --entry.child_index;
        binding = bindings.erase(binding);
    }
    return complete;
}
}

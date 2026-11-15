# Experimental Game cleanup service v1

Local research patch on Anomaly v3.0.0. Not an upstream-supported API.

`anomaly.game-cleanup` uses the existing `game-events` capability. Declare it as
an optional service to retain compatibility with unmodified hosts. Never enable
game mutations unless query and registration succeed. The complete C ABI is in
`include/anomaly/sdk/services/game_cleanup.h`; no existing ABI struct changes.

Register one callback per plugin generation in on_load/on_start. The registry is
scope-owned. Unregister only while active and after the plugin has already cleaned
its game resources. During stopping, the host owns retirement; on_stop must not
unregister this callback. An unavailable Game dispatcher returns UNAVAILABLE.

Stopping freezes ordinary callback sources, cancels ordinary queued work and
drains ordinary leases before dispatching cleanup on a separate host Game queue.
The cleanup callback has a lifecycle lease but Game thread affinity; ordinary
callbacks never resume. Game services may be used, while synchronous state I/O
remains forbidden by its Game-domain restriction. Do not register new work.

Return COMPLETE only after all game ownership is cleared. PENDING requests another
bounded attempt, not necessarily a new simulation frame. Use game state or tick
sequence when a transition requires a new frame. The host shares one fixed stop
deadline across attempts and the subsequent on_stop. The coordinator releases
the host execution mutex while waiting; it retains lifecycle serialization.

The context contains the current `ue5.world` handle only if available. UNAVAILABLE
does not prove that an old world or its actors have been destroyed. Never
dereference cached pointers across world transitions. This version deliberately
does not invent a world-invalidation proof: the plugin must separately validate
its old handles or remain pending/fail. Global shared modifications do not vanish
with a world. `stop_request_id` is the unique generation being retired.

Only COMPLETE proceeds to on_stop and normal unload. Rejection, failure, exception,
no verified Game thread, no Game pump or deadline expiry quarantines the generation.
Already-entered callbacks retain their lease and module. Late queued callbacks do
not execute after the request closes; late completion does not reverse a timeout.
Faulted plugin contexts are not reentered for cleanup. Existing quarantine prevents
replacement activation for the same plugin identity.

Production dispatch rechecks `ue5.framework.is_game_thread`; there is no Worker or
Lifecycle fallback. In a full runtime shutdown where Game pumping has already
ceased, this service may quarantine instead of cleanly unload. That is deliberate
failure isolation, not a successful full shutdown or in-game cleanup guarantee.

Offline fixtures are in `.local/game_cleanup` and are not release payload. They
verify the C ABI, actual scope drain/lease behavior, dispatcher affinity, pending
attempts, cancellation, deadlines and integration with a real plugin DLL. Game
entity destruction and CN/Global play acceptance remain separate requirements.

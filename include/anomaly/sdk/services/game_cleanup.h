#pragma once
#include "anomaly/sdk/base.h"

#define ANOMALY_GAME_CLEANUP_SERVICE_V1_ID "anomaly.game-cleanup"
#define ANOMALY_GAME_CLEANUP_SERVICE_V1_VERSION 1u

#ifdef __cplusplus
extern "C" {
#endif

typedef enum AnomalyGameCleanupResultV1 {
    ANOMALY_GAME_CLEANUP_V1_COMPLETE = 0,
    ANOMALY_GAME_CLEANUP_V1_PENDING = 1,
    ANOMALY_GAME_CLEANUP_V1_FAILED = 2
} AnomalyGameCleanupResultV1;

typedef enum AnomalyGameCleanupWorldStateV1 {
    ANOMALY_GAME_CLEANUP_WORLD_V1_UNAVAILABLE = 0,
    ANOMALY_GAME_CLEANUP_WORLD_V1_AVAILABLE = 1
} AnomalyGameCleanupWorldStateV1;

typedef struct AnomalyGameCleanupContextV1 {
    uint32_t struct_size;
    uint32_t remaining_milliseconds;
    uint64_t plugin_generation;
    uint64_t stop_request_id;
    AnomalyGenerationHandleV1 world;
    uint32_t world_state;
    uint32_t attempt;
} AnomalyGameCleanupContextV1;

// Called only on the verified Game dispatcher after ordinary callbacks drain.
// Never dereference a cached UE pointer just because a world is unavailable.
// UNAVAILABLE is not evidence that the old world's owned resources are gone.
// COMPLETE confirms all owned game mutations have been cleaned; PENDING asks
// for another bounded attempt. Callbacks must not block waiting for a new frame.
typedef uint32_t (ANOMALY_CALL *AnomalyGameCleanupCallbackV1)(
    void* user, const AnomalyGameCleanupContextV1* context);

// Optional service; uses the existing game-events capability. Old hosts return
// UNAVAILABLE. Register once in on_load/on_start before enabling mutations.
// Starting stop freezes registration; missing Game pumping or timeout retains
// the generation in quarantine and never invokes cleanup on a fallback thread.
typedef struct AnomalyGameCleanupServiceV1 {
    uint32_t struct_size;
    uint32_t service_version;
    void* user;
    AnomalyStatusV1 (ANOMALY_CALL *register_cleanup)(
        void* user, AnomalyGameCleanupCallbackV1 callback, void* callback_user,
        AnomalyGenerationHandleV1* handle);
    AnomalyStatusV1 (ANOMALY_CALL *unregister_cleanup)(
        void* user, AnomalyGenerationHandleV1 handle);
} AnomalyGameCleanupServiceV1;
#ifdef __cplusplus
}
#endif

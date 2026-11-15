#pragma once
#include "anomaly/sdk/base.h"
#define ANOMALY_WINDOW_PRESENTATION_SERVICE_V1_ID "anomaly.window-presentation"
#define ANOMALY_WINDOW_PRESENTATION_SERVICE_V1_VERSION 1u
#ifdef __cplusplus
extern "C" {
#endif
// Optional, scope-owned presentation policy. Does not change open state.
// Only registered owned windows are accepted. Not persisted across generations.
typedef struct AnomalyWindowPresentationServiceV1 {
    uint32_t struct_size;
    uint32_t service_version;
    void* user;
    AnomalyStatusV1 (ANOMALY_CALL *set_independent)(
        void* user, AnomalyGenerationHandleV1 window, int32_t enabled);
} AnomalyWindowPresentationServiceV1;
#ifdef __cplusplus
}
#endif

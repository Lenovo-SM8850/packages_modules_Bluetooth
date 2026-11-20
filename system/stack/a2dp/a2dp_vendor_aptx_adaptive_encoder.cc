/******************************************************************************
    Copyright (c) 2018, The Linux Foundation. All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted (subject to the limitations in the
    disclaimer below) provided that the following conditions are met:

       * Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.

       * Redistributions in binary form must reproduce the above
         copyright notice, this list of conditions and the following
         disclaimer in the documentation and/or other materials provided
         with the distribution.

       * Neither the name of The Linux Foundation nor the names of its
         contributors may be used to endorse or promote products derived
         from this software without specific prior written permission.

    NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
    GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
    HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
    GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
    IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
    IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#define LOG_TAG "a2dp_vendor_aptx_adaptive_encoder"

#include "stack/include/a2dp_vendor_aptx_adaptive_encoder.h"

#include <dlfcn.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <cmath>

#include "stack/include/a2dp_vendor.h"
#include "stack/include/a2dp_vendor_aptx_adaptive.h"

typedef struct {
  uint64_t session_start_us;

  size_t media_read_total_expected_packets;
  size_t media_read_total_expected_reads_count;
  size_t media_read_total_expected_read_bytes;

  size_t media_read_total_dropped_packets;
  size_t media_read_total_actual_reads_count;
  size_t media_read_total_actual_read_bytes;
} a2dp_aptx_adaptive_encoder_stats_t;

typedef struct {
  a2dp_source_read_callback_t read_callback;
  a2dp_source_enqueue_callback_t enqueue_callback;

  bool use_SCMS_T;
  bool is_peer_edr;          // True if the peer device supports EDR
  bool peer_supports_3mbps;  // True if the peer device supports 3Mbps EDR
  uint16_t peer_mtu;         // // MTU of the A2DP peer
  uint32_t timestamp;        // Timestamp for the A2DP frames

  a2dp_aptx_adaptive_encoder_stats_t stats;

} tA2DP_APTX_ADAPTIVE_ENCODER_CB;

static tA2DP_APTX_ADAPTIVE_ENCODER_CB a2dp_aptx_adaptive_encoder_cb;

bool A2DP_VendorLoadEncoderAptxAdaptive(void) { return true; }

void A2DP_VendorUnloadEncoderAptxAdaptive(void) {}

void a2dp_vendor_aptx_adaptive_encoder_init(const tA2DP_ENCODER_INIT_PEER_PARAMS* /* p_peer_params */,
                                            A2dpCodecConfig* /* a2dp_codec_config */,
                                            a2dp_source_read_callback_t /* read_callback */,
                                            a2dp_source_enqueue_callback_t /* enqueue_callback */) {}

void a2dp_vendor_aptx_adaptive_feeding_reset(void) {}

void a2dp_vendor_aptx_adaptive_feeding_flush(void) {}

uint64_t a2dp_vendor_aptx_adaptive_get_encoder_interval_ms(void) { return 0; }

int a2dp_vendor_aptx_adaptive_get_effective_frame_size() { return 0; }

void a2dp_vendor_aptx_adaptive_send_frames(uint64_t /* timestamp_us */) {}

void a2dp_vendor_aptx_adaptive_encoder_cleanup(void) {
  memset(&a2dp_aptx_adaptive_encoder_cb, 0, sizeof(a2dp_aptx_adaptive_encoder_cb));
}

void A2dpCodecConfigAptxAdaptive::debug_codec_dump(int fd) {
  a2dp_aptx_adaptive_encoder_stats_t* stats = &a2dp_aptx_adaptive_encoder_cb.stats;

  A2dpCodecConfig::debug_codec_dump(fd);

  dprintf(fd,
          "  Packet counts (expected/dropped)                        : %zu / "
          "%zu\n",
          stats->media_read_total_expected_packets, stats->media_read_total_dropped_packets);

  dprintf(fd,
          "  PCM read counts (expected/actual)                       : %zu / "
          "%zu\n",
          stats->media_read_total_expected_reads_count, stats->media_read_total_actual_reads_count);

  dprintf(fd,
          "  PCM read bytes (expected/actual)                        : %zu / "
          "%zu\n",
          stats->media_read_total_expected_read_bytes, stats->media_read_total_actual_read_bytes);
}

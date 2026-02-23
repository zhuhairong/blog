#include "mqtt_pkt.h"
#include <string.h>

size_t mqtt_pkt_connect(uint8_t *buf, const char *client_id) {
    uint8_t *p = buf;
    *p++ = 0x10; // Connect
    size_t id_len = strlen(client_id);
    size_t rem_len = 10 + 2 + id_len;
    *p++ = (uint8_t)rem_len;
    *p++ = 0x00; *p++ = 0x04; memcpy(p, "MQTT", 4); p += 4;
    *p++ = 0x04; // Level
    *p++ = 0x02; // Flags (Clean Session)
    *p++ = 0x00; *p++ = 0x3C; // Keep Alive (60s)
    *p++ = (uint8_t)(id_len >> 8); *p++ = (uint8_t)(id_len & 0xFF);
    memcpy(p, client_id, id_len); p += id_len;
    return p - buf;
}

size_t mqtt_pkt_publish(uint8_t *buf, const char *topic, const char *payload) {
    uint8_t *p = buf;
    *p++ = 0x30; // Publish
    size_t t_len = strlen(topic);
    size_t p_len = strlen(payload);
    size_t rem_len = 2 + t_len + p_len;
    *p++ = (uint8_t)rem_len;
    *p++ = (uint8_t)(t_len >> 8); *p++ = (uint8_t)(t_len & 0xFF);
    memcpy(p, topic, t_len); p += t_len;
    memcpy(p, payload, p_len); p += p_len;
    return p - buf;
}

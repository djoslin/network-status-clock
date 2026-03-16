# Network Status Clock

A network-monitoring clock built on the [Info-Orbs](https://brett.tech/collections/electronics-projects/products/info-orbs-full-dev-kit) hardware platform (ESP32 + five round GC9A01 240×240 displays). It displays the current time across the five screens and continuously monitors internet connectivity, changing color to indicate outages and recovery.

## Features

- **Clock display** — Hours and minutes shown as large DSEG7 segment-style digits across four screens, with a flashing colon on the center screen. Supports 12h/24h mode with AM/PM indicator.
- **Network monitoring** — Periodically checks internet connectivity by reaching Google DNS (8.8.8.8:53). Distinguishes between internet-down and WiFi-down by also probing the local router.
- **Outage indication** — Clock turns red when internet is down. The center screen shows downtime (hours and minutes stacked).
- **Recovery indication** — Clock turns green when connectivity is restored, showing total downtime duration for a configurable period before returning to normal.
- **Flap detection** — Requires multiple consecutive failures before declaring an outage (and multiple consecutive successes before declaring recovery) to avoid false alarms from transient drops.
- **OTA updates** — Supports over-the-air firmware updates after initial USB flash.
- **Nighttime dimming** — Optional automatic brightness reduction on a schedule.

## Hardware

This project is designed for the **Info-Orbs Full Dev Kit** available from:
https://brett.tech/collections/electronics-projects/products/info-orbs-full-dev-kit

The kit includes an ESP32 DOIT DevKit V1, five GC9A01 round LCD displays, and three buttons.

## Setup

### 1. Install PlatformIO

Install [PlatformIO](https://platformio.org/) (either the CLI or the VS Code extension).

### 2. Create your config file

Copy the template configuration and edit it with your settings:

```bash
cp firmware/config/config.h.template firmware/config/config.h
```

Edit `firmware/config/config.h` to configure:

- **WiFi** — Uncomment and set `WIFI_SSID` and `WIFI_PASS`, or leave them commented to use the WiFiManager captive portal (connect to the `Info-Orbs_XX` access point and configure via 192.168.4.1).
- **Timezone** — Set `TIMEZONE_API_LOCATION` to your timezone (see [timezonedb.com/time-zones](https://timezonedb.com/time-zones)).
- **Clock preferences** — 12h/24h mode, AM/PM indicator, colors, shadow style, etc.
- **Network monitoring** — Check interval, recovery display duration, and flap detection thresholds.
- **Dimming** — Optionally uncomment and set `DIM_START_HOUR`, `DIM_END_HOUR`, and `DIM_BRIGHTNESS`.

> **Note:** `config.h` is gitignored to keep your credentials out of version control. Only `config.h.template` is tracked.

### 3. Build and flash via USB

Connect the ESP32 via USB and run:

```bash
pio run -e esp32doit -t upload
```

### 4. Subsequent updates via OTA

Once connected to WiFi, you can flash over the air:

```bash
pio run -e esp32doit-ota -t upload
```

The device advertises as `network-status-clock.local` on the network.

## Buttons

| Button | Pin | Short Press |
|--------|-----|-------------|
| Left   | G26 | Toggle 12h/24h mode |
| Middle | G27 | Toggle IP address display on center screen |
| Right  | G14 | Toggle network status preview (green debug view) |

## Configuration Reference

Key settings in `config.h`:

| Setting | Default | Description |
|---------|---------|-------------|
| `FORMAT_24_HOUR` | `false` | Use 24-hour clock format |
| `SHOW_AM_PM_INDICATOR` | `false` | Show AM/PM on center screen (12h mode) |
| `CLOCK_COLOR` | `0xfc80` (orange) | Normal clock color |
| `CLOCK_COLOR_NET_DOWN` | `0xf800` (red) | Clock color during outage |
| `CLOCK_COLOR_NET_UP` | `0x07e0` (green) | Clock color after recovery |
| `CLOCK_NET_CHECK_INTERVAL_MS` | `30000` | Connectivity check interval (ms) |
| `CLOCK_NET_RECOVERY_MS` | `900000` | How long to show green after recovery (ms) |
| `CLOCK_NET_FAIL_THRESHOLD` | `3` | Consecutive failures to declare outage |
| `CLOCK_NET_RECOVER_THRESHOLD` | `3` | Consecutive successes to declare recovery |

## License

This project is derived from [Info-Orbs](https://github.com/brett-dot-tech/info-orbs) by brett.tech.

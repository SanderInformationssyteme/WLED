# WLED - Sander Fork

Dieses Repository ist ein Fork von WLED mit angepassten Usermods für die Sander LED-Installation (122 LEDs).

## Features

### Sander Logo Chase (Background Layer)
- Läuft kontinuierlich im Hintergrund
- Zeigt Sander Logo-Farben (Orange, Grün, Blau) auf LEDs 84-99
- Wechselt zwischen statischer Anzeige und Wipe-Effekten
- **Wichtig**: Andere Segmente haben automatisch Vorrang - Logo Chase zeichnet nur auf freien LEDs

## Hardware Setup

- **Board**: ESP32 Dev Module
- **LED Count**: 122 LEDs (WS2812B oder kompatibel)
- **LED Layout**:
  - LEDs 84-89: Sander Orange (RGB 255,218,51)
  - LEDs 89-94: Sander Green (RGB 56,255,215)
  - LEDs 94-99: Sander Blue (RGB 128,170,255)
  - Übrige LEDs: Dynamische Hintergrund-Animation

## Build-Anleitung

### Voraussetzungen
- Node.js 20+ (siehe `.nvmrc`)
- Python 3.x
- PlatformIO

### Installation

1. **Repository klonen**
   ```bash
   git clone <repository-url>
   cd WLED
   ```

2. **Node.js Abhängigkeiten installieren**
   ```bash
   npm ci
   ```

3. **Python Abhängigkeiten installieren**
   ```bash
   pip install -r requirements.txt
   ```

### Build-Prozess

**WICHTIG**: Die Web UI muss **VOR** der Firmware gebaut werden!

#### 1. Web UI bauen (erforderlich)
```bash
npm run build
```
- Dauer: ~3 Sekunden
- Generiert `wled00/html_*.h` Dateien

#### 2. Firmware kompilieren
```bash
python -m platformio run -e esp32dev_sander
```
- Dauer: ~15-20 Minuten (erster Build, danach schneller durch Caching)
- Output: `build_output/release/WLED_0.16.0-alpha_ESP32.bin`

#### 3. Firmware hochladen
```bash
python -m platformio run -e esp32dev_sander --target upload
```

### Schnell-Build (beide Schritte)
```bash
npm run build && python -m platformio run -e esp32dev_sander
```

## Entwicklung

### Tests ausführen
```bash
npm test
```
- Validiert das Build-System
- Muss vor jedem Commit laufen und erfolgreich sein

### Development Mode (Web UI)
```bash
npm run dev
```
- Überwacht Dateiänderungen und baut Web UI automatisch neu

### Verfügbare PlatformIO Environments
- `esp32dev_sander` - ESP32 mit Sander-spezifischen Usermods (empfohlen)

## Konfiguration

Die Sander-spezifischen Einstellungen können über die Web UI angepasst werden:
- **Duration**: Wie lange die statische Anzeige gehalten wird (Standard: 10 Sekunden)
- **Wipe Delay**: Geschwindigkeit der Wipe-Effekte (Standard: 30ms)
- **LED Count**: Gesamtanzahl der LEDs (Standard: 122)

Einstellungen werden in `cfg.json` gespeichert.

## Verwendung

### Segmente erstellen
Du kannst jederzeit neue Segmente mit Effekten erstellen:

**Via Web UI:**
- Segments → Add Segment
- Start/Stop LEDs festlegen
- Effekt auswählen
- Logo Chase läuft automatisch im Hintergrund weiter

**Via JSON API:**
```bash
curl -X POST http://<ESP32-IP>/json/state \
  -H "Content-Type: application/json" \
  -d '{"seg":[{"start":0,"stop":50,"fx":9}]}'
```

Der Logo Chase respektiert alle aktiven Segmente und zeichnet nur auf freien LEDs.

### Segmente löschen
- Via Web UI: Segment auswählen → Delete
- Via API: Segment auf `stop: 0` setzen

Logo Chase übernimmt automatisch wieder die freigegebenen LEDs.

## Troubleshooting

### Build schlägt fehl
1. **Web UI nicht gebaut**: Immer zuerst `npm run build` ausführen
2. **Node.js Version**: Sicherstellen dass Node.js 20+ installiert ist
3. **Cache löschen**: 
   ```bash
   rm -rf wled00/html_*.h
   npm run build
   ```

### PlatformIO Fehler
- **Clean build**: `python -m platformio run -e esp32dev_sander --target clean`
- **Cache löschen**: `.pio` Ordner löschen und neu bauen

### LEDs reagieren nicht
- IP-Adresse des ESP32 überprüfen
- LED-Pin-Konfiguration in Web UI prüfen (Settings → LED Preferences)
- LED Count auf 122 setzen

## Original WLED

Dieser Fork basiert auf [WLED](https://github.com/wled-dev/WLED).

Für allgemeine WLED-Dokumentation siehe:
- [WLED Wiki](https://kno.wled.ge)
- [Original README](readme.md)

# AI Vision Assistive System - Version 2

## Folder Structure

```
v2/
├── android-app/          # Android application code
│   ├── build.gradle      # Project-level build config
│   ├── settings.gradle   # Project settings
│   └── app/
│       ├── build.gradle  # App-level build config
│       └── src/main/
│           ├── AndroidManifest.xml
│           ├── java/com/example/aivision/
│           │   └── MainActivity.java
│           └── res/
│               ├── layout/
│               │   └── activity_main.xml
│               ├── values/
│               │   ├── strings.xml
│               │   └── colors.xml
│               ├── drawable/
│               └── xml/
│                   └── network_security_config.xml
├── esp32/
│   ├── esp32_main.ino           # Main ESP32 code
│   └── esp32_stair_detection.ino # Stair detection code
└── README.md
```

## How to Use
1. Paste your **Android app code** into the files inside `android-app/`
2. Paste your **ESP32 code** into the `.ino` files inside `esp32/`

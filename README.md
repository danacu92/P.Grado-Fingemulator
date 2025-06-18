# 🎮 FingEmulator

## 📘 Español

**FingEmulator** es un prototipo de plataforma de emulación multiconsola, con un enfoque modular, escalable y orientado al juego multijugador online. Fue desarrollado y presentado como parte del proyecto de grado para la obtención del título de **Ingeniero en Computación**.  
El prototipo fue desarrollado para arquitecturas **x64** con sistema operativo **Windows**.

---

### 🧩 Arquitectura de la plataforma

![Screenshot_2](https://github.com/user-attachments/assets/026bfb09-2607-4e4c-a2db-548ae69749c7)

---

### 🧩 Componentes del sistema

El proyecto está compuesto por tres módulos:

- **FingEmulator**  
  Módulo donde se implementa toda la plataforma de emulación, incluyendo el emulador de NES. Su arquitectura modular permite agregar múltiples consolas en el futuro con pocos cambios.

- **FingEmulatorAPI**  
  Interfaz que permite manejar la plataforma programáticamente. Útil, por ejemplo, para proyectos de Inteligencia Artificial que aprendan a jugar juegos retro.

- **TestAPI**  
  Proyecto de ejemplo que muestra cómo utilizar la FingEmulatorAPI.

---

## 🧠 Características principales

- Emulador de **Nintendo Entertainment System (NES)** implementado desde cero e integrado a la plataforma. Soporta juegos con Mapper 0, 2, 3 y 4.
- Arquitectura preparada para agregar más consolas en el futuro y reutilizar componentes de hardware ya implementados.
- **Soporte multijugador online** utilizando rollback netcode mediante la librería **GGPO**. *(El modo online solo está disponible para juegos con Mapper 0, ya que el manejo de estados aún no fue implementado para el resto de los mappers).*
- Soporte para GamePads.
- Interfaz gráfica con opciones básicas.
- Opción **Runahead** en estado experimental para reducir input lag.

---

## 🛠️ Tecnologías utilizadas

- **C++**
- **SDL 2.0** – Manejo de gráficos, audio e inputs.
- **ImGui** – Interfaz gráfica.
- **GGPO** – Para la implementación del netcode.
- **Visual Studio 2019** – Entorno de desarrollo.
- [Clumsy](https://jagt.github.io/clumsy/) – Para testing del multijugador online.
- [Nes_Snd_Emu: NES Sound Emulator](https://github.com/jamesathey/Nes_Snd_Emu) – Para la emulación del audio NES.

---

## 📦 Dependencias

Todas las dependencias utilizadas son de libre distribución y se incluyen en la carpeta `FingEmulator/deps`.

---

## ⚙️ Requisitos y compilación

1. Clonar o descargar el repositorio.
2. Abrir la solución `.sln` en **Visual Studio 2019**.
3. Seleccionar la configuración **Release** y la plataforma **x64**.
4. Compilar los proyectos en el siguiente orden:
   - `FingEmulatorAPI`
   - `FingEmulator`
   - `TestAPI`

> ⚠️ Es importante respetar el orden de compilación para evitar errores de referencia entre proyectos.

---

## 🚀 Cómo ejecutar `TestAPI`

1. Compilar el proyecto `TestAPI` en modo **Release x64**.
2. Arrastrar una **ROM de NES con Mapper 0** sobre el ejecutable (`TestAPI.exe`).
3. El emulador se iniciará automáticamente y comenzará a ejecutar la ROM.

---

## 🖼️ Capturas de pantalla

![c43ec72d3a35abee22fbe803ae01944ecf75dcc1](https://github.com/user-attachments/assets/4dbd281c-4282-4d32-bea0-678e9017b8cc)  
![Screenshot_1](https://github.com/user-attachments/assets/b7561f47-46ca-46ec-9474-9647181e09ea)

---

## 🌍 Estado del proyecto

Este proyecto fue presentado como trabajo final para la obtención del título de **Ingeniero en Computación** y se considera formalmente concluido.  
Sin embargo, podrá recibir actualizaciones ocasionales para mantenimiento o mejoras puntuales.

---


## 📗 English

**FingEmulator** is a multi-console emulation platform prototype, with a modular and scalable architecture, focused on online multiplayer gaming. It was developed and presented as a graduation project for the **Computer Engineering degree**.  
The prototype was built for **x64 architectures** and **Windows OS**.

---

### 🧩 Platform Architecture

![Screenshot_2](https://github.com/user-attachments/assets/026bfb09-2607-4e4c-a2db-548ae69749c7)

---

### 🧩 System Components

The project consists of three main modules:

- **FingEmulator**  
  Module where the entire emulation platform is implemented, including the NES emulator. Its modular architecture allows new consoles to be added in the future with minimal changes.

- **FingEmulatorAPI**  
  Interface that allows programmatic control of the platform. Useful, for example, in AI projects that learn to play retro games.

- **TestAPI**  
  Example project demonstrating how to use the FingEmulatorAPI.

---

## 🧠 Key Features

- A **Nintendo Entertainment System (NES)** emulator developed from scratch and integrated into the platform. Supports games using Mapper 0, 2, 3, and 4.
- Architecture designed to support more consoles in the future and to reuse existing hardware components.
- **Online multiplayer support** using rollback netcode through the **GGPO** library.  
  *(Online mode is currently available only for Mapper 0 games, since save state management for other mappers has not yet been implemented.)*
- GamePad support.
- Basic graphical interface.
- **Experimental Runahead option** to reduce input lag.

---

## 🛠️ Technologies Used

- **C++**
- **SDL 2.0** – Graphics, audio, and input handling.
- **ImGui** – Graphical user interface.
- **GGPO** – Netcode implementation.
- **Visual Studio 2019** – Development environment.
- [Clumsy](https://jagt.github.io/clumsy/) – Used to test online multiplayer.
- [Nes_Snd_Emu: NES Sound Emulator](https://github.com/jamesathey/Nes_Snd_Emu) – For audio emulation.

---

## 📦 Dependencies

All used dependencies are open-source and included in the `FingEmulator/deps` folder.

---

## ⚙️ Build Instructions

1. Clone or download the repository.
2. Open the `.sln` solution file using **Visual Studio 2019**.
3. Select **Release** configuration and **x64** platform.
4. Build the projects in the following order:
   - `FingEmulatorAPI`
   - `FingEmulator`
   - `TestAPI`

> ⚠️ Building in the correct order is important to avoid reference errors between projects.

---

## 🚀 How to Run `TestAPI`

1. Build the `TestAPI` project in **Release x64** mode.
2. Drag and drop an **NES ROM (Mapper 0)** onto the executable (`TestAPI.exe`).
3. The emulator will start automatically and begin running the ROM.


---

## 🌍 Project Status

This project was presented as a final requirement for the **Computer Engineering degree** and is considered formally completed.  
However, it may receive **occasional updates** for maintenance or minor improvements.
---
## 📙 Deutsch

**FingEmulator** ist ein Prototyp einer Multi-Konsolen-Emulationsplattform mit modularer und skalierbarer Architektur, die auf Online-Mehrspielermodus ausgerichtet ist.  
Das Projekt wurde im Rahmen der Abschlussarbeit für den **ComputerIngeniur** entwickelt und präsentiert.  
Der Prototyp wurde für **x64-Architekturen** und Windows OS.

---

### 🧩 Plattformarchitektur

![Screenshot_2](https://github.com/user-attachments/assets/026bfb09-2607-4e4c-a2db-548ae69749c7)

---

### 🧩 Systemkomponenten

Das Projekt besteht aus drei modulen:

- **FingEmulator**  
   Modul, in dem die gesamte Emulationsplattform einschließlich des NES-Emulators implementiert ist. Die modulare Architektur ermöglicht es, künftig mit minimalem Aufwand weitere Konsolen hinzuzufügen.

- **FingEmulatorAPI**  
  Schnittstelle zur programmgesteuerten Steuerung der Plattform. Nützlich z. B. für KI-Projekte, die lernen, Retrospiele zu spielen.

- **TestAPI**  
  Beispielprojekt, das zeigt, wie die FingEmulatorAPI verwendet wird.

---

## 🧠 Hauptfunktionen

- Ein **Nintendo Entertainment System (NES)**-Emulator, der von Grund auf neu entwickelt und in die Plattform integriert wurde. Unterstützt Spiele mit Mapper 0, 2, 3 und 4.
- Architektur vorbereitet für die zukünftige Integration weiterer Konsolen und Wiederverwendung bereits implementierter Hardwarekomponenten.
- **Online-Mehrspielermodus** mit Rollback-Netcode über die **GGPO**-Bibliothek.  
  *(Der Online-Modus ist derzeit nur für Spiele mit Mapper 0 verfügbar, da das Speichern von Zuständen für andere Mapper noch nicht implementiert wurde.)*
- Unterstützung für GamePads.
- Grafische Benutzeroberfläche mit grundlegenden Optionen.
- **Experimentelle Runahead-Option** zur Reduzierung der Eingabeverzögerung (Input Lag).

---

## 🛠️ Verwendete Technologien

- **C++**
- **SDL 2.0** – Verarbeitung von Grafik, Audio und Inputs.
- **ImGui** – Grafische Benutzeroberfläche.
- **GGPO** – Implementierung des Netcodes.
- **Visual Studio 2019** – Entwicklungsumgebung.
- [Clumsy](https://jagt.github.io/clumsy/) – Für das Testen des Online-Modus.
- [Nes_Snd_Emu: NES Sound Emulator](https://github.com/jamesathey/Nes_Snd_Emu) – Für die NES-Klangerzeugung.

---

## 📦 Abhängigkeiten

Alle verwendeten Abhängigkeiten sind frei verfügbar und im Ordner `FingEmulator/deps` enthalten.

---

## ⚙️ Anforderungen und Kompilierung

1. Repository klonen oder herunterladen.
2. Die `.sln`-Datei mit **Visual Studio 2019** öffnen.
3. **Release**-Konfiguration und **x64**-Plattform auswählen.
4. Die Projekte in folgender Reihenfolge kompilieren:
   - `FingEmulatorAPI`
   - `FingEmulator`
   - `TestAPI`

> ⚠️ Die Kompilierungsreihenfolge ist wichtig, um Referenzfehler zwischen den Projekten zu vermeiden.

---

## 🚀 Ausführen von `TestAPI`

1. Projekt `TestAPI` im Modus **Release x64** kompilieren.
2. Eine **NES-ROM mit Mapper 0** auf die ausführbare Datei (`TestAPI.exe`) ziehen.
3. Der Emulator startet automatisch und beginnt mit der Ausführung der ROM.

---

## 🌍 Projektstatus

Dieses Projekt wurde als Abschlussarbeit für den **ComputerIngeniur** präsentiert und gilt als formell abgeschlossen.  
Es kann jedoch gelegentlich Updates für Wartung oder gezielte Verbesserungen erhalten.

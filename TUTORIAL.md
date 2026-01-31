
# Tutorial – Morse WebUI on Arduino UNO Q

## 1. Introduction

This tutorial explains in detail how the **Morse WebUI project** works internally and how it can be reproduced step by step.  
It is intended for **university students**, educators, and developers who want to understand:

- how the Arduino UNO Q architecture works,
- how a Web interface can control hardware behavior,
- how responsibilities are split between Linux and the MCU,
- how real-time constraints are handled correctly.

The project uses Morse code as a concrete and educational example, but the concepts apply to many other domains.

---

## 2. Arduino UNO Q Architecture Overview

The Arduino UNO Q is composed of two distinct computing domains:

### Linux Core
- Runs a full Linux system
- Executes Python code
- Hosts application-level services (WebUI, HTTP APIs)
- Manages scheduling, threading, and synchronization

### MCU (STM32)
- Executes Arduino sketches
- Handles real-time behavior
- Drives hardware peripherals (buzzer, LEDs, sensors)
- Must remain simple, deterministic, and responsive

These two domains communicate using the **Arduino Bridge**.

---

## 3. Conceptual Architecture of the Project

```
User (Browser)
   |
   | HTTP requests
   v
WebUI (Linux application layer)
   |
   | Python logic + queue + synchronization
   v
Bridge (Linux ↔ MCU)
   |
   | function call with parameters
   v
MCU (STM32)
   |
   | real-time Morse playback
   v
Modulino Buzzer
```

This architecture enforces a clear separation of concerns:
- **Web**: user interaction and visualization
- **Linux/Python**: orchestration and safety
- **MCU**: real-time execution

---

## 4. Data Flow: From Click to Sound

Let us follow the complete path of a Morse message:

1. The user enters text in the Web interface and clicks *Send*.
2. The browser sends an HTTP request:
   ```
   /morse?data=HELLO&speed=17
   ```
3. WebUI routes the request to the Python function `morse()`.
4. Python validates parameters and pushes the request into a queue.
5. The main loop checks if the MCU is idle.
6. Python calls:
   ```python
   bridge.call("morse_play", text, speed)
   ```
7. The MCU receives the parameters.
8. Morse code is generated and played on the buzzer.
9. Python updates the internal status.

---

## 5. Web Layer (HTML / JavaScript)

### 5.1 index.html

The HTML file defines:
- text input
- speed slider
- buttons
- a container for Morse visualization

CSS is used to:
- scale the slider,
- apply consistent colors,
- visually represent dots and dashes.

### 5.2 app.js

The JavaScript code:
- listens to user input events,
- dynamically updates the speed value,
- builds URLs with query parameters,
- sends HTTP requests using `fetch()`,
- renders Morse symbols graphically.

Important design choice:
> The Web layer never talks directly to the MCU.

It only communicates with the Linux-side API.

---

## 6. Python Layer (Linux Side)

### 6.1 Why Python Handles Orchestration

Python is used because:
- it runs comfortably on Linux,
- it supports threading and synchronization,
- it can safely manage queues and shared state.

### 6.2 Request Queue

A queue is used to:
- avoid concurrent MCU access,
- allow multiple browser tabs,
- serialize Morse playback requests.

### 6.3 Thread Safety

A lock protects:
- the queue,
- the global speed value,
- the status structure.

This prevents race conditions.

### 6.4 The Main Loop

The loop:
- checks if the MCU is busy,
- pops a job from the queue,
- calls the MCU via Bridge,
- waits for completion,
- updates status.

This design acts as a **scheduler**.

---

## 7. Bridge Communication

The Arduino Bridge allows:
- calling named functions on the MCU,
- passing parameters,
- receiving acknowledgements.

Example:
```python
bridge.call("morse_play", text, speed)
```

On the MCU side:
```cpp
Bridge.provide("morse_play", morse_play);
```

This creates a clean and explicit interface.

---

## 8. MCU Layer (Arduino / STM32)

### 8.1 Design Philosophy

The MCU:
- does not manage networking,
- does not parse HTTP,
- does not deal with concurrency.

It only:
- receives clean parameters,
- performs real-time work.

### 8.2 Morse Implementation

- A static Morse table maps characters to symbols.
- Speed is converted to time units.
- Dot, dash, and spacing follow standard Morse rules.
- Unsupported characters are ignored safely.

### 8.3 Timing Accuracy

Morse timing:
- dot = 1 unit
- dash = 3 units
- inter-element gap = 1 unit
- inter-letter gap = 3 units
- inter-word gap ≈ 7–8 units

This was validated by ear by a former Morse instructor.

---

## 9. Step-by-Step Experiments

### Step 1 – MCU Test
Upload the sketch.  
Expected result:
- two short beeps at startup.

### Step 2 – Linux Test
Run `main.py`.  
Expected result:
- `/ping` returns `{ "ok": true }`.

### Step 3 – API Test
Open:
```
/morse?data=SOS&speed=15
```
Expected result:
- Morse playback on buzzer.

### Step 4 – Web UI Test
Open `index.html`.  
Expected result:
- visual dots/dashes,
- adjustable speed,
- sound playback.

---

## 10. Common Issues

- No sound: check buzzer wiring and Modulino initialization.
- 404 error: Python WebUI not running.
- Bridge error: function name mismatch.
- No response: MCU not flashed or busy.

---

## 11. Educational Value

After completing this tutorial, students understand:
- layered system design,
- separation of concerns,
- safe concurrency handling,
- real-time vs non-real-time tasks,
- practical Web-to-hardware integration.

---

## 12. Conclusion

This project is not only a Morse code tool.  
It is a **reference educational example** showing how to design clean, maintainable systems on the Arduino UNO Q.

The same architecture can be reused for:
- LED control,
- displays,
- sensors,
- radio interfaces,
- robotics.





# UNO Q – WebUI → Python → Bridge → STM32 (Morse) – Step by step

This document describes **the complete execution path** of a user request,
from the WebUI browser interface to the execution of Morse code on the STM32
using the Modulino Buzzer.

---

## 1. Browser side (WebUI)

### 1.1 User action
The user enters a text (e.g. `COUCOU`) and a speed value (e.g. `12`),
then clicks **Send**.

### 1.2 URL construction
JavaScript builds the following URL:
```
/morse?data=COUCOU&speed=12
```
In a URL, **everything is text**.

### 1.3 HTTP request sent
```
GET /morse?data=COUCOU&speed=12 HTTP/1.1
Host: 192.168.1.50:7000
```

---

## 2. Linux side (Python – WebUI)

### 2.1 WebUI routing
```python
ui.expose_api("GET", "/morse", morse)
```
The request triggers the following call:
```python
morse(data="COUCOU", speed="12")
```

### 2.2 Normalization and validation
- text cleanup
- rejection if empty

### 2.3 Explicit speed conversion
```python
n = int(v)      # "12" → 12
n = clamp(n)    # 5..30
```
The conversion is **not implicit**: it is deliberately performed here.

### 2.4 Enqueueing (_queue)
```python
_queue.append({"txt": "COUCOU", "speed": 12})
```
The HTTP handler responds immediately (JSON),
without waiting for Morse playback to complete.

### 2.5 Threading and synchronization
- `_queue`: FIFO job queue
- `_busy`: prevents concurrent Bridge calls
- `_lock`: protects shared access (HTTP handlers + worker)
- `_last`: current state exposed via `/status`

### 2.6 Worker `loop()`
The worker:
1. dequeues a job if available
2. calls the Bridge
3. updates the state (`sending`, `done`, `error`)

---

## 3. Bridge → STM32

### 3.1 Bridge call
```python
bridge.call("morse_play", txt, speed)
```
At this point, `speed` is **already a Python integer**.

### 3.2 STM32 exposure
```cpp
Bridge.provide("morse_play", morse_play);
```

### 3.3 STM32 function signature
```cpp
String morse_play(String txt, int speed)
```
The Bridge transmits a numeric value, which is received directly as an `int`.

---

## 4. STM32 processing

### 4.1 Message preparation
- text cleanup
- copy to `char[]` buffer
- overflow protection

### 4.2 Morse class + ModulinoBuzzer
The `Morse` class:
- converts each character into dots and dashes
- uses `speed` to control timing
- drives the `ModulinoBuzzer` to produce sound

### 4.3 Execution
```cpp
Morse code(buzzer, message, speed);
code.joue();
```

---

## 5. Architectural summary

- **Web UI**: fast, non-blocking
- **Python WebUI**: validation + queueing
- **Worker**: serialized hardware access
- **Bridge**: Linux → STM32 RPC
- **STM32**: deterministic Morse execution

This model guarantees:
- responsive UI
- no HTTP blocking
- thread-safe behavior
- stable and predictable hardware control

---

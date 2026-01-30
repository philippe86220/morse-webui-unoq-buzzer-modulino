import time
import threading
from arduino.app_utils import App, Bridge
from arduino.app_bricks.web_ui import WebUI

ui = WebUI()
bridge = Bridge()

_queue = []               # liste de dict: {"txt": "...", "speed": int}
_busy = False
_last = {"state": "idle"}

_speed = 17               # vitesse globale par defaut
_lock = threading.Lock()  # protege _speed + _queue + _last (simple et safe)

def clamp_speed(v):
    try:
        n = int(v)
    except Exception:
        return None
    if n < 5: n = 5
    if n > 30: n = 30
    return n

def ping():
    return {"ok": True}

def speed(value=None):
    global _speed
    if value is None:
        with _lock:
            return {"ok": True, "speed": _speed}

    n = clamp_speed(value)
    if n is None:
        return {"ok": False, "error": "bad_speed"}

    with _lock:
        _speed = n
    return {"ok": True, "speed": n}

def morse(data=None, speed=None):
    global _queue, _last

    txt = "" if data is None else str(data)
    txt = txt.replace("\r", "").strip()
    if not txt:
        return {"ok": False, "error": "empty"}

    sp = clamp_speed(speed) if speed is not None else None
    with _lock:
        if sp is None:
            sp = _speed  # fallback sur globale

        _queue.append({"txt": txt, "speed": sp})
        _last = {"state": "queued", "txt": txt, "speed": sp, "queue_len": len(_queue)}

        return {"ok": True, "accepted": True, "txt": txt, "len": len(txt), "speed": sp}

def status():
    with _lock:
        return {"ok": True, "busy": _busy, "queue_len": len(_queue), "last": _last, "speed": _speed}

ui.expose_api("GET", "/ping", ping)
ui.expose_api("GET", "/morse", morse)
ui.expose_api("GET", "/status", status)
ui.expose_api("GET", "/speed", speed)   # /speed?value=12 ou /speed

def loop():
    global _busy, _last, _queue

    job = None
    with _lock:
        if (not _busy) and _queue:
            job = _queue.pop(0)
            _busy = True
            _last = {"state": "sending", "txt": job["txt"], "speed": job["speed"], "queue_len": len(_queue)}

    if job is not None:
        try:
            ack = bridge.call("morse_play", job["txt"], job["speed"], timeout=120)
            with _lock:
                _last = {"state": "done", "txt": job["txt"], "speed": job["speed"], "ack": ack, "queue_len": len(_queue)}
        except Exception as e:
            with _lock:
                _last = {"state": "error", "txt": job["txt"], "speed": job["speed"], "detail": repr(e), "queue_len": len(_queue)}
        finally:
            with _lock:
                _busy = False



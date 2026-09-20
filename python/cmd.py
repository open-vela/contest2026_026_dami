import serial, serial.tools.list_ports, threading, time
from time_sender import TimeSender
from picture import take_screenshot, bgr_to_rgb565, save_rgb565

PORT, BAUD = '/dev/ttyACM0', 115200
disconnected = threading.Event()
auto_input_done = threading.Event()
ser = None


def safe_write(data):
    try:
        ser.write(data)
        return True
    except (serial.SerialException, OSError):
        disconnected.set()
        return False


time_sender = TimeSender(disconnected, auto_input_done, safe_write)


def receive():
    while not disconnected.is_set():
        try:
            if ser.in_waiting:
                print(ser.read(ser.in_waiting).decode('utf-8', 'ignore'),
                      end='', flush=True)
        except (serial.SerialException, OSError):
            break
        time.sleep(0.01)
    disconnected.set()


def auto_input():
    for _ in range(3):
        if not safe_write(b"\n"):
            return
        time.sleep(0.3)
    time.sleep(1)
    if not safe_write(b"cm_app\n"):
        return
    time.sleep(1)
    auto_input_done.set()


def user_input_loop():
    auto_input_done.wait()
    while not disconnected.is_set():
        try:
            cmd = input()
        except (EOFError, KeyboardInterrupt):
            break
        c = cmd.strip().lower()
        if c == "time":
            if not safe_write(b"time\n"):
                break
            time_sender.start_sending()
        elif c == "stoptime":
            time_sender.stop_sending()
        elif c == "screenshot":
            print("Taking screenshot...")
            img_bgr = take_screenshot()
            rgb565 = bgr_to_rgb565(img_bgr)
            save_rgb565(rgb565)
            print(f"Saved: {rgb565.shape}, sample: {hex(rgb565[0, 0])}")
        elif not safe_write((cmd + "\n").encode()):
            break
    disconnected.set()


while True:
    try:
        disconnected.clear()
        auto_input_done.clear()
        time_sender.reset()

        while PORT not in [p.device for p in serial.tools.list_ports.comports()]:
            time.sleep(1)

        ser = serial.Serial(PORT, BAUD, timeout=0.1)
        time.sleep(3)

        for fn in (receive, auto_input, time_sender.loop, user_input_loop):
            threading.Thread(target=fn, daemon=True).start()

        while not disconnected.is_set():
            time.sleep(0.5)

        try: ser.close()
        except: pass
        time.sleep(1)

    except KeyboardInterrupt:
        disconnected.set()
        try: ser.close()
        except: pass
        break

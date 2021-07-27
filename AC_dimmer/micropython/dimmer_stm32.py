
from machine import Timer, Pin
from micropython import alloc_emergency_exception_buf
import pyb
import time
alloc_emergency_exception_buf(200)


# scale(10, (0, 100), (80, 30))
def scale(val, src, dst):
    return ((val - src[0]) / (src[1]-src[0])) * (dst[1]-dst[0]) + dst[0]


class Dimmer:
    def __init__(self, dim_pin, zc_pin):

        self.state = False
        self.power_max = 30
        self.power_min = 80

        # Control
        self.power = 30
        self.puls_width = 2

        # PIN
        self._dim = Pin(dim_pin, Pin.OUT)
        self._zc = Pin(zc_pin,  Pin.IN)

        # ISR
        self.extint = pyb.ExtInt(self._zc, pyb.ExtInt.IRQ_RISING , pyb.Pin.PULL_DOWN, self.debounce_handler)
        self.zero_cross = 0
        self.cont = 0
        self._next_call = time.ticks_ms() + 1

        # Dimmer Timer
        self.timer3 = pyb.Timer(3)
        self.timer3.init(freq=10526.31, callback=self.dim_pin_touch)
        self.tim1_ch1 = self.timer3.channel(2, pyb.Timer.OC_TIMING, compare=100)

    def power_set(self, power):
        if power > 0:
            self.state = 1
        else:
            self.state = 0

        self.power = int(scale(power, (0, 100), (self.power_min, self.power_max)))

    def dim_pin_touch(self, timer):

        if self.zero_cross == 1:
            self.cont = self.cont + 1

            if self.cont == self.power:
                self._dim.on()

            if self.cont >= self.power+self.puls_width:
                self._dim.off()
                self.cont = 0
                self.zero_cross = 0

    def debounce_handler(self, pin):
        now = time.ticks_ms()
        if now > self._next_call and self.state:
            self._next_call = now + 1
            self.zero_cross = 1


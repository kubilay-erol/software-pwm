# Bare-Metal Software PWM Controller (C++)

A lightweight, object-oriented Software PWM (Pulse Width Modulation) implementation written in bare-metal C++. This project is designed for microcontrollers where hardware PWM peripherals are either unavailable, entirely mapped to other tasks, or where direct register control over GPIOs is preferred.

It features a non-blocking busy-wait state architecture, allowing the duty cycle to be updated dynamically mid-run while ensuring the execution can be killed instantly by background tasks or interrupts.

## Key Features

* **Custom Memory Allocation:** Uses a custom `MemoryAllocator` namespace to handle explicit placement `new` construction, completely bypassing the standard heap (`malloc`/`free`) to avoid fragmentation.
* **Direct Register Mapping:** Leverages a lightweight `DigIOReg` wrapper to handle direct register interaction (`GPIO_ENABLE_REG`, `GPIO_DATA_REG`) for extremely fast pin toggling.
* **Non-Blocking Control Flow:** The runtime `on()` loop spins freely using an explicit timer reference check. It can be instantly exited via `off()` without waiting for a full wave cycle to finish.
* **Dynamic Adjustment:** High and low time windows are recalculated at the start of every cycle, allowing real-time adjustments via `set_perc()` and `set_count()`.

## Core Logic & Waveform

The timing interval relies on the total tick period (`count`) and the duty cycle percentage (`perc`). To prevent integer underflow bugs commonly found in low-level drivers, the active low period is strictly bounded by subtracting the high ticks from the total period:

$$\text{Active High Ticks} = \frac{\text{count} \times \text{perc}}{100}$$
$$\text{Active Low Ticks} = \text{count} - \text{Active High Ticks}$$

## File Structure & Dependencies

To compile and use this class, ensure your toolchain includes the following low-level peripheral drivers:
* `<memalloc>` - Custom arena/buffer-offset memory allocation utility.
* `<digioreg>` - Direct-mapped GPIO configuration and bit-masking wrapper.
* `<time>` - Core hardware timer tracking reference cycles.

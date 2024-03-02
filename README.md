# radio

radio is the radio server that captures computer's audio and broadcasts it to one client using HTTP.

Audio is transmitted as `audio/wav`, so if you don't want to experience a considerable delay, use it only on LAN-connected devices.

For capturing audio application uses CGO to interface with [libpulse-simple](https://freedesktop.org/software/pulseaudio/doxygen/simple.html). Logically, it requires you to have a [PulseAudio](https://www.freedesktop.org/wiki/Software/PulseAudio/) sound backend. It can be easily ported to any other sound backend, if desired.

# Copyright

Pavlovskii Anton, 2024 (MIT). See [LICENSE](LICENSE) for more details.

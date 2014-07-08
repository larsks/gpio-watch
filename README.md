gpio-watch
==========

`gpio-watch` is a tool for running scripts in response to GPIO events.

Synopsis
========

    gpio-watch [-D script_directory] [-e default_edge] [pin[:edge]] ...

Description
===========

`gpio-watch` was written to make it easy to connect external
edge-triggered sensors -- like pushbuttons -- to a Raspberry Pi (or
other computer with available GPIO lines).  It will watch a set of
pins for events, and will trigger shell scripts in response to those
events.

`gpio-watch` accepts a list of pins to watch for events.  By default
it will trigger on both rising and falling events, but you can change
the default behavior with the `-e default_edge` command line option.

You may also modify the behavior per-pin by appending an edge mode
after the pin number, such as `4:rising` to monitor the rising edge on
pin 4.

Options
=======

- `-D script_directory` -- location in which `gpio-watch` will look
  for event handling scripts.  Scripts must be named after the pin
  number triggering the event.  For example, if you specify `-D
  /etc/gpio-scripts`, and `gpio-watch` processes an event on pin 4, it
  will attempt to run `/etc/gpio-scripts/4`.

  Defaults to `/etc/gpio-scripts`.

- `-e default_edge` -- specifies whether `gpio-watch` should monitor
  rising, falling, or both edges by default.  You can also specify
  edge detection per-pin.

Example
=======

Watch pins 21, 22, and 23 for events.  Monitor rising and falling
events on 21 and 22, but only rising events on 23:

    gpio-watch 21 22 23:rising

If you were to press a button connected to pin 23, `gpio-watch` would
attempt to run:

    /etc/gpio-scripts/23 23 1

That is, the event script is called with both the pin number and the
current pin value to permit a single script to handle multiple events.


# Changelog

The user manual is available at <https://esp32-si4732.github.io/ats-mini/manual.html>. The firmware flashing instructions are available at <https://esp32-si4732.github.io/ats-mini/flash.html>

<!-- towncrier release notes start -->

## v1.00 (2026-09-04)

First public release of the ATS Mini Airband modification.

> This project was heavily vibe-coded with the help of AI.
> I am not a professional programmer — most changes were built through testing, debugging, and a lot of trial and error. 😅
> Code optimization is appreciated.

### Added
- AIR band support.
- 100 MHz and 110 MHz downconverter options.
- 25 kHz and 8.33 kHz airband channel spacing.
- 8.33 kHz channel designator handling.
- Adaptive AM bandwidth:
  - 6.0 kHz for 25 kHz channels.
  - 4.0 kHz for 8.33 kHz-specific channels.
- AIR frequency display in MHz.
- AIR scale support using the displayed channel frequency.
- 108–117.950 MHz instrument-band support with the 100 MHz downconverter.
- 50 kHz tuning in the instrument band.
- Airband-specific tuning acceleration.
- AIR band wrapping for both downconverter options.
- AIR band hidden when the downconverter is disabled.
- AM-only mode for AIR.
- 100 MHz downconverter range: 108–130 MHz.
- 110 MHz downconverter range: 118–137 MHz.
- Direct digit tuning disabled on AIR.

### Note
- This initial release is based on ats-mini firmware version 2.38.

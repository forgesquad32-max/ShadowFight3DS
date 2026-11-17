# ShadowFight3DS

An original, lightweight 2D silhouette fighting game for Nintendo 3DS/2DS. It uses original procedural visuals and code, not proprietary Shadow Fight assets.

## Build from your phone

A GitHub Actions workflow is included. Pushes to `main` trigger a cloud build using devkitPro, so you do not need a PC.

1. Open the **Actions** tab in this repository.
2. Select **Build Nintendo 3DS game**.
3. Open the latest run and wait for it to finish successfully.
4. Download the `ShadowFight3DS-homebrew` artifact from the completed run.
5. Extract the ZIP on your phone and copy `ShadowFight3DS.3dsx` to your SD card, for example:
   `/3ds/ShadowFight3DS/ShadowFight3DS.3dsx`
6. Launch it with Homebrew Launcher on your 3DS.

This workflow targets the Homebrew Launcher `.3dsx` format. It does not produce a CIA installer. CIA packaging needs valid application metadata, icon/banner assets and a separate packaging configuration.

## Controls

- Circle Pad: move
- D-pad Up: jump
- A: punch
- B: kick
- X: weapon attack
- Y (hold): block
- L: dodge
- R: special attack (requires energy)
- START: return to mode selection
- Touchscreen: six combat zones

## Current status

This is an early playable foundation, not a finished commercial-sized game. It currently includes a title screen, mode selection, one arena, procedural fighter silhouettes, movement, jumping, blocking, dodging, attacks, energy, health and basic enemy AI. Distinct mode objectives, multiple weapons, bosses, save data, authored animation, sound, full menus and final artwork remain to be developed.

The GitHub Actions build must complete successfully before a downloadable artifact exists. The game still needs testing on physical 3DS hardware.

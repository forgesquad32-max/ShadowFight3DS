# Chronicles of the Veil — Shadow Fight 3DS

An original 2D martial-arts fighting game for Nintendo 3DS/2DS, built with devkitPro, libctru and Citro2D. It takes inspiration from the side-view fighting and progression genre while using original names, character designs and game content.

## Build from an Android phone

GitHub Actions builds the project in the cloud; no PC is required.

1. Open the repository's **Actions** tab.
2. Open the latest **Build Nintendo 3DS game** run.
3. Wait for the build to finish successfully.
4. Download the release artifact and extract it.
5. Copy `ShadowFight3DS.3dsx` to `/3ds/ShadowFight3DS/` on the SD card.
6. Launch it through Homebrew Launcher.

The CIA packaging job is experimental and is not considered ready until its workflow succeeds and its artifact is available. A CIA requires custom firmware and a CIA installer.

## Controls

- Circle Pad: move
- D-pad Up: jump
- A: punch / confirm
- B: kick
- X: weapon attack
- Y (hold): block
- L: dodge
- R: special attack (uses energy)
- START: return to the hall

The bottom touchscreen is now a menu, shop, gear and loadout display; combat uses the physical controls.

## Current game systems

- Two-screen interface with readable system-font text
- Original armored martial-arts character designs drawn with lightweight 2D primitives
- Campaign stage names and player XP/level progression
- Duel and boss battle modes
- Coin and gem rewards
- Weapon shop with five weapon tiers; weapons increase attack damage
- Armor shop with five armor tiers; armor increases maximum health and reduces incoming damage
- Combo feedback, hit sparks, impact flashes and screen shake

## Development status and limitations

This is an expanding homebrew prototype, not a finished commercial game. Character art is currently procedural 2D artwork rather than hand-drawn sprite animation. The economy and progression currently reset when the application is restarted; persistent save files, authored animation, sound/music, more enemy moves, more stages and full balance testing remain future work.

Build success does not guarantee performance on every 3DS model. Test on hardware before distributing a release.

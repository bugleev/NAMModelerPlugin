# Gateway Modded

Fork of [sdatkinson/NeuralAmpModelerPlugin](https://github.com/sdatkinson/NeuralAmpModelerPlugin) — a VST3 / standalone app for loading [Neural Amp Modeler](https://github.com/sdatkinson/neural-amp-modeler) `.nam` models, built with [iPlug2](https://iplug2.github.io).

**Publisher:** NAMenjoyer  
**Current version: 0.10.2**

## What's new in 0.10.2

- Tag push publishes Windows + macOS artifacts via GitHub Actions (no manual upload)
- macOS unsigned zip available on the release page
- Favorites store link guarded to Apple builds only

## Releasing (tag → CI builds + GitHub Release)

All release artifacts are built and attached **by GitHub Actions**. No local upload.

1. Bump version in `NeuralAmpModeler/config.h` (+ changelog)
2. Commit on `main`
3. Tag and push the tag (this triggers [`.github/workflows/release.yml`](.github/workflows/release.yml)):

```bash
git tag vX.Y.Z
git push origin main
git push origin vX.Y.Z
```

4. Wait for the **Release** workflow: builds Windows + macOS, then publishes https://github.com/bugleev/NAMModelerPlugin/releases with:
   - `Gateway-Modded-Installer.exe` (Windows VST3 + standalone)
   - Windows zip / standalone EXE
   - `GatewayModded-v*-mac.zip` (unsigned macOS app + VST3 + AU)

### Manual test builds (no release page)

- **Actions → Build Windows / Build macOS → Run workflow** — artifacts only, for testing.

## Install

**Windows:** run `Gateway-Modded-Installer.exe` from the release.

**macOS (unsigned):** unzip, copy `GatewayModded.vst3` → `~/Library/Audio/Plug-Ins/VST3/` and `GatewayModded.component` → `~/Library/Audio/Plug-Ins/Components/`. First open: **right-click → Open** (Gatekeeper).

## Version bumps

Edit `PLUG_VERSION_STR` / `PLUG_VERSION_HEX` in [`NeuralAmpModeler/config.h`](NeuralAmpModeler/config.h).

## Upstream

- Original plugin: https://github.com/sdatkinson/NeuralAmpModelerPlugin
- NAM trainer / models: https://github.com/sdatkinson/neural-amp-modeler

## License

Same as upstream — see [LICENSE](LICENSE).

# Gateway Modded

Fork of [sdatkinson/NeuralAmpModelerPlugin](https://github.com/sdatkinson/NeuralAmpModelerPlugin) — a VST3 / standalone app for loading [Neural Amp Modeler](https://github.com/sdatkinson/neural-amp-modeler) `.nam` models, built with [iPlug2](https://iplug2.github.io).

**Publisher:** NAMenjoyer  
**Current version: 0.12.0**

## What's new in 0.12.0

- **CAPTURE knob** (left of the model row): cycles through the `.nam` captures in the folder that is currently loaded in the model browser. Drag up/down or use the mouse wheel to step, hold Cmd/Ctrl for finer steps, double-click to jump back to the first capture. The knob shows `index/total` and the capture file name, and is greyed out with *No folder* until a folder with captures is loaded. It is a UI-only control (no plugin parameter) and loads through the same path as clicking a row in the model list.

## What's new in 0.11.0

- Title branding: **GATEWAY MODDED**
- Model list toolbar: folder icon + current folder name + model count + Favorites (search removed)
- Compact list header for more list space

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

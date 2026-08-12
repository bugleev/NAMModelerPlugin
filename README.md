# NAMampPlugin

Fork of [sdatkinson/NeuralAmpModelerPlugin](https://github.com/sdatkinson/NeuralAmpModelerPlugin) — a VST3 / standalone app for loading [Neural Amp Modeler](https://github.com/sdatkinson/neural-amp-modeler) `.nam` models, built with [iPlug2](https://iplug2.github.io).

**Publisher:** bugleev  
**Current version: 0.10.0**

## What's new in 0.10.0

- Persistent **MODEL LIST** panel always visible at the bottom of the UI (search + favorites)
- Rebranded to **NAMampPlugin** / **bugleev** (installer and VST3 bundle no longer use Steven Atkinson / NeuralAmpModeler names)
- Installs VST3 as `C:\Program Files\Common Files\VST3\NAMampPlugin.vst3`

## Building on Windows

Requires **Visual Studio** (2019/2022) with the C++ desktop workload, and git submodules:

```bat
git submodule update --init --recursive
```

### Build via GitHub Actions (recommended from Linux)

1. Push changes to GitHub
2. Open **Actions** → **Build Windows** → **Run workflow**
3. Download **NAMampPlugin Installer** and run it on Windows to install the VST3 + standalone app

```bash
gh workflow run "Build Windows"
gh run download --name NeuralAmpModeler-Installer
```

### Local Visual Studio

1. Open `NeuralAmpModeler\NeuralAmpModeler.sln`
2. **Release | x64**
3. Build **NeuralAmpModeler-app** and/or **NeuralAmpModeler-vst3**
4. Outputs under `NeuralAmpModeler\build-win\` as `NAMampPlugin_x64.exe` and `NAMampPlugin.vst3`

Or:

```bat
cd NeuralAmpModeler\scripts
makedist-win.bat full installer
```

## Version bumps

```bash
python3 bump_version.py minor
```

Or edit `PLUG_VERSION_STR` / `PLUG_VERSION_HEX` in [`NeuralAmpModeler/config.h`](NeuralAmpModeler/config.h).

## Upstream

- Original plugin: https://github.com/sdatkinson/NeuralAmpModelerPlugin
- NAM trainer / models: https://github.com/sdatkinson/neural-amp-modeler

## License

Same as upstream — see [LICENSE](LICENSE).

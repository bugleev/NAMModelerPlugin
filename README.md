# Neural Amp Modeler Plug-in (fork)

Fork of [sdatkinson/NeuralAmpModelerPlugin](https://github.com/sdatkinson/NeuralAmpModelerPlugin) — a VST3 / standalone app for loading [Neural Amp Modeler](https://github.com/sdatkinson/neural-amp-modeler) `.nam` models, built with [iPlug2](https://iplug2.github.io).

**Current version: 0.9.0**

## What's new in 0.9.0

- **Model list popover** — click the loaded model name to browse `.nam` files in the current folder (replaces the native popup menu for models)
- **Search** — filter the list by filename
- **Favorites** — star models; Favorites filter shows starred models across folders
- Favorites are stored in `%LOCALAPPDATA%\NeuralAmpModeler\favorites.json`
- IR browser behavior is unchanged

## Building on Windows

Requires **Visual Studio** (2019/2022) with the C++ desktop workload, and git submodules:

```bat
git submodule update --init --recursive
```

### Standalone EXE

1. Open `NeuralAmpModeler\NeuralAmpModeler.sln` in Visual Studio
2. Configuration: **Release**, platform: **x64**
3. Build the **NeuralAmpModeler-app** project
4. Output (after post-build): `NeuralAmpModeler\build-win\NeuralAmpModeler_x64.exe`

### VST3 + EXE via script

From a Developer Command Prompt / VS-enabled `cmd`:

```bat
cd NeuralAmpModeler\scripts
makedist-win.bat full zip
```

That builds both `NeuralAmpModeler-app` and `NeuralAmpModeler-vst3` and packages under `NeuralAmpModeler\build-win\`.

### Version bumps

From the repo root (needs `pip install semver`):

```bash
python3 bump_version.py minor   # or major / patch
```

Or edit `PLUG_VERSION_STR` / `PLUG_VERSION_HEX` in [`NeuralAmpModeler/config.h`](NeuralAmpModeler/config.h) and sync [`NeuralAmpModeler/installer/NeuralAmpModeler.iss`](NeuralAmpModeler/installer/NeuralAmpModeler.iss).

## Upstream

- Original plugin: https://github.com/sdatkinson/NeuralAmpModelerPlugin
- NAM trainer / models: https://github.com/sdatkinson/neural-amp-modeler
- Pre-built “Gateway” builds from the original author: https://neuralampmodeler.com/users

## License

Same as upstream — see [LICENSE](LICENSE).

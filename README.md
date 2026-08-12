# Gateway Modded

Fork of [sdatkinson/NeuralAmpModelerPlugin](https://github.com/sdatkinson/NeuralAmpModelerPlugin) — a VST3 / standalone app for loading [Neural Amp Modeler](https://github.com/sdatkinson/neural-amp-modeler) `.nam` models, built with [iPlug2](https://iplug2.github.io).

**Publisher:** NAMenjoyer  
**Current version: 0.10.1**

## What's new in 0.10.1

- Renamed to **Gateway Modded** / **NAMenjoyer**
- Favorites filter scoped to the current folder
- Empty-state folder browse in the model list
- Clearer model list text
- Installs VST3 as `C:\Program Files\Common Files\VST3\GatewayModded.vst3`

## Windows (installer)

1. Open **Actions** → **Build Windows** → **Run workflow** (or download from [Releases](https://github.com/bugleev/NAMModelerPlugin/releases))
2. Run **Gateway-Modded-Installer.exe**

```bash
gh workflow run "Build Windows"
gh run download --name NeuralAmpModeler-Installer
```

Local: open `NeuralAmpModeler\NeuralAmpModeler.sln`, **Release | x64**, build app/VST3 → `GatewayModded_x64.exe` / `GatewayModded.vst3`.

## macOS (unsigned zip — free / OSS)

No Apple Developer signing. Builds are **unsigned**; Gatekeeper will warn once.

1. Open **Actions** → **Build macOS** → **Run workflow**
2. Download **GatewayModded-macOS** zip
3. Copy `GatewayModded.vst3` → `~/Library/Audio/Plug-Ins/VST3/`
4. Copy `GatewayModded.component` → `~/Library/Audio/Plug-Ins/Components/` (AU)
5. First open: **right-click → Open** (or `xattr -dr com.apple.quarantine` on the bundles)

```bash
gh workflow run "Build macOS"
gh run download --name GatewayModded-macOS
```

## Version bumps

Edit `PLUG_VERSION_STR` / `PLUG_VERSION_HEX` in [`NeuralAmpModeler/config.h`](NeuralAmpModeler/config.h).

## Upstream

- Original plugin: https://github.com/sdatkinson/NeuralAmpModelerPlugin
- NAM trainer / models: https://github.com/sdatkinson/neural-amp-modeler

## License

Same as upstream — see [LICENSE](LICENSE).

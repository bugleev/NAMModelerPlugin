import zipfile, os, fileinput, string, sys, shutil, glob

scriptpath = os.path.dirname(os.path.realpath(__file__))
projectpath = os.path.abspath(os.path.join(scriptpath, os.pardir))

IPLUG2_ROOT = "..\..\iPlug2"

sys.path.insert(0, os.path.join(scriptpath, IPLUG2_ROOT + "\Scripts"))

from get_archive_name import get_archive_name


def find_files(patterns):
    """Collect build outputs by pattern so that renaming the plug-in does not break packaging."""
    found = []
    for pattern in patterns:
        matches = [p for p in glob.glob(os.path.join(projectpath, pattern)) if os.path.isfile(p)]
        found.extend(sorted(matches))
    return found


def main():
    if len(sys.argv) != 3:
        print("Usage: make_zip.py demo[0/1] zip[0/1]")
        sys.exit(1)
    else:
        demo = int(sys.argv[1])
        zip = int(sys.argv[2])

    dir = projectpath + "\\build-win\\out"

    if os.path.exists(dir):
        shutil.rmtree(dir)

    os.makedirs(dir)

    if not zip:
        installer_pattern = (
            "build-win\\installer\\*Demo Installer.exe" if demo else "build-win\\installer\\*Installer.exe"
        )
        files = find_files([installer_pattern])
        if not demo:
            files = [f for f in files if "Demo" not in os.path.basename(f)]

        files += [
            projectpath + "\\installer\\changelog.txt",
            projectpath + "\\installer\\known-issues.txt",
        ]
        files += find_files(["manual\\*manual.pdf"])
    else:
        files = find_files(
            [
                "build-win\\*.vst3\\Contents\\x86_64-win\\*.vst3",
                "build-win\\*_x64.exe",
            ]
        )

    if not files:
        print("ERROR: no build outputs found to package")
        sys.exit(1)

    zipname = get_archive_name(projectpath, "win", "demo" if demo == 1 else "full")

    zf = zipfile.ZipFile(
        projectpath + "\\build-win\\out\\" + zipname + ".zip", mode="w"
    )

    for f in files:
        print("adding " + f)
        zf.write(f, os.path.basename(f), zipfile.ZIP_DEFLATED)

    zf.close()
    print("wrote " + zipname)

    zf = zipfile.ZipFile(
        projectpath + "\\build-win\\out\\" + zipname + "-pdbs.zip", mode="w"
    )

    pdbs = find_files(["build-win\\pdbs\\*.pdb"])

    for f in pdbs:
        print("adding " + f)
        zf.write(f, os.path.basename(f), zipfile.ZIP_DEFLATED)

    zf.close()
    print("wrote " + zipname)


if __name__ == "__main__":
    main()

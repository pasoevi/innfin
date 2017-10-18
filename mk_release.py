#!/usr/bin/env python3

import os
import sys
import shutil
import tarfile
import zipfile

version_number_str = sys.argv[1]
target_os = 'win' if os.name == 'nt' else 'linux'
version_name = "innfin" + "_" + version_number_str + "_" + target_os

def ignore(dir, contents):
    return ['Release', 'Debug']

def zipdir(path, ziph):
    # ziph is zipfile handle
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file))

if __name__ == '__main__':
    # Create the copy of the build directory
    if os.name == 'nt':
        # shutil.copy('build/Debug/innfin.exe', 'build')
        shutil.copy('./msvc/Release/Innfin.exe', 'build')
    shutil.copytree('build', 'release/' + version_name, ignore = ignore )

    os.chdir('release')

    # Create the tar archive
    with tarfile.open(version_name + ".tar.gz", "w:gz") as tar:
        tar.add(version_name)

    # Create the zip archive
    zipf = zipfile.ZipFile(version_name + '.zip', 'w', zipfile.ZIP_DEFLATED)
    zipdir(version_name, zipf)
    zipf.close()

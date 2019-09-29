import sys
import os.path

program_name = "innfin"
release_dir = 'release'
build_dir = 'build'

platform = sys.platform
# Wextra
w = ' -Wall -Wstrict-prototypes -Wshadow -Wwrite-strings -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wstrict-aliasing'
cc = "gcc"

libs = Split('tcod m')
ccflags = '-g -O0  -Wall'
ccflags_win = ' -mwindows'
ccflags_osx = ' -Wl,-rpath=.'
ccflags_linux = ' -Wl,-rpath=.'

print(platform)

if platform == 'win32':
   ccflags += ccflags_win
elif platform == 'linux' or platform == 'linux2':
   ccflags += ccflags_linux
elif platform == 'darwin':
   ccflags += ccflags_linux

print(ccflags)

VariantDir(build_dir, 'src', duplicate=0)
env = Environment(CC = cc,
                  CCFLAGS = ccflags,
                  tools=['default', 'mingw'],
                  TARFLAGS = '-c -z')

libdirs = [
   'lib',
   '/usr/lib',
   '/usr/local/lib',
   ]

libpath = [p for p in libdirs if os.path.exists(p)]

includedirs = [
   'src',
   'include',
   'src/libtcod'
   ]

cpppath = [p for p in includedirs if os.path.exists(p)]

src_files = Glob('build/*.c')
#Command("build/graphics", "assets/graphics", Copy("build/${SOURCE.file}", "$SOURCE"))
Command("build/terminal.png", "assets/fonts/terminal.png", Copy("build/${SOURCE.file}", "$SOURCE"))
Command("build/monsters.txt", "data/monsters.txt", Copy("build/${SOURCE.file}", "$SOURCE"))
Command("build/libtcod.cfg", "data/cfg/libtcod.cfg", Copy("build/${SOURCE.file}", "$SOURCE"))
#Command("build/levels", "assets/levels", Copy("build/${SOURCE.file}", "$SOURCE"))
for lib in Glob('lib/*'):
   Command("build/${SOURCE.file}", lib, Copy("build/${SOURCE.file}", "$SOURCE"))

env.Program(source = src_files, target = 'build/' + program_name, LIBS=libs,
LIBPATH=libpath, CPPPATH=cpppath)

releaseflag = ARGUMENTS.get('release', 0)

if int(releaseflag):
   if(platform == 'win32'):
      env.Zip(os.path.join(release_dir, program_name + '-win32.zip'), ['build'])
   else:
      env.Tar(os.path.join(release_dir, program_name + '-gnu-linux.tar.gz'), ['build'])

Clean('.', build_dir)

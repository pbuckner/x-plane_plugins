# XPPython3 Plugin Build

## Structure
This directory XPPython3 contains code for python plugin, as modified to support
Python3 and _only_ SDK 303+ (X-Plane 11.50+)

- Linux, tested on Ubuntu18 with gnu compiler,
- Mac, developed and tested on Catalina 10.15 with Xcode compiler (command line only), 
- Windows, tested on Windows 10 with mingw-w64 tools. Might be compilable with VisualStudio, but I don't care.

    XPPython3/
       *.[ch]
       lin/
          Makefile
       win/
          Makefile
       mac/
          Makefile
       SDK/
          CHeaders/
          Libraries/
          Delphi/
       Resources/plugins/
       examples/
          <sample PI Plugins>
       stubs/
          XP*.py 

## To build:
1. verify ``python-config'' returns info for python 3 (this is used by makefile)
2. Copy / link current SDK as "SDK" directory immediately under XPPython3. I DO NOT INCLUDE SDK with this plugin source
3. To build:

On the appropriate architecture:

    $ make Linux
    $ make Darwin
    $ make Windows

4. Build objects are in (lin|mac|win) subdirectory: .o and <target>.xpl.
   AND python plugin <target>.xpl copied to `Resources/plugins/XPPython3/(mac|win|lin)_x64/`.

Quick non-X-Plane test, from XPPython3 directory  ##!! Only partially supported 5/29/2020
`$ make test`
... This will call mac/main_302, with the plugin location correctly set.
   
## To test:
Set your X-Plane to point to build product location

    X-Plane/Resources/plugins/XPPython3 -> <code>/XPPython3/Resources/plugins/XPPython3

That way, starting X-Plane, will use latest built plugin

For Windows.. I installed 64-bit version of mingw, updated makefiles to support
cd /z

## Internal Development
1. Check what branch you're on, ideally, it should be named for feature ('parameters')
   git status
   git switch <feature>
2. Update makefile for alpha/beta numbering (this gets compiled into code)
3. Update docs/source/conf.py for (ultimate) release (without alpha/beta)
4. Update docs/source/_theme/python_docs_theme/versions.html versionlist, setting stable / beta
5. Build and test locally

## Release Beta (make available remotely)
1. Check branch, latest work should be (still) in feature branch
   git status
2. Build all versions (python3.x for each platform)
3. Compare staging python vs. build python code ~/xp/Resources/plugins/XPPython/compare.csh
4. Verify imgui compiled correctly
5. Merge feature branch into beta
   git switch beta
   git merge <feature>
5. Commit
   git commit -a  -m 'merge from feature'
6. Push to remote. Docs should build with change to 'beta' branch.

## To Release:

1. On Beta branch
   git switch beta
2. Update Makefile for final version
3. Build all versions (python3.x for each platform)
4. Quick test
5. Commit (final) for beta
   git commit -a -m 'final for 3.1.1'
6. Merge beta into master
   git switch master
   git merge beta
   git commit -a -m 'merge from beta'
4. Push to github
5. "Make Release".
   a) github.com/pbuckner/x-plane_plugins/releases "Draft a new release"
   b) "Choose a tag": create new tag, e.g., v3.0.12
   c) Set Release Title to version, e.g., "3.0.12"
   d) Target: master
   e) Publish Release
   ... this will cause this to be "latest" build, and will allow readthedocs to pull this release as "stable"


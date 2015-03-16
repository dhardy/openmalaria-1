# Dependencies #

Dependencies for the mac build are  similar to those required for the [unix build](UnixBuildOpenMalaria.md). However, be aware that several libraries and toolkits haven't yet been ported over to MAC OS X 10.7 Snow Leopard, and in particular, for Xcode 4.3 does not support gcc tools.  For that specific reason, we suggest you go no higher than Xcode 4.2.

For example, you will need to build python GTK from source, which as of this writing has not yet been ported to snow leopard and is required to build the OpenMalariaTools front end.   If you can't be persuaded to  save yourself some trouble, and run OpenMalaria and OpenMalariaTools under ubuntu on a linux virtual machine, then be prepared to roll up your sleeve and get a cup of coffee before diving in. Let us know how you get on.

# Backward compatibility #

To ensure backward compatibility, specify the older SDK version you'd like to use in cmake.  In particular,  boinc requires SKD < 10.3.9. Use the flag CMAKE\_OSX\_SYSROOT to specify your SDK of choice.

# Building #

After checking out the source code, and building all the dependencies that haven't yet been ported to MAC OS X,  you have two options to build OpenMalaria:

  * Use Xcode (see below)
  * Use Makefiles (well described on the [unix](UnixBuildOpenMalaria.md) page - in this case, recall that you must specify the use of the gcc complier and linker)


## Xcode ##

  * Run cmake-gui
  * Set the source dir, and the build dir (suggested: SOURCE\_DIR/build)
  * Run configure. Set the build system to Xcode.
  * Either disable OM\_CXXTEST\_ENABLE or install [yaml-cpp](http://code.google.com/p/yaml-cpp/)
  * Configure as described in the linux/unix instructions, and generate
  * From the build dir, open openmalaria.xcodeproj
  * Build the xsdcxx target (select libxsdcxx.a as the active target; build)
  * Then build the rest with the ALL\_BUILD target
  * Run the tests by building the RUN\_TESTS target

Should cmake regenerate the project (as is known to happen), you'll need to manually reload the project into Xcode.


# Building static binaries #

Whether you use Xcode or Makefiles, for a static build, xerces must be linked with the Carbon Framework. In cmake, add the required link parameter using CMAKE\_EXE\_LINKER\_FLAGS and adding **-framework Carbon**
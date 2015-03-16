# Introduction #

The following instructions are  _only_ necessary for building a release for malariacontrol.net. For development of OpenMalaria itself,  boinc-integration can be disabled. For participating in the volunteer computing, go to [malariacontrol.net] or install [boinc](http://boinc.berkeley.edu/wiki/System_requirements) from your distribution's packages.

You may just be able to use the [boinc-dev](http://packages.debian.org/search?keywords=boinc-dev) package. Otherwise, read on...


# Details #

Check out a copy of boinc. Normally you can use the latest stable version:
```
svn co http://boinc.berkeley.edu/svn/branches/server_stable boinc
cd boinc
```
For older systems, this may not work. For ubuntu 6.06:
```
svn co http://boinc.berkeley.edu/svn/branches/boinc_core_release_5_10 boinc
cd boinc
```

Configure:
```
./_autosetup
# For compiling a release version, we only want static libraries: 
./configure --disable-client --disable-manager --disable-server --disable-shared -C
OR
./configure --disable-client --disable-server LDFLAGS=-static-libgcc
```

You only need the main lib and api; to save time just build and install these:
```
cd lib
make
sudo make install
cd ../api
make
sudo make install
```
# Introduction #

Xerces should be included into the ubuntu release, so you should be able to install xerces-c using your package manager of choice:
```
apt-get install libxerces-c2-dev
```
If you would rather build the library from source, read on.

# Details #
Both versions 2.8.x and 3.0.x should work, but both have given me plenty of problems, either while compiling or while linking (creating the OpenMalaria executable) using the resultant library. I don't know if older versions would work.

### Install from mac ports ###
Note that installing multiple copies of libraries isn't a good idea. In particular, if you already have xerces include files available on your system, they might be used along with a library from elsewhere, creating conflicts if the library is built from a different version of the source code.

Download and install macports from http://www.macports.org/ or [darwinports](http://darwinports.com/download/).

Start the interactive shell:
```
sudo port -d selfupdate
sudo port
```
  * From the interactive port shell:
```
list xercesc
install xercesc
quit
```

Macports installs xerces into /opt/local without making libraries or include files here generally available - so you need to make those locations explicitly available.

  * make the library accessible:
```
cd openMalaria/lib
ln -s /opt/local/lib/libxerces-c.a ./
```
  * make the includes accessible:
```
cd ../include
ln -s /opt/local/include/xercesc ./
```

### Compile from source ###

As noted above, you don't have to use version 2.8., but if you did, the command to retrieve the source code would be:
```
wget http://mirror.switch.ch/mirror/apache/dist/xerces/c/2/sources/xerces-c-src_2_8_0.tar.gz
tar -xvzf xerces-c-src_2_8_0.tar.gz
cd xerces-c-src_2_8_0/
export XERCESCROOT=`pwd`
cd src/xercesc
```
  * configure for linux:
```
./runConfigure -plinux -cgcc -xg++ -minmem -nsocket -tnative -rpthread -s
```
  * configure for mac:
```
 ./runConfigure -pmacosx -cgcc -xg++ -minmem -nsocket -tnative -rpthread -s
```
Then build and install. If you are installing static libraries, put them in /usr/local/lib:
```
make
sudo make install
cd ../../..
```
# Introduction #

Windows comes with relatively little software, compared to macs and linux, so installing a few other programs does help. Everything listed here is totally free and should be very easy to install.


## File types ##

All (or nearly all) files on windows platforms have an extension (.txt, .doc, .zip, etc.) which identifies which type of file they are. By default, windows hides this for all files it recognizes, but I would recommend you show it so you can be sure what type of file you're using. You can do this by, in a file browser, clicking "Tools", "Folder options", and disabling "Hide extension for known file types".

The file types you're likely to encounter with OpenMalaria are:

  * .txt : plain text files (open with a text editor)
  * .xml : XML files, also open with a text editor (note: internet explorer can be used to view them nicely, but cannot edit them)
  * .xsd : XML Schema Document; describes what content is allowed in the associated XML file
  * .gz, .bz2 (or .something.gz / .bz2) : a compressed file; see "Archive tools" below
  * .zip : standard windows archive
  * .bat : windows batch file (a very basic script format)
  * .py : python scripts
  * .cpp, .h, etc. : C++ code files (the project code)


## Text editors ##

Windows comes with 2: notepad and wordpad. However neither are very good for editing code (XML or other types) and notepad cannot understand unix line-endings.  A better text editor on windows is [Notepad++](http://notepad-plus.sourceforge.net) which can be installed from ([here](http://sourceforge.net/projects/notepad-plus/files/)) and run it.

By default Notepad++ won't register itself with any files (meaning if you double-click on XML, txt, files, etc., they will open in some other program, not notepad++). Instead, you can right-click on a file and choose "open with Notepad++" (or "edit with ..."). (It may be necessary to close and re-open file browsers or restart to see this; don't remember exactly.) If you also want to make some types of file open in notepad++ when double-clicked on, you can do so by going to notepad++'s settings menu and adjusting the "associate/register with file types" item, for whichever file types you want (e.g. .txt and .xml).

## Archive programs ##

Windows has built-in support for .zip files, but not the .gz files we sometimes use as output. (Hopefully you won't need to decompress the output files yourself, but being able to read various other archive formats is generally quite useful so I'll describe this anyway.)

[7-zip](http://7-zip.org/) is recommended for this (another common program is winzip). Download the installer on the site (the 32-bit, non-beta version is probably your best choice) and run it. As with notepad++, it doesn't associate itself with files automatically but provides right-click menus.  It is recommended to do so by running it, going to the settings menu and "associate with file types", selecting everything except .zip (since windows can deal with these anyway), and pressing OK/whatever.

To decompress something you can do one of the following:
  * right click on it and use one of the "extract..." options
  * opening the archive in the 7-zip program, then viewing/extracting files

Note that extracting a file won't replace the original archive, but will often create a new file/folder without the archive extension (.gz or whatever).
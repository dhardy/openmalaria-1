# Source code #

Code is stored with the 'git' version control system. Click on the 'source' tab to browse, then select one of the repositories:
| **repository** | **description** |
|:---------------|:----------------|
| [default](http://code.google.com/p/openmalaria/source/browse/) | OpenMalaria code |
| [wiki](http://code.google.com/p/openmalaria/source/browse?repo=wiki) | the data for this wiki |
| [web-portal](http://code.google.com/p/openmalaria/source/browse?repo=web-portal) | Web portal |
| [omtools](http://code.google.com/p/openmalaria/source/browse?repo=omtools) | [OpenMalaria-Tools (GUI frontend)](openmalariaTools.md) |
| [experiment-creator](http://code.google.com/p/openmalaria/source/browse?repo=experiment-creator) | [Tool for generating experiments](ExperimentDesign.md) |
| [svn-archive](http://code.google.com/p/openmalaria/source/browse?repo=svn-archive) | Git repository containing a complete checkout of the old subversion repository (for history, old tags) |

To check out a complete copy of one of the repositories and to commit changes, you need to use git. The [Git Pro](http://git-scm.com/book) book should help get you started.

The `default` repository (i.e. for the OpenMalaria model code) has several different branches. To get the latest stable code, use the `master` branch, or for the very latest code, use the `develop` branch. When committing changes, work from the `develop` branch (or a new branch based on `develop`). For an explanation of the branch system [see here](http://nvie.com/posts/a-successful-git-branching-model/).

You can find a little more information about organisation of the repository in the [readme file](http://code.google.com/p/openmalaria/source/browse/README.txt).


# Doxygen documentation #

The OpenMalaria code is, to a large extent, documented with [Doxygen](http://www.doxygen.org/)-style comments. At one point in time this was used to generate HTML documentation of the code, but due to lack of use and some minor issues we no longer do this. It is suggested to read the code instead.

<a href='Hidden comment: 
(This is used to autonomously generate html documentation of various interfaces in the code, largely aimed at software developers.) The latest build of this documentation (generated daily) is at:

http://www.malariacontrol.net/openmalaria/html/
'></a>
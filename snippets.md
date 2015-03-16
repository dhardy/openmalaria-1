# Snippet library #

To ease configuration of OpenMalaria scenarios, a library of parameterised "snippets" of XML is available.

  * [You can browse the library here](https://code.google.com/p/openmalaria/source/browse/?repo=snippets)
  * [You can clone the library here (required to commit edits)](https://code.google.com/p/openmalaria/source/checkout?repo=snippets)


## Format ##

The library contains a separate section for each OpenMalaria schema version. It is the responsibility of OpenMalaria developers to update snippets to the latest version. [OpenMalaria changelog](Changelog.md)

The snippets provided are sections of XML which can be copied into a scenario description under development. Template documents into which these snippets can be pasted should also be provided.

All snippets _should_ be documented with information on parameter sources.

There are currently no requirements on the content of snippets other than that it be clear to a human with a little familiarity with OpenMalaria XML documents how to make use of it.


## Contributions ##

Contributions of new publishable and documented snippets are welcomed. If you can edit this wiki you should also be able to amend this library yourself, by cloning the git repository (see link above); alternatively contact the OpenMalaria developer.

Snippets should be placed under the folder for the appropriate version in a sensible location (it may be necessary to rearrange other snippets; there is currently no defined organisation beyond versions). New snippets should also be updated to the latest version (by notifying someone able to do this if necessary).
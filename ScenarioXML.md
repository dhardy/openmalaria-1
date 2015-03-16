# XML Basics #

Scenarios are written in XML. There is a (long and confusing) standard describing XML [here](http://www.w3.org/TR/REC-xml/), and a (much more accessible) tutorial [here](http://www.w3schools.com/xml/default.asp). But all you really need to know about XML is that:

  1. The following things enclosed by angle brackets are called elements. Each one must be closed, either with the short form `<.../>` or the long form `<x>...</x>`.
```
<a/>
<aLongElementName/>
<b></b>
<c>
  <d>
    <e/>
  </d>
</c>
```
  1. Elements may have attributes. Each attribute has a name (before the `=` sign) and a value (within the double quotes).
```
<x a="1" b="2" c="3"/>
<y anAttribute="a value">
  <z anotherAttribute="..."/>
</y>
```
  1. You can add comments or "comment out" code with the following notation:
```
<!-- This is a comment. -->
<!-- Element 'a' is used by the program, 'b' is not. -->
<a/>
<!-- <b/> -->
```
  1. All scenarios start and end with code like one of the following chunks (pick the appropriate version). Versions 31 and earlier require `analysisNo` and `wuID` attributes (in version 32 these are used only for fitting runs and BOINC).  Versions 23 and earlier also require an `assimMode` attribute. The `name` attribute can be set to whatever you want.
```
<!-- versions 32 or later (replace the number 32 below) -->
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<om:scenario xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    name="example" schemaVersion="32"
    xsi:schemaLocation="http://openmalaria.org/schema/scenario_32 scenario_32.xsd"
    xmlns:om="http://openmalaria.org/schema/scenario_32">
  ...
</om:scenario>

<!-- versions 24 to 31 (replace the number 31 below) -->
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<scenario xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" analysisNo="0"
    name="template scenario" schemaVersion="31" wuID="0"
    xsi:noNamespaceSchemaLocation="scenario_31.xsd">
  ...
</scenario>
```
> > The file name (`scenario_xx.xsd`) can be changed to the name of the schema file, if this is different, but the URL (`http://.../scenario_32`) and the `schemaVersion="32"` attribute shouldn't be changed, except to replace the number with the appropriate version.
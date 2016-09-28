The Expat XML Parse Instruction
=============================

Expat is an XML parser library written in C which be used for parse XML documents.

It is a stream-oriented parser in which an application registers handlers for things the parser might find in the XML document.

It can parse some larger files.

-   Expat XML Parser support many different processor, but for the most part function you only need the following functions:

    **XML_ParserCreate**: Create a new parser object

    **XML_SetElementHandler**: Set handlers for start and end tags

    **XML_SetCharacterDataHandler**: Set handler for text

    **XML_Parse**: Pass a buffer full of document to the parser

More information about Expat library can be found on http://expat.sourceforge.net

An introductory article on using Expat is available on http://xml.com

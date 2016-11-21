<?xml version="1.0" encoding="UTF-8"?>

<!-- This XSL transform converts Doxygen XML output for a header file into Sphinx/Breathe compatible list of APIs -->
<!-- Usage:  xsltproc doxygen_xml_to_rst.xslt xml/esp__xxxx_8h.xml -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="text" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:text>Macros&#xA;</xsl:text>
        <xsl:text>^^^^^^&#xA;&#xA;</xsl:text>
        <xsl:for-each select="doxygen/compounddef/sectiondef/memberdef[@kind='define']">
            <xsl:text>.. doxygendefine:: </xsl:text>
            <xsl:value-of select="name"/>
            <xsl:text>&#xA;</xsl:text>
        </xsl:for-each>
        <xsl:text>&#xA;</xsl:text>

        <xsl:text>Type Definitions&#xA;</xsl:text>
        <xsl:text>^^^^^^^^^^^^^^^^&#xA;&#xA;</xsl:text>
        <xsl:for-each select="doxygen/compounddef/sectiondef/memberdef[@kind='typedef']">
            <xsl:text>.. doxygentypedef:: </xsl:text>
            <xsl:value-of select="name"/>
            <xsl:text>&#xA;</xsl:text>
        </xsl:for-each>
        <xsl:text>&#xA;</xsl:text>

        <xsl:text>Enumerations&#xA;</xsl:text>
        <xsl:text>^^^^^^^^^^^^&#xA;&#xA;</xsl:text>
        <xsl:for-each select="doxygen/compounddef/sectiondef/memberdef[@kind='enum']">
            <xsl:text>.. doxygenenum:: </xsl:text>
            <xsl:value-of select="name"/>
            <xsl:text>&#xA;</xsl:text>
        </xsl:for-each>
        <xsl:text>&#xA;</xsl:text>

        <!-- TODO: figure out why structures are listed as innerclass instances -->
        <xsl:text>Structures&#xA;</xsl:text>
        <xsl:text>^^^^^^^^^^&#xA;&#xA;</xsl:text>
        <xsl:for-each select="doxygen/compounddef/innerclass">
            <xsl:text>.. doxygenstruct:: </xsl:text>
            <xsl:value-of select="."/>
            <xsl:text>&#xA;</xsl:text>
            <xsl:text>    :members:&#xA;</xsl:text>
            <xsl:text>&#xA;</xsl:text>
        </xsl:for-each>
        <xsl:text>&#xA;</xsl:text>

        <xsl:text>Functions&#xA;</xsl:text>
        <xsl:text>^^^^^^^^^&#xA;&#xA;</xsl:text>
        <xsl:for-each select="doxygen/compounddef/sectiondef/memberdef[@kind='function']">
            <xsl:text>.. doxygenfunction:: </xsl:text>
            <xsl:value-of select="name"/>
            <xsl:text>&#xA;</xsl:text>
        </xsl:for-each>
        <xsl:text>&#xA;</xsl:text>

    </xsl:template>
</xsl:stylesheet>

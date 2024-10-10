# ParseWeb
Accessible Web Dictionary App

## Overview
This program is used to access websites, to retrieve information
(usually dictionary / synonyms etc.) and remove the superfluous
formatting and present the results cleanly for a visually impaired
user.

## Configuration

All of the configuration is undertaken in the parseweb.ini file
Multiple dictionaries are supported, just add more sections.

This ini file has a different section for each online resource (e.g. dictionary)
Each section contains the following fields:

### APPNAME - Name of the Application

This name appears in the window title bar.

### SHORTAPPNAME - Shortened Name of the Application

This is an accessible shortened name, which appears on a braille display.

### ICON - Icon to use for application

This is the icon to use for the application, and must be one of the following:

    * parseweb
    * network
    * search
    * news
    * rss
    * television
    * france
    * germany
    * italy
    * spain
    * uk

### SEARCHPROMPT - Text prompt for the GUI input

This parameter is used for the text input field prompt, for example:

    SEARCHPROMPT="English Word"

### ACCESSIBLESEARCHNAME - Accessible name for input text prompt

This parameter is used on accessible displays for the search prompt, for example:

    ACCESSIBLESEARCHNAME="E-F Search"

### ACCESSIBLEOUTPUTNAME - Accessible name for text output prompt

This parameter is used on accessible displays for the search results, for example:

    ACCESSIBLEOUTPUTNAME="English French Results"

### URL - URL to search

The URL to search is a string, with the supplied keyword location identified with {{KEYWORD}}
For example:

    https://www.thefreedictionary.com/{{KEYWORD}}

### HEADER - HTTP Headers to submit

Multiple additional HTTP headers can be provided.  Each of these are in the format Header: value-string, and are separated with the \n newline sequence, for example:

    Accept: *\nOrigin: www.site.com

### USERAGENT - Override default web browser user agent

By default, the ParseWeb application provides the following string as the useragent (identifying the
type of 'web browser' used:

    Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36

This can be overridden with the USERAGENT parameter.

### REMOVECOMMENTS - Remove all comments

If set to true, ensures that <!-- --> comments are removed before any searching or parsing.

### SEARCH - Search pattern with text surrounding results.

The search pattern is used to extract the core 'results' section of the web page to be parsed.  Wildcards
are permitted in this parameter. Data to be processed for the response is identified with {{SEARCHDATA}}.
Note that quotation marks within the search sequence must be escaped - i.e. \"

For example:

    SEARCH="id=\"maincontent\" *>{{SEARCHDATA}}<div id=\"end-of-results-banner\" *>"

### REPLACEn - Replaces the text given with the string provided in WITHn (n=1-4)

The REPLACEn and WITHn allows for up to four different string sequences to be replaced.  This step is performed immediately after the SEARCH operation.

Note that * and ? characters can be used in the REPLACEn parameters.

### DELIMIT - Sequence marking the end of an entry

Searches may provide multiple responses, and these responses can be separated with a '----' delimiter.
The DELIMIT parameter is used to replace a given string with a '\n----\n' sequence, for example:

    DELIMIT="<div class=\"flex-row\">"

### NEWLINE - Sequence to be used for a newline in the results

The <br> sequence is automatically interpreted, and the sequence is replaced with the \n newline character.  The NEWLINE parameter allows for an additional sequence to be used to introduce newlines in the output, for example:

    NEWLINE="<div class=\"ds-list\">"


### OUTPUTPREFIX - Test to insert at the start of an output

This parameter can be used to inject an additional string at the start of the output, for example:

    OUTPUTPREFIX="Synonyms of "

### ISO8559RESPONSE - true/false - identify if the response is in ISO8559 format

If set to true, the captured web page response is expected to contain legacy characters in ISO8559-1 format (i.e. characters 0-255 representing ASCII / Extended ASCII).

If set to false (default), then ISO10646 / UTF-8 is assumed, i.e. Unicode characters may be present.

## Configuration - Debugging

The following parameters may be used in the ini file to assist in debugging web page captures.

### TESTFILE - HTML File to use instead of URL

In order to test the parsing, the TESTFILE parameter may be used.  In this case, the specified file
is loaded instead of the website specified in 'URL'.

### DEBUG - Causes raw html to be captured to local disk

When this option is set to true, several files are created in the current directory following each search operation.  These files store the captured data at each of the following steps:

* parseweb-00-url.txt
* parseweb-01-rawresponse.txt
* parseweb-02-searchpattern.txt
* parseweb-03-parsedsearchpattern.txt
* parseweb-04-uncommented.txt
* parseweb-05-extracteddata.txt
* parseweb-06-replacedwith.txt
* parseweb-07-newlines.txt
* parseweb-08-delimit.txt
* parseweb-09-removedtags.txt
* parseweb-10-decodedcharacters.txt
* parseweb-11-whitespaces.txt
* parseweb-12-output.txt

The points at which the file is generated are shown in the following section.

## Sequence of Operations

The following list presents the sequence of processing undertaken by ParseWeb

1. Get URL from ini file<sup>00</sup>
2. Load URL or File<sup>01</sup>
3. Get SEARCH from ini file <sup>02</sup>
4. Process / expand SEARCH pattern into internal 'regexp' format <sup>03</sup>
5. Remove Comments if requested <sup>04</sup>
6. SEARCH and discard any data outside {{SEARCHDATA}} <sup>05</sup> - note that the '*' wildcard can be used
7. Replace REPLACEn sequence in HTML with WITHn <sup>06</sup> - note that '*' and '?' wildcards can be used in the REPLACEn sequence
8. Replace <br> with newlines <sup>07</sup>
9. Remove all html tags <sup>08</sup>
10. Mark DELIMIT sequence in HTML <sup>09</sup>
11. Decode escape sequences <sup>10</sup>
12. Replace NEWLINE sequence in HTML with a newline, remove rogue spaces and Add delimit sequences <sup>11</sup>
13. Output data <sup>12</sup>

## Notes

Note that the ParseWeb application does not run any embedded javascript, and is unable to parse dynamically updated web pages.
It may be possible to capture the source stream data, however note that this program is capable of parsing XML data streams, but cannot process json data, so it is important to understand the format of the source data before attempting any processing.

# Interactive Content File Format (IC)
This repository currently contains both the IC format specification as well as a Windows client to read IC files. The project is currently in alpha and the specification is a LaTeX document. The specification is currently a work in progress [which can be viewed here](Website/Spec.tex).

## Overview
- Interactive Content is based on a container based system. There are four main components in an Interactive Content file: Containers, Layouts, Links, and Content.
- Each container follows a defined Layout.  A container can include more Containers or have Content.
- Content objects must be inside a container for them to be displayed.
- Links are between containers, and must be a part of a Content object.
- All objects have unique IDs within the file. The same Content object can be used in the same Container object.

A key feature to Interactive Content is its integration with the web. Interactive Content allows both displaying entire webpages as Content and retrieving native Content from web endpoints. This allows a single Interactive Content file to be used to define the structure of Content, yet have the content itself be up to date without changing the file itself. The file also supports self updating when opened by the client, meaning a single file can be used to always show up-to-date content.
      
The IC reader client is built using wxWidgets and is planned to be cross platform betwen Windows, Mac, and Linux.

The website also currently a work in progress, viewable [here](http://interactivecontent.org/).

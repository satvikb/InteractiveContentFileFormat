# Interactive Content File Format Specification
## Overview
- Interactive Content is based on a container based system. There are four main components in an Interactive Content file: Containers, Layouts, Links, and Content.
- Each container follows a defined Layout.  A container can include more Containers or have Content.
- Content objects must be inside a container for them to be displayed.
- Links are between containers, and must be a part of a Content object.
- All objects have unique IDs within the file. The same Content object can be used in the same Container object.

## Components
- A Element is referred to as either a Container or Content.

### Layouts
- A Layout defines the positions of Elements inside a Container. All containers must follow a Layout.

| Component Type | Layout ID | Number of Elements | xN      | yN      | widthN  | heightN | Infinite | startXN | startYN | widthN  | heightN | paddingN |
|----------------|-----------|--------------------|---------|---------|---------|---------|----------|---------|---------|---------|---------|----------|
| 3 Bits         | 13 Bits   | 8 Bits             | 16 Bits | 16 Bits | 16 Bits | 16 Bits | 8 Bits   | 16 Bits | 16 Bits | 16 Bits | 16 Bits | 16 Bits  |

#### Explanation
- `Component Type`: `011` is for Layouts
- `Layout ID`: Used to uniquely identify the Layout in the file. 
- `Number of Elements (N)`: Determines the number of Positions to expect for the rest of the Layout object.
- Position Object (there are N of the following). Each component of a position can be a number between 0 and 40,000. This number is multiplied with 0.0025 to get the final position float.
  - `xN`: This will become a float between 0 and 1, and a multiple of 0.0025. This defines the X position of the element.
  - `xY`: Same as above, but for the Y position.
  - `widthN`: The width of the element, also between 0 and 1.
  - `heightN`: The height of the element, between 0 and 1.
  - `Infinite`: An Infinite element supports any number of elements. Breaking down the byte like so: `ZYXW0000`
    - `Z bit` (Infinite bit): Decides if this element is an infinite element. If it is 0, the rest of the byte is ignored.
    - `Y bit`: Decides the direction of the content. 1 for vertical content (a list), 0 for horizontal content (slideshow).
    - `X bit`: Decides if paging is enabled. 1 for enabled. Paging means content should be shown in multiples of `widthN` or `heightN` (based on `Y bit`)
  - The following only applies if the Infinite bit is 1
    - `startXN`: This will become a float between 0 and 1. This defines the X position of the element.
    - `startYN`: Same as above, but for the Y position.
    - `widthN`: The width of the element, also between 0 and 1.
    - `heightN`: The height of the element, between 0 and 1.
    - `paddingN`: The padding between each element on the axis defined by the Direction bit.

### Containers
- A Container is a simple box that does not display anything itself. It only contains other Containers or Content.

| Component Type | Container ID | Layout Code | Layout ID | ID_N | End Container Code N | ID_K  | End Container Code K | End Chunk Code |
|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| 3 Bits         | 13 Bits   | 3 Bits             | 13 Bits | 16 Bits | 8 Bits | 16 Bits | 8 Bits |8 Bits|

#### Explanation
- `Component Type`: `001` is for Containers
- `Container ID`: Used to uniquely identify the Container in the file. 
- `Layout Code`: `011` is for Layouts
- `Layout ID`: The Layout this container will be using.
- Now continue until `Layout.NumberOfElements - 1` End Container bytes have been reached:
  - `Element ID 1`: Contains both the element type (first 3 bits) and its ID. This can be either another Container or Content.
  - `Element ID 2`: A second element can only be defined within a single Element if the Element is a Container and the Container has its Infinite bit to 1.
  - `End Container Code`: The byte `1111 1111` is used to signify all of the Elements have been read for the current Element.
- `End Container Code`: The byte `0000 0000` is used to signify the entire Container has been read.

### Content
- Content is the final underlying data that is to be displayed. Content is designed to be extensible, and therefore contains only the Content type, length, and its raw data.

| Component Type | Content ID | Content Type | Content Length | Content Data |
|----------------|-----------|--------------------|---------|---------|
| 3 Bits         | 13 Bits   | 8 Bits | 32 Bits | [Length] bytes|

#### Explanation
- `Component Type`: `010` is for Content
- `Container ID`: Used to uniquely identify the Content in the file. 
- `Content Type`: Used to identify the type of data the Content is. Content types 0 - 31 is reserved for the Interactive Content content types. Content types 32 - 255 are reserved for external file types (TBD).
- `Content Length`: Used to know how many bytes of data to read before the content ends.
- `Content Data`: Raw bytes of the Content, with [Length] number of bytes

### Links
- Links allow for redirection and alternative display modes of other containers.

| Component Type | LInk ID | Link To ID | Display Mode |
|-----|-----|-----|-----|
| 3 Bits         | 13 Bits   | 16 Bits | 8 bits|

#### Explanation
- `Component Type`: `100` is for Links
- `Link ID`: Used to uniquely identify the Link in the file. 
- `Link To ID`: The Container to link to. Links must always link to a container, so the first 3 bits will always be `001`.
- `Content Length`: Used to know how many bytes of data to read before the content ends.
- `Display Mode`: Breaking down the byte like so: `000VWXYZ`
  - `V bit`: Show this container as a tooltip. The display of the tooltip is left to the reader.
  - `W bit`: Allow showing a back button to return to the container before the Link was pressed.
  - `XYZ bits`: Defines how many sublevels of Containers that should be shown from the linked Container. The highest level Container that is chosen is what is displayed (since it will always contain the linked Container)
    - `000`: Show only the linked Container, even if the Container is inside a subcontainer
	  - `001`: Show the linked Container and the parent Container.
	  - `010`: The linked Container, with its parent and grandparent Container.
	  - `...`
	  - `110`: Show the 6th parent Container from the linked Container.
	  - `111`: Show the highest Container ancestor possible.

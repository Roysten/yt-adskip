# Description

YouTube ad skipping has worked for years using TamperMonkey scripts. Recently
YouTube started checking the isTrusted property of the click events that are
used to click the skip button. This boolean property reports whether the click
event was done by the user (true) or programmatically (false). It is also
unforgable: its value cannot be changed through JavaScript.

To get around this I took an existing TamperMonkey script for clicking on the
ad button and made it detect the exact coordinates of the skip button. These
coordinates are then sent to the simple HTTP server in this repository. This
HTTP server moves the cursor to the desired location and clicks. Who said the
isTrusted property is unforgable? :)

# Dependencies

- [FakeInput](https://github.com/uiii/FakeInput)
- [sockets.h](https://gist.github.com/willeccles/3ba0741143b573b74b1c0a7aea2bdb40)

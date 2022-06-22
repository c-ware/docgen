# CHANGELOG.md

VERSION: 1.0.6
DATE: June 21st, 2022

## Features
- You can now generate documentation for macro functions with the `macros`
  category.
- --md-mono option for enabling or disabling Monospace fonts for code

## Bug Fixes
- No longer raises an EOF error for a file where the comment is the only
  thing in the file. This was caused because the end of the C comment was
  quite literally at the end of the file.

## Misc
- License changed-- now using the C-Ware License.
- Complete overhaul of the internal functions for extracting individual tags
  into usable buffers. Reformatted the parameter order for them, as well as
  separated most of the error checking into their own functions to improve
  code readability.

## Documentation
- Added internal documentation for functions, albeit it is not generated
  *yet*.

## Work In Progress
- Markdown generator

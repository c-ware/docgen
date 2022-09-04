# NOTES.md

- A pipeline oriented implementation of docgen
- Each individual component feeds input into the next
- Components are unified through a shell script

- Extractor
    - Strips all non-docgen comments and code from the stdin
      by following these rules.

      1. Read line
      2. Does this line contain a @? (jump to 1 if no)
      3. Starting at the @, does the line contain @docgen\n? (jump to 5 if no)
      4. Invert the IN_DOCGEN_COMMENT boolean
      5. Are we in a docgen comment? (jump to 1 if no)
      6. Print everything after @
      7. Jump to 1

    - Outputs each line with its text after the @. Each "object" which are the tags
      extracted from a single comment is grouped in a pair of "@docgen" tags. Each
      object is not necessarily the same type to allow the compiler to handle embeds,
      as it needs to know each object to embed.

- Compiler
    - Translates each object of a certain type into a machine-readable format
    - Format consists of:
        - Global settings
        - List of section and body pairs
        - List of embeds for the synopsis
    - Most of the language-specific logic comes from the synopsis,
      and by proxy, embeds.
    - Calls on docgen library functions for generating the description, notes, etc
    - The compiler will output a format like this:
        SETTINGS 0x1 SETTING 0x1 VALUE 0x2 SETTING 0x1 VALUE 0x3
        SECTIONS 0x1 SECTIOB 0x1 BODY 0x2 SECTION 0x1 BODY 0x3
        EMBEDS 0x1 TYPE 0x1 EMBED 0x2 TYPE 0x1 EMBED
        0x4
        SETTINGS 0x1 SETTING 0x1 VALUE 0x2 SETTING 0x1 VALUE 0x3
        SECTIONS 0x1 SECTIOB 0x1 BODY 0x2 SECTION 0x1 BODY 0x3
        EMBEDS 0x1 TYPE 0x1 EMBED 0x2 TYPE 0x1 EMBED
        0x4
        ...

- Formatter
    - Translates each compiled object into an actual piece of documentation
      using the target's rules and syntax.
    - Iteratively loads each compiled object
    - Tags are extracted from the array with a "find_tag" functionn
    - Name of the file extracted from the "name" setting

We need to generate a block of settings, composed embeds, and sections
for each pair of @docgen_start and @docgen_end.

Embeds are a bit more complex. They require performing another scan across
the lines to determine two initial things:
    -


PARAM_START(

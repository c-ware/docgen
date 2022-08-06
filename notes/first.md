- What will need to be added?
    - Settings array and structure
    - Embeds array and structure
    - Tag array and structure

This change to docgen is a chance to think about how docgen's memory usage
could be improved. It currently regulars > 1 million bytes of memory, which
all things considered is not HORRIBLE, but it could be better. Not to the
point of squeezing every byte out, but just improvement in general.

A lot of the memory consumption in docgen seems to come from the embed
extraction. Holding arrays of giant proporations simply is not good for
overall memory consumption. I propose the following changes:
    - Use of a global libmatch buffer that holds the entire read file.
    - Extraction functions should operate under a 'get next comment'
      basis that allows 'retrieving' the next comment. This allows
      for things like
        - Searching for embeds
        - Searching for things to generate manuals for
      to happen entirely iteratively as opposed to extracting all of
      them first into a single buffer.

In order to make parsing less absolutely painful, I also propose a change
to docgen where error checkers are invoked over the buffer to make sure
that there are no glaring problems with the input before it is extracted
from. For example,
    - All multi-line comments are closed (remember to only count comments not in strings)
    - Tags do not appear on the same line as each other.
    - All multi-line tags are closed IN THE COMMENT THEY ARE DEFINED IN!!!
    - Multi-line tags have no holes in the middle of them (i.e lines without
      a @)
    - No tags that are unrecognized by everything (only check in docgen

Now, the real question is 'how do we make this work?' Well, this is a pretty difficult thing to do
as unlike the change to generators and post processors, extractors are at the bottom of the docgen
pipeline. We could to this two main ways:
    - Have an extractor that is converted into some of the types we target,
      like functions and projects.
    - Purge the old extractors, and re-fit the postprocessor and probably rewrite
      parts of it.

The first option would be much, much easier since it provides direct compatibility between the
interfaces of docgen-- it would definitely be less work. This being said, keeping the old
interface will most likely keep docgen from reaching its full potential. In the long run, I
think it would be best if we went with the second option.

So how do we do this? Well.
    - First, re-write the main function to NOT invoke the generator functions.
    - Do not purge the old structures just yet to keep compile compatibility and
      to keep our sanity.
    - Purge the old extractor functions
    - Add the new structures, and make the correct static buffers
    - Begin writing the new extractor function
    - Make sure its output is correct by printing its structure in the main function.

When this refactoring is done, the pipeline will look like this:

Extractor functions -> Error checking -> Postprocessing -> Writing

It is important to note that the structure generating during the extractor phase is actually
given to the postprocessor as opposed to having a function transform it. This essentially
means that the way we make different types of manuals is literally just by giving the postprocessor
an integer.

Something else that is important to note is that the structure of the `make_embedded_xxxxx` functions
has a pretty radical shift in design. Previously, these functions relied on a giant array containing
each respective type of 'thing' in the file. This lead to a lot of memory being used due to potentially
loading useless structures into memory. So what is the approach we should take now?

The base extractor functions will be based around iteration rather than complete extraction. If you wanted
to start extracting comments, you would make a new cursor with the buffer of memory that holds the entire
file loaded into memory's global variable. Once this is done, you repeatedly call the `get_next_comment`
function until there are no more left. This type of 'scanning' is definitely slower, but it greatly
reduces memory usage if the container that the function writes the comment data into is reused, which could
also be a global variable.

For example:

char *file_contents;
struct DocgenComment comment;

int main(void) {
    /* Load basic memory buffers into the global comment */
    comment_initialize(&comment);

    /* get_next_comment will return 1 or 0 depending on whether or not
       it could find a comment. */
    while(get_next_comment(&comment) != 0) {
        /* Do something with the comment */
    }

    /* At the end of the program */
    comment_free(&comment)

    return 0;
}

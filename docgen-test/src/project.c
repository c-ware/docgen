/*
 * @docgen: project
 * @brief: this is a test project
 * @name: test_project
 *
 * @embed function: test_function
 * @embed function: test_function
 * @embed constant: MY_CONSTANT
 * @embed constant: MY_OTHER_CONSTANT
 * @embed structure: MyStructure
 * @embed structure: MyStructure
 * @embed macro_function: test_macro
 *
 * @arguments
 * @These are the arguments to the program.
 * @arguments
 *
 * @description
 * @This is a description.
 * @This is another line
 * @This is also another line.
 * @table
 * @sep: ;
 * @a;b
 * @foo;bar
 * @baz;spam
 * @table
 * @description
 *
 * @reference: foo(bar)
 * @reference: baz(tuna)
*/

/*
 * @docgen: constant
 * @name: MY_CONSTANT
 * @brief: this is a constant
 * @value: 0
*/

/*
 * @docgen: constant
 * @name: MY_OTHER_CONSTANT
 * @brief: this is a constant
 * @value: 0
*/

/*
 * @docgen: function
 * @brief: this is a test function
 * @name: test_function
 *
 * @include: foo.h
 *
 * @description
 * @This is a description.
 * @description
 *
 * @example
 * @This is an example.
 * @example
 *
 * @notes
 * @These are the notes for this function.
 * @notes
 *
 * @error: you fail to use it
 * @error: foo bar
 *
 * @return: some data
 * @type: struct pollfd
*/

/*
 * @docgen: structure
 * @brief: this is a structure
 * @name: MyStructure
 *
 * @field foo: the foo field
 * @type: int
 *
 * @field bar: the bar field
 * @type: struct Foobar *
 *
 * @struct_start
 * @brief: this is a substructure
 * @name: MySubStructure
 *
 * @field baz: the baz field
 * @type: int
 *
 * @field tuna: the tuna field
 * @type: struct Foobar *
 * @struct_end
*/

/*
 * @docgen: macro_function
 * @brief: this is a macro function
 * @name: test_macro
 *
 * @include: foo
 *
 * @description
 * @This is a description
 * @description
 *
 * @notes
 * @These are the notes for the macro function
 * @notes
 *
 * @example
 * @This is an example.
 * @example
 *
 * @error: this is an error
 * @error: this is another error
 *
 * @param foo: the foo parameter
*/

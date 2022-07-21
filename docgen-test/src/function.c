/*
 * @docgen: function
 * @brief: this is a test function
 * @name: test_function
 *
 * @embed function: foo_function
 * @embed structure: SomeStructure
 * @embed macro_function: foobar_baz_function
 * @embed constant: MY_CONSTANT
 *
 * @setting: func-briefs
 * @setting: mfunc-briefs
 * @setting: struct-briefs
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
 * @param foo: the foo parameter
 * @type: const char *
 *
 * @param bar: the bar parameter
 * @type: unsigned int
 *
 * @return: some data
 * @type: struct pollfd
*/

/*
 * @docgen: function
 * @brief: foo bar baz
 * @name: foo_function
 *
 * @description
 * @foo bar baz
 * @description
 *
 * @param x: the x parameter
 * @type: int
 * 
 * @param y: the y parameter
 * @type: int
 *
 * @return: some value
 * @type: int
*/

/*
 * @docgen: structure
 * @brief: this is a structure
 * @name: SomeStructure
 *
 * @field foo: bar
 * @type: int
 *
 * @field baz: tuna
 * @type: const char *
 *
 * @struct_start
 * @brief: some nested one
 * @name: NestedSomeStructure
 *
 * @field thud: waldo
 * @type: struct SomeStructure *
 *
 * @field foobar: quz
 * @type: void *
 *
 * @struct_end
 *
 * @struct_start
 * @brief: some nested one
 * @name: Beep
 *
 * @field thud: waldo
 * @type: struct SomeStructure *
 *
 * @field foobar: quz
 * @type: void *
 *
 * @struct_start
 * @brief: some nested one
 * @name: Boop
 *
 * @field thud: waldo
 * @type: struct SomeStructure *
 *
 * @field foobar: quz
 * @type: void *
 *
 * @struct_start
 * @brief: some nested one
 * @name: Bop
 *
 * @field thud: waldo
 * @type: struct SomeStructure *
 *
 * @field foobar: quz
 * @type: void *
 *
 * @struct_end
 *
 * @struct_end
 *
 * @struct_end
*/

/*
 * @docgen: macro_function
 * @brief: this is macro function
 * @name: foobar_baz_function
 *
 * @description
 * @This is a macro function.
 * @description
 *
 * @param foo: this is the foo parameter
 * @param bar: this is the bar parameter
*/

/*
 * @docgen: constant
 * @brief: this is a constant
 * @name: MY_CONSTANT
 * @value: "foo"
 *
 * @setting: ifndef
*/

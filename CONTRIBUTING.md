#Style guidelines

##General Rules

Line width is typically 100 characters. Please do not use 80-characters.
Don't ever introduce new external dependencies into Core
Don't use any platform specific code in Core
Use namespaces often
Avoid the use of C-style casts and instead prefer C++-style static_cast and reinterpret_cast. Try to avoid using dynamic_cast. Never use const_cast except for when dealing with external const-incorrect APIs.

##Naming Rules

Functions: PascalCase
Variables: lower_case_underscored. Prefix with g_ if global.
Classes: PascalCase
Files and Directories: lower_case_underscored
Namespaces: PascalCase, _ may also be used for clarity (e.g. ARM_InitCore)

#Indentation/Whitespace Style

Follow the indentation/whitespace style shown below. Do not use tabs, use 4-spaces instead.
Comments

For regular comments, use C++ style (//) comments, even for multi-line ones.
For doc-comments (Doxygen comments), use /// if it's a single line, else use the /** */ style featured in the example. Start the text on the second line, not the first containing /**.
For items that are both defined and declared in two separate files, put the doc-comment only next to the associated declaration. (In a header file, usually.) Otherwise, put it next to the implementation. Never duplicate doc-comments in both places.

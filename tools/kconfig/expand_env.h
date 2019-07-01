#pragma once

/* Expand any $ENV type environment variables in 'input',
   return a newly allocated buffer with the result.

   Buffer should be freed after use.

   This is very basic expansion, doesn't do escaping or anything else.
*/
char *expand_environment(const char *input, const char *src_name, int src_line_no);

/* Free a buffer allocated by expand_environment */
void free_expanded(char *expanded);

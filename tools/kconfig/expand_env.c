#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

#include "expand_env.h"

static bool allowed_env_var_name(char c)
{
  return c != '\0' &&
    !isblank(c) &&
    !iscntrl(c) &&
    c != '/' &&
    c != '\\' &&
    c != '=' &&
    c != '$';
}

#define MAX_LEN (128 * 1024) /* Longest a result can expand to */

/* Very basic expansion that looks for variable references like $NAME and expands them
 *
 */
char *expand_environment(const char *input, const char *src_name, int src_line_no)
{
  char *result = malloc(MAX_LEN);

  char *out = result;
  const char *in = input;

  while (*in != '\0') {
    // check for buffer overflow
    if (out >= result + MAX_LEN - 1) {
      goto too_long;
    }

    if (*in != '$') {
      // not part of an environment variable name, copy directly
      *out++ = *in++;
      continue;
    }

    // *in points to start of an environment variable reference
    in++;
    const char *env_start = in;
    while (allowed_env_var_name(*in)) { // scan to the end of the name
      in++;
    }
    size_t env_len = in - env_start;

    // make a buffer to hold the environment variable name
    //
    // strndup is not available on mingw32, apparently.
    char *env_name = calloc(1, env_len + 1);
    assert(env_name != NULL);
    strncpy(env_name, env_start, env_len);

    const char *value = getenv(env_name);
    if (value == NULL || strlen(value) == 0) {
      printf("%s:%d: undefined environment variable \"%s\"\n",
             src_name, src_line_no, env_name);
      exit(1);
    }
    free(env_name);
    if (out + strlen(value) >= result + MAX_LEN - 1) {
      goto too_long;
    }
    strcpy(out, value); // append the value to the result (range checked in previous statement)
    out += strlen(value);
  }

  *out = '\0'; // null terminate the result string

  return result;

too_long:
  printf("%s:%d: Expansion is longer than %d bytes\n",
         src_name, src_line_no, MAX_LEN);
  free(result);
  exit(1);
}

void free_expanded(char *expanded)
{
  free(expanded);
}

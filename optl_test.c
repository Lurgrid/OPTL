#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "optl.h"

struct context {
  long int val;
  const char *str;
};

//  test_aopt : Returns 0 if all the nmemb options in the aopt array are not 
//    NULL, otherwise returns 1.
static int test_aopt(const optparam **aopt, size_t nmemb);

//  version : Displays the compilation date of the executable and the version of
//    the compiler use and return 0.
static int version(void *cntxt, const char *value, const char **err);

//  get_int: Tries to convert value to an integer of type long int. In case of
//    success cntxt->val is the converted value and returns 0, otherwise returns
//    a non-null value and *err points to the associated error message.
static int get_int(struct context *cntxt, const char *value, const char **err);

//  get_string: Put the value of value in cntxt->str and return 0.
static int get_string(struct context *cntxt, const char *value,
    const char **err);

int main(int argc, char **argv) {
  const optparam *aopt[] = {
    opt_init('V', "version", "Show version", false, true, version),
    opt_init('g', "give-int", "Set a value to cnt.val", true, false,
        (int (*)(void *, const char *, const char **))get_int),
    opt_init('G', "give-string", "Set a value to cnt.str", true, false,
        (int (*)(void *, const char *, const char **))get_string)
  };
  if (test_aopt(aopt, sizeof(aopt) / sizeof(*aopt)) != 0) {
    fprintf(stderr, "Error : Not enougth memory\n");
    return EXIT_FAILURE;
  }
  struct context cnt = {
    .val = 0,
    .str = "default"
  };
  const char *err;
  optreturn r = opt_process(argc - 1, argv + 1, aopt,
          sizeof(aopt) / sizeof(*aopt), NULL, &cnt, &err, "-", "--",
          "./test [-a] [-b param] [-ab] [--aaaa] [--bbbb=param]\n",
          "Test program for the management of options by the optl module.\n");
  switch (r) {
    case ERROR_HDL:
      fprintf(stderr,
          "Error: in processing function\n -- %s --\n", err);
      return EXIT_FAILURE;
    case ERROR_AMB:
      fprintf(stderr, "Option %s, is ambiguous\n", err);
      return EXIT_FAILURE;
    case ERROR_PARAM:
      fprintf(stderr, "Option %s, did not receive a parameter\n", err);
      return EXIT_FAILURE;
    case ERROR_UNKNOWN:
      fprintf(stderr,
          "The option %s, is not supported by the program\n", err);
      return EXIT_FAILURE;
    case ERROR_DEFAULT:
      printf("An error occurred while executing the processing of what was not"
          " an option.\n");
      break;
    case STOP_PROCESS:
      printf("The program was terminated due to the use of an option that "
          "stipulate to end it.\n");
      break;
    case DONE:
      printf("All is well.\n");
      printf("val: %ld, str: %s\n", cnt.val, cnt.str);
      break;
  }
  return EXIT_SUCCESS;
}

int test_aopt(const optparam **aopt, size_t nmemb) {
  for (size_t k = 0; k < nmemb; ++k) {
    if (aopt[k] == NULL) {
      return -1;
    }
  }
  return 0;
}

static int get_int(struct context *cntxt, const char *value, const char **err) {
  errno = 0;
  long int val;
  char *endptr;
  val = strtol(value, &endptr, 10);
  if (*endptr != '\0') {
    *err = "Incorrect value";
    return -1;
  }
  if (errno != 0) {
    *err = "Value too large, Out of range";
    return -1;
  }
  cntxt->val = val;
  return 0;
}

static int get_string(struct context *cntxt, const char *value,
    __attribute__((unused))const char **err) {
  cntxt->str = value;
  return 0;
}

int version(__attribute__((unused)) void *cntxt, __attribute__(
      (unused))const char *value, __attribute__((unused))const char **err) {
  printf("build on %s with ", __DATE__);
#ifdef __clang__
  printf("clang %d.%d.%d\n", __clang_major__, __clang_minor__,
      __clang_patchlevel__);
#elif defined(__GNUC__)
  printf("gcc %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
  printf("Unknow Compiler\n");
#endif
  return 0;
}

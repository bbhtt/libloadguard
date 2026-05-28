#include <dlfcn.h>
#include <stdio.h>

int
main (int argc, char *argv[])
{
  if (argc != 2)
    return 1;

  void *h = dlopen (argv[1], RTLD_NOW);

  if (h == NULL)
    {
      fprintf (stderr, "dlopen failed: %s\n", dlerror ());
      return 1;
    }

  dlclose (h);
  return 0;
}

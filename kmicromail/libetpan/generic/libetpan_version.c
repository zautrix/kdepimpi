#include "libetpan_version.h"

#ifndef CONFIG_H
#define CONFIG_H
#include "config.h"
#endif

/* version of libEtPan! at runtime */

int libetpan_get_version_major(void)
{
  return LIBETPAN_VERSION_MAJOR;
}

int libetpan_get_version_minor(void)
{
  return LIBETPAN_VERSION_MINOR;
}

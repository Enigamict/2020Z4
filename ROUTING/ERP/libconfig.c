#include <stdlib.h>
#include <stdio.h>
#include "config.h"

int main()
{
  struct config cfg;
  int ret = config_parse(&cfg,"./setting.json");
  if (ret < 0) {
    fprintf(stderr, "failed on config parser\n");
    return 1;
  }
  
 // print_config(&cfg);
}

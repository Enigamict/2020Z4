#include <stdlib.h>
#include <stdio.h>
#include "config.h"

int main()
{
  struct config cfg;
  config_parse(&cfg, "./setting.json");
  
 // print_config(&cfg);
}

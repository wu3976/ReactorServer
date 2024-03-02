#pragma once
#include <stdio.h>
#define SHOWERR(errstr) printf("%s, %s, at line %d: %s", __FILE__, __FUNCTION__, __LINE__, errstr);
#include "stdafx.h"
#include "common.h"
#define flash_ModuleDefinition

#define EXTERN extern

#undef EXTERN

#define EXTERN 
#include "Flash.h"
#undef EXTERN


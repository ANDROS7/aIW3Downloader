// Hedgehogscience.com
#pragma once

#include "np\libnp.h"

struct np_header
{
	unsigned char key[32];
	unsigned char iv[32];
	unsigned char sig[128];
};

bool IsEncrypted(NPGetPublisherFileResult *File);
bool Decrypt(NPGetPublisherFileResult *File, unsigned char *OutBuffer);
// Hedgehogscience.com

#include "..\Include\Crypto.h"
#include <iostream>
#include <fstream>

#pragma comment(lib, "Dependencies\\tomcrypt")
#pragma comment(lib, "Dependencies\\tommath")
#pragma comment(lib, "Dependencies\\libnp")

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Please supply a filename to start downloading.\n";
		return 0;
	}

	if (!NP_Init())
	{
		std::cout << "NP failed to init, this shouldn\'t happen.\n";
		return 0;
	}

	if (!NP_Connect("server.aiw3.net", 3036))
	{
		std::cout << "Failed to connect to the aIW3 server, it\'s probably down.\n";
		return 0;
	}

	NPAsync<NPAuthenticateResult>* Auth = NP_AuthenticateWithLicenseKey("");
	Auth->Wait();

	NPAuthenticateResult *AR = Auth->GetResult();
	if (AR->result != 0)
	{
		std::cout << "Failed to authenticate, aIW3 must have updated.\n";
		return 0;
	}

	unsigned char *Buffer = (unsigned char *)malloc(0x400000);
	NPAsync<NPGetPublisherFileResult>* async = NP_GetPublisherFile(argv[1], Buffer, 0x400000);
	async->Wait();

	NPGetPublisherFileResult *Res = async->GetResult();
	switch (Res->result)
	{
	case 0:
		std::cout << "File status: GetFileResultOK\n";
		break;
	case 1:
		std::cout << "File status: GetFileResultNotFound\n";
		return 0;
	case 2:
		std::cout << "File status: GetFileResultNotAllowed\n";
		return 0;
	case 3:
		std::cout << "File status: GetFileResultServiceError\n";
		return 0;
	default:
		break;
	}

	std::ofstream OutputFile(argv[1], std::ios::binary);
	unsigned char *OutBuffer;

	if (IsEncrypted(Res))
	{
		std::cout << "The file is encrypted and needs to be decrypted, this will only take a second.\n";

		OutBuffer = (unsigned char *)malloc(Res->fileSize);
		if (!Decrypt(Res, OutBuffer))
		{
			std::cout << "Something went very wrong while decrypting, please try again.\n";
			return 0;
		}
		OutputFile.write((const char *)OutBuffer, Res->fileSize - sizeof(np_header));
	}
	else
		OutputFile.write((const char *)Res->buffer, Res->fileSize - sizeof(np_header));

	std::cout << "Writing file: " << argv[1] << ".\n";
	OutputFile.close();


	return 1;
}

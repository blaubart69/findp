#include "stdafx.h"

void PrintEntry(LSTR *FullBaseDir, WIN32_FIND_DATA *finddata, LineWriter *lineWriter)
{
	//logger->writeLine(L"%s\\%s", FullBaseDir, finddata->cFileName);
	lineWriter->append(FullBaseDir->str, FullBaseDir->len);
	lineWriter->appendf(L"\\%s\r\n", finddata->cFileName);
	lineWriter->write();
}
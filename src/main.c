/**
 * @author zuri
 * @date dimanche 25 novembre 2018, 23:59:47 (UTC+0100)
 */
#include <minunit/minunit.h>

#include <zrlib/base/macro.h>
#include "base/ArrayOpTests.h"
#include "base/BitsTests.h"
#include "base/MapTests.h"
#include "base/MemoryOpTests.h"
#include "base/MPoolTests.h"
#include "base/Identifier/IdentifierTests.h"
#include "base/Identifier/IDGenerator/IDGeneratorTests.h"
#include "base/Graph/GraphTests.h"
#include "base/Tree/TreeTests.h"
#include "base/Vector/VectorTests.h"

char MESSAGE_BUFF[2048];
char FUN_PREFIX[1024];

int main(int argc, char **argv)
{
	zrlib_initCurrentThread();
	int (*testSuites[])(void) =
	{	//
			BitsTests,//
			MemoryOpTests,//
			MemoryArrayOpTests,//
			VectorTests,//
			MapTests,//
			IdentifierTests,
			MPoolTests,//
			GraphTests,//
			TreeTests,//
			IDGeneratorTests,
	};
	int i = 0;
	const int nbof = ZRCARRAY_NBOBJ(testSuites);

	for (; i < nbof; i++)
	{
		int status = testSuites[i]();

		if (status)
			fprintf(stderr, "Error for test suite %d, status %d\n", i, status);
	}
	zrlib_endCurrentThread();
	return 0;
}

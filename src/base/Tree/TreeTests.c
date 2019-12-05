/**
 * @author zuri
 * @date mardi 3 décembre 2019, 20:53:38 (UTC+0100)
 */

#include <zrlib/base/Graph/Tree/SimpleTree.h>
#include <zrlib/base/macro.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include "../../main.h"

#include <stdio.h>

// ============================================================================

ZRAllocator *ALLOCATOR;

// ============================================================================

void repeatChar(char const *s, unsigned repeat)
{
	while (repeat--)
		fputs(s, stdout);
}

void printTree(ZRTree *tree, ZRTreeNode *node, unsigned depth)
{
	size_t nbChilds = ZRTreeNode_getNbChilds(tree, node);
	size_t i;

	double val = *(double*)ZRTreeNode_getObj(tree, node);
	repeatChar(" ", 2 * depth);
	printf("%lf\n", val);

	for (i = 0; i < nbChilds; i++)
	{
		ZRTreeNode *child = ZRTreeNode_getChild(tree, node, i);
		printTree(tree, child, depth + 1);
	}
}

MU_TEST(testGet)
{
	size_t const objSize = sizeof(double);
	ZRTreeBuilder *builder = ZRSimpleTreeBuilder_create(objSize, ALLOCATOR);
	double val = 1.0;

	ZRTreeBuilder_node(builder, &val);
	__ val = 12.5;
	__ ZRTreeBuilder_node(builder, &val);
	__ mu_check(*(double* )ZRTreeBuilder_currentObj(builder) == val);
	__ ZRTreeBuilder_end(builder);
	__ val = 120;
	__ ZRTreeBuilder_node(builder, &val);
	__ mu_check(*(double* )ZRTreeBuilder_currentObj(builder) == val);
	____ val = 1.12345;
	____ ZRTreeBuilder_node(builder, &val);
	____ mu_check(*(double* )ZRTreeBuilder_currentObj(builder) == val);
	____ ZRTreeBuilder_end(builder);
	__ ZRTreeBuilder_end(builder);
	__ val = 1000.25;
	__ ZRTreeBuilder_node(builder, &val);
	__ mu_check(*(double* )ZRTreeBuilder_currentObj(builder) == val);
	ZRTreeBuilder_end(builder);
	ZRTree *tree = ZRTreeBuilder_new(builder);

	printTree(tree, tree->root, 0);

	ZRSimpleTreeBuilder_destroy(builder);
	ZRTree_destroy(tree);
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testGet);
}

int TreeTests(void)
{
	puts(__FUNCTION__);

	ALLOCATOR = malloc(sizeof(*ALLOCATOR));
	ZRCAllocator_init(ALLOCATOR);

	MU_RUN_SUITE(AllTests);
	MU_REPORT()
	;

	free(ALLOCATOR);
	return minunit_status;
}

/**
 * @author zuri
 * @date mardi 3 décembre 2019, 20:53:38 (UTC+0100)
 */

#include <zrlib/base/Graph/Tree/SimpleTree.h>
#include <zrlib/base/macro.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include "../../main.h"

#include <stdalign.h>
#include <math.h>
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
	size_t nbChilds = ZRGraphNode_getNbChilds(ZRTREE_GRAPH(tree), node);
	size_t i;

	double val = *(double*)ZRGraphNode_getObj(ZRTREE_GRAPH(tree), node);
	repeatChar(" ", 2 * depth);
	printf("%lf\n", val);

	for (i = 0; i < nbChilds; i++)
	{
		ZRTreeNode *child = ZRGraphNode_getChild(ZRTREE_GRAPH(tree), node, i);
		printTree(tree, child, depth + 1);
	}
}

static int cmpLong(long *a, long *b)
{
	return *a - *b;
}

static void printLong(char *out, long *val)
{
	sprintf(out, "%ld", *val);
}
static int cmpChar(char *a, char *b)
{
	return *a - *b;
}

static void printChar(char *out, char *c)
{
	*out = *c;
}

MU_TEST(testGet)
{
	ZRTEST_BEGIN();
	size_t const objSize = sizeof(double);
	ZRTreeBuilder *builder = ZRSimpleTreeBuilder_create(objSize, alignof(double), 0, 0, ALLOCATOR);
	double val = 1.0;

	ZRTreeBuilder_node(builder, &val, NULL);
	mu_assert_double_eq(val, *(double* )ZRTreeBuilder_currentObj(builder));
	__ val = 12.5;
	__ ZRTreeBuilder_node(builder, &val, NULL);
	mu_assert_double_eq(val, *(double* )ZRTreeBuilder_currentObj(builder));
	__ ZRTreeBuilder_end(builder);
	__ val = 120;
	__ ZRTreeBuilder_node(builder, &val, NULL);
	mu_assert_double_eq(val, *(double* )ZRTreeBuilder_currentObj(builder));
	____ val = 1.12345;
	____ ZRTreeBuilder_node(builder, &val, NULL);
	mu_assert_double_eq(val, *(double* )ZRTreeBuilder_currentObj(builder));
	____ ZRTreeBuilder_end(builder);
	__ ZRTreeBuilder_end(builder);
	__ val = 1000.25;
	__ ZRTreeBuilder_node(builder, &val, NULL);
	mu_assert_double_eq(val, *(double* )ZRTreeBuilder_currentObj(builder));
	ZRTreeBuilder_end(builder);
	ZRTree *tree = ZRTreeBuilder_new(builder);

	printTree(tree, tree->root, 0);

	ZRTreeBuilder_destroy(builder);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testNNodes)
{
	ZRTEST_BEGIN();
	size_t const objSize = sizeof(char);
	ZRTreeBuilder *builder = ZRSimpleTreeBuilder_create(objSize, alignof(char), 0, 0, ALLOCATOR);
	char val = 'a';
	size_t const nb = 10;
	ZRTreeNode *nodes_p[nb];

	size_t i;

	for (i = 0; i < nb; i++, val++)
		ZRTreeBuilder_node(builder, &val, NULL);

	ZRTree *tree = ZRTreeBuilder_new(builder);
	ZRGraph_getNNodes(ZRTREE_GRAPH(tree), nodes_p, 0, nb);

	for (i = 0, val = 'a'; i < nb; i++, val++)
	{
		char *c = ZRGraphNode_getObj(ZRTREE_GRAPH(tree), nodes_p[i]);
		ZRTEST_PRINTF("%c == %c", val, *c);
		ZRTEST_RESULT_6(FUN_PREFIX, MESSAGE_BUFF, &val, c, (int (*)(void*, void*) )cmpChar, (void (*)(char*, void*) )printChar);
	}
	ZRTreeBuilder_destroy(builder);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testNEdges)
{
	ZRTEST_BEGIN();
	size_t const objSize = sizeof(char);
	ZRTreeBuilder *builder = ZRSimpleTreeBuilder_create(objSize, alignof(char), 0, 0, ALLOCATOR);
	char val = 'a';
	size_t const nb = 10;
	ZRGraphEdge edges[nb];

	size_t i;

	for (i = 0; i <= nb; i++, val++)
		ZRTreeBuilder_node(builder, &val, NULL);

	ZRTree *tree = ZRTreeBuilder_new(builder);
	size_t nbCpy = ZRGraph_cpyNEdges(ZRTREE_GRAPH(tree), edges, 0, nb);
	mu_assert_int_eq(nb, nbCpy);
	ZRTreeBuilder_destroy(builder);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testNodeNEdges)
{
	ZRTEST_BEGIN();
	ZRTreeBuilder *builder = ZRSimpleTreeBuilder_create(sizeof(double), alignof(double), 0, 0, ALLOCATOR);
	double val = 15.0;
	size_t const nb = 10;
	ZRGraphEdge edges[nb + 1];

	ZRTreeBuilder_node(builder, &val, NULL);
	ZRTreeBuilder_node(builder, &val, NULL);
	val = 0;

	for (size_t i = 0; i < nb; i++, val += 1)
	{
		ZRTreeBuilder_node(builder, &val, NULL);
		ZRTreeBuilder_end(builder);
	}
	ZRTree *tree = ZRTreeBuilder_new(builder);
	ZRGraphNode *node = ZRTREENODE_FROMARRAYCOORDINATE(tree, (size_t[] ) { 0 });
	printTree(tree, tree->root, 0);
	size_t nbCpy;
	//IN
	{
		nbCpy = ZRGraphNode_cpyNEdges(ZRTREE_GRAPH(tree), node, edges, 0, ZRCARRAY_NBOBJ(edges), ZRGraphEdge_selectIN);
		mu_assert_int_eq(1, nbCpy);
		mu_check(edges->a == tree->root);
		mu_check(edges->b == node);
	}
	//OUT
	{
		nbCpy = ZRGraphNode_cpyNEdges(ZRTREE_GRAPH(tree), node, edges, 0, ZRCARRAY_NBOBJ(edges), ZRGraphEdge_selectOUT);
		mu_assert_int_eq(nb, nbCpy);

		for (size_t i = 0; i < nb; i++)
		{
			ZRTEST_PRINTF("[%lu] %p == %p", i, node, edges[i].a);
			ZRTEST_CHECK(edges[i].a == node);
		}
	}
	//INOUT
	{
		nbCpy = ZRGraphNode_cpyNEdges(ZRTREE_GRAPH(tree), node, edges, 0, ZRCARRAY_NBOBJ(edges), ZRGraphEdge_selectINOUT);
		mu_assert_int_eq(nb + 1, nbCpy);
		mu_check(edges->a == tree->root);
		mu_check(edges->b == node);

		for (size_t i = 0; i < nb; i++)
		{
			ZRTEST_PRINTF("[%lu] %p == %p", i, node, edges[i].a);
			ZRTEST_CHECK(edges[i + 1].a == node);
		}
	}
	ZRTreeBuilder_destroy(builder);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testNewBuilder)
{
	ZRTEST_BEGIN();
	double val;
	size_t const objSize = sizeof(val);
	ZRTree *tree;
	ZRTreeBuilder *builder;
	builder = ZRSimpleTreeBuilder_create(objSize, alignof(double), 0, 0, ALLOCATOR);

	val = 1;
	ZRTreeBuilder_node(builder, &val, NULL);
	val = 2;
	ZRTreeBuilder_node(builder, &val, NULL);
	ZRTreeBuilder_end(builder);
	val = 3;
	ZRTreeBuilder_node(builder, &val, NULL);

	tree = ZRTreeBuilder_new(builder);
	ZRTreeBuilder_destroy(builder);

//	printTree(tree, tree->root, 0);

	size_t coordinate[] = { 0, };
	builder = ZRTree_newBuilder(tree, ZRTREENODE_FROMARRAYCOORDINATE(tree, coordinate));

	val = 4;
	ZRTreeBuilder_node(builder, &val, NULL);
	ZRTreeBuilder_end(builder);
	val = 5;
	ZRTreeBuilder_node(builder, &val, NULL);
	ZRTreeBuilder_end(builder);

	ZRGraph_destroy(ZRTREE_GRAPH(tree));
	tree = ZRTreeBuilder_new(builder);

//	printTree(tree, tree->root, 0);

	ZRTreeNode *newNode = ZRTREENODE_FROMARRAYCOORDINATE(tree, coordinate);
	mu_check(ZRGraphNode_getNbChilds(ZRTREE_GRAPH(tree), newNode) == 2);
	mu_check(*(double* )ZRGraphNode_getObj(ZRTREE_GRAPH(tree), ZRGraphNode_getChild(ZRTREE_GRAPH(tree), newNode, 0)) == 4);
	mu_check(*(double* )ZRGraphNode_getObj(ZRTREE_GRAPH(tree), ZRGraphNode_getChild(ZRTREE_GRAPH(tree), newNode, 1)) == 5);

	ZRTreeBuilder_destroy(builder);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testIteratorChilds)
{
	ZRTEST_BEGIN();
	long val;
	size_t const objSize = sizeof(long);
	ZRTreeBuilder *builder = ZRSimpleTreeBuilder_create(objSize, alignof(long), 0, 0, ALLOCATOR);
	size_t const nb = 10;
	char objs[nb];

	val = 0;
	ZRTreeBuilder_node(builder, &val, NULL);

	for (size_t i = 0; i < nb; i++)
	{
		val++;
		ZRTreeBuilder_node(builder, &val, NULL);
		ZRTreeBuilder_end(builder);
	}
	ZRTree *tree = ZRTreeBuilder_new(builder);
	ZRTreeBuilder_destroy(builder);

	ZRIterator *childs_it = ZRTreeNode_getChilds(tree, tree->root);
	size_t nbChilds_it = 0;
	val = 1;

	while (ZRIterator_hasNext(childs_it))
	{
		nbChilds_it++;
		ZRIterator_next(childs_it);
		long tmp = *(long*)ZRGraphNode_getObj(ZRTREE_GRAPH(tree), ZRIterator_current(childs_it));
		ZRTEST_PRINTF("%ld == %ld", val, tmp);
		ZRTEST_RESULT_6(FUN_PREFIX, MESSAGE_BUFF, &tmp, &val, (int (*)(void*, void*) )cmpLong, (void (*)(char*, void*) )printLong);
		val++;
	}
	mu_check(nbChilds_it == nb);
	ZRIterator_destroy(childs_it);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testIteratorAscendants)
{
	ZRTEST_BEGIN();
	long val;
	size_t const objSize = sizeof(long);
	ZRTreeBuilder *builder = ZRSimpleTreeBuilder_create(objSize, alignof(long), 0, 0, ALLOCATOR);
	size_t const nb = 10;
	size_t const offset_end = 2;
	char objs[nb];
	size_t coord[nb - offset_end - 1];

	memset(coord, 0, sizeof(coord));
	val = 0;

	for (size_t i = 0; i < nb; i++)
	{
		val++;
		ZRTreeBuilder_node(builder, &val, NULL);
	}
	ZRTree *tree = ZRTreeBuilder_new(builder);
	ZRTreeBuilder_destroy(builder);

	ZRIterator *asc_it = ZRTreeNode_getAscendants(tree, ZRTreeNode_getNodeFromCoordinate(tree, ZRCARRAY_NBOBJ(coord), coord));
	size_t nbAscendants_it = 0;

	val -= offset_end;

	while (ZRIterator_hasNext(asc_it))
	{
		nbAscendants_it++;
		ZRIterator_next(asc_it);
		long tmp = *(long*)ZRGraphNode_getObj(ZRTREE_GRAPH(tree), ZRIterator_current(asc_it));
		ZRTEST_PRINTF("%ld == %ld", val, tmp);
		ZRTEST_RESULT_6(FUN_PREFIX, MESSAGE_BUFF, &tmp, &val, (int (*)(void*, void*) )cmpLong, (void (*)(char*, void*) )printLong);
		val--;
	}
	mu_check(nbAscendants_it == nb - offset_end);
	ZRIterator_destroy(asc_it);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

static ZRTree* makeDoubleTree(void)
{
	double val;
	size_t const objSize = sizeof(double);
	ZRTreeBuilder *builder = ZRSimpleTreeBuilder_create(objSize, alignof(double), 0, 0, ALLOCATOR);

	val = 1, ZRTreeBuilder_node(builder, &val, NULL);
	val = 11, ZRTreeBuilder_node(builder, &val, NULL);
	val = 111, ZRTreeBuilder_node(builder, &val, NULL), ZRTreeBuilder_end(builder);
	val = 112, ZRTreeBuilder_node(builder, &val, NULL);
	val = 1121, ZRTreeBuilder_node(builder, &val, NULL), ZRTreeBuilder_end(builder);
	val = 1122, ZRTreeBuilder_node(builder, &val, NULL), ZRTreeBuilder_end(builder);
	ZRTreeBuilder_end(builder);
	val = 113, ZRTreeBuilder_node(builder, &val, NULL), ZRTreeBuilder_end(builder);
	ZRTreeBuilder_end(builder);
	val = 12, ZRTreeBuilder_node(builder, &val, NULL);
	val = 121, ZRTreeBuilder_node(builder, &val, NULL);
	val = 1211, ZRTreeBuilder_node(builder, &val, NULL), ZRTreeBuilder_end(builder);
	val = 1212, ZRTreeBuilder_node(builder, &val, NULL), ZRTreeBuilder_end(builder);

	ZRTree *tree = ZRTreeBuilder_new(builder);
	ZRTreeBuilder_destroy(builder);
	return tree;
}

MU_TEST(testIteratorDescendants)
{
	ZRTEST_BEGIN();
	ZRTree *tree = makeDoubleTree();
	size_t coord[] = { 0 };
	ZRIterator *it = ZRTreeNode_getDescendants(tree, ZRTREENODE_FROMARRAYCOORDINATE(tree, coord));
	size_t nb = 0;
	long ref = 11;

	for (; ZRIterator_hasNext(it); nb++)
	{
		ZRIterator_next(it);
		double val = *(double*)ZRGraphNode_getObj(ZRTREE_GRAPH(tree), ZRIterator_current(it));

		long val_size = log10(val) + 1;
		long prefix = 0;

		if (val_size > 2)
			prefix = val / pow(10, val_size - 2);
		else
			prefix = val;

		ZRTEST_RESULT_6(FUN_PREFIX, MESSAGE_BUFF, &prefix, &ref, (int (*)(void*, void*) )cmpLong, (void (*)(char*, void*) )printLong);
	}
	mu_check(nb == 6);
	mu_check(ZRIterator_hasNext(it) == false);
	ZRIterator_destroy(it);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testIteratorDescendantsBF)
{
	ZRTEST_BEGIN();
	ZRTree *tree = makeDoubleTree();
	size_t coord[] = { 0 };
	ZRIterator *it = ZRTreeNode_getDescendants_BF(tree, ZRTREENODE_FROMARRAYCOORDINATE(tree, coord));

	long refs[] = { 11, 111, 112, 113, 1121, 1122 };
	size_t const ref_nb = ZRCARRAY_NBOBJ(refs);
	size_t refs_i;
	size_t nb;

	for (refs_i = 0, nb = 0; ZRIterator_hasNext(it); nb++)
	{
		ZRIterator_next(it);

		if (nb == ref_nb)
			continue;

		double val = *(double*)ZRGraphNode_getObj(ZRTREE_GRAPH(tree), ZRIterator_current(it));
		long val_l = val;

		ZRTEST_RESULT_6(FUN_PREFIX, MESSAGE_BUFF, &val_l, &refs[refs_i++], (int (*)(void*, void*) )cmpLong, (void (*)(char*, void*) )printLong);
	}
	mu_check(nb == ref_nb);
	mu_check(ZRIterator_hasNext(it) == false);
	ZRIterator_destroy(it);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testIteratorDescendantsDF)
{
	ZRTEST_BEGIN();
	ZRTree *tree = makeDoubleTree();
	size_t coord[] = { 0 };
	ZRIterator *it = ZRTreeNode_getDescendants_DF(tree, ZRTREENODE_FROMARRAYCOORDINATE(tree, coord));

	long refs[] = { 11, 111, 112, 1121, 1122, 113 };
	size_t const ref_nb = ZRCARRAY_NBOBJ(refs);
	size_t refs_i;
	size_t nb;

	for (refs_i = 0, nb = 0; ZRIterator_hasNext(it); nb++)
	{
		ZRIterator_next(it);

		if (nb == ref_nb)
			continue;

		double val = *(double*)ZRGraphNode_getObj(ZRTREE_GRAPH(tree), ZRIterator_current(it));
		long val_l = val;

		ZRTEST_RESULT_6(FUN_PREFIX, MESSAGE_BUFF, &val_l, &refs[refs_i++], (int (*)(void*, void*) )cmpLong, (void (*)(char*, void*) )printLong);
	}
	mu_check(nb == ref_nb);
	mu_check(ZRIterator_hasNext(it) == false);
	ZRIterator_destroy(it);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

#include <zrlib/base/Iterator/FilterIterator.h>

bool fvalidate_pair(ZRGraphNode *node, ZRGraph *graph)
{
	double val = *(double*)ZRGraphNode_getObj(graph, node);
	return remainder(val, 2.0) == 0.0;
}

bool fvalidate_moreThan120(ZRGraphNode *node, ZRGraph *graph)
{
	double val = *(double*)ZRGraphNode_getObj(graph, node);
	return val >= 120;
}

/*
 * TODO: create its own test file
 */
MU_TEST(testFilterIteratorOr)
{
	ZRTEST_BEGIN();
	ZRFilterIterator_fvalidate_t fvalidates[] = { //
		(ZRFilterIterator_fvalidate_t)fvalidate_pair, //
		(ZRFilterIterator_fvalidate_t)fvalidate_moreThan120, //
		};

	ZRTree *tree = makeDoubleTree();
	ZRIterator *it;
	size_t nb = 0;
	it = ZRTreeNode_getDescendants(tree, tree->root);
	it = ZRFilterIterator_createOr(it, tree, ZRCARRAY_NBOBJ(fvalidates), fvalidates, ALLOCATOR);

	while (ZRIterator_hasNext(it))
	{
		ZRIterator_next(it);
		mu_check(
			__ fvalidate_pair((ZRGraphNode*)ZRIterator_current(it), (ZRGraph*)tree) == true //
			|| fvalidate_moreThan120((ZRGraphNode*)ZRIterator_current(it), (ZRGraph*)tree) == true//
			);
		nb++;
	}
	mu_check(nb == 7);
	mu_check(ZRIterator_hasNext(it) == false);
	ZRIterator_destroy(it);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

MU_TEST(testFilterIteratorAnd)
{
	ZRTEST_BEGIN();
	ZRFilterIterator_fvalidate_t fvalidates[] = { //
		(ZRFilterIterator_fvalidate_t)fvalidate_pair, //
		(ZRFilterIterator_fvalidate_t)fvalidate_moreThan120, //
		};
	ZRTree *tree = makeDoubleTree();
	ZRIterator *it;
	size_t nb = 0;
	it = ZRTreeNode_getDescendants(tree, tree->root);
	it = ZRFilterIterator_createAnd(it, tree, ZRCARRAY_NBOBJ(fvalidates), fvalidates, ALLOCATOR);

	while (ZRIterator_hasNext(it))
	{
		ZRIterator_next(it);
		mu_check(fvalidate_pair((ZRGraphNode* )ZRIterator_current(it), (ZRGraph* )tree) == true);
		mu_check(fvalidate_moreThan120((ZRGraphNode* )ZRIterator_current(it), (ZRGraph* )tree) == true);
		nb++;
	}
	mu_check(nb == 2);
	mu_check(ZRIterator_hasNext(it) == false);
	ZRIterator_destroy(it);
	ZRGraph_destroy(ZRTREE_GRAPH(tree));
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST_SUITE( AllTests)
{
	MU_RUN_TEST(testGet);
	MU_RUN_TEST(testNNodes);
	MU_RUN_TEST(testNewBuilder);
	MU_RUN_TEST(testNEdges);
	MU_RUN_TEST(testNodeNEdges);
	MU_RUN_TEST(testIteratorChilds);
	MU_RUN_TEST(testIteratorAscendants);
	MU_RUN_TEST(testIteratorDescendants);
	MU_RUN_TEST(testIteratorDescendantsBF);
	MU_RUN_TEST(testIteratorDescendantsDF);
	MU_RUN_TEST(testFilterIteratorOr);
	MU_RUN_TEST(testFilterIteratorAnd);
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

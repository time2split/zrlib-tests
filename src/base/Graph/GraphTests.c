/**
 * @author zuri
 * @date mer. 13 mai 2020 17:59:20 CEST
 */

#define NDEBUG 1

#include <zrlib/base/Graph/SimpleGraph.h>
#include <zrlib/base/macro.h>
#include <zrlib/base/Allocator/CAllocator.h>
#include "../../main.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define TEST_BUILDER_AS_GRAPH 1

#define TYPE_NODE int
#define TYPE_EDGE char

#define ZRGRAPHTEST_BEGIN() \
	ZRTEST_BEGIN(); \
	ZRTEST_PRINTF("for loop I=%lu graph=%s; ", I, TEST_NEW_GRAPH ? "new" : "builder")

// ============================================================================

ZRAllocator *ALLOCATOR;

ZRGraph *GRAPH;
ZRGraphBuilder *GBUILDER;

#define NODES_REF_INITIAL 16

void *NODES_REF[NODES_REF_INITIAL];
void *NODES_REF_TMP[NODES_REF_INITIAL];

size_t NB_EDGES;
size_t NB_NODES;
size_t NB_NODES_REF;

size_t I;
bool TEST_NEW_GRAPH;

struct ConfigS
{
	ZRGraphBuilder* (*fmake)(void);
};
/*
 ZRCONCAT(PREF,X)(test) \
	ZRCONCAT(PREF,X)(getNodes) \
	ZRCONCAT(PREF,X)(cpyEdges) \
	ZRCONCAT(PREF,X)(nodeGetNNodes) \
	ZRCONCAT(PREF,X)(nodeCpyNEdges) \
	ZRCONCAT(PREF,X)(GB_addGraph) \
*/
#define TEST_LIST(PREF) \
	ZRCONCAT(PREF,X)(GB_addGraph) \

static ZRGraphBuilder* makeSimpleBuilder(void)
{
	return ZRSimpleGraphBuilder_create(ZRTYPE_SIZE_ALIGNMENT(TYPE_NODE), ZRTYPE_SIZE_ALIGNMENT(TYPE_EDGE), ALLOCATOR);
}

struct ConfigS CONFIG[] = { //
	makeSimpleBuilder,
	};

// ============================================================================

MU_TEST(testMakeGB)
{
	ZRGRAPHTEST_BEGIN();

	NB_NODES = 2;
	NB_EDGES = 1;

	ZRGraphBuilderNode *gnodea = ZRGRAPHBUILDER_NODE(GBUILDER, (int[] ) { 1 });
	ZRGraphBuilderNode *gnodeb = ZRGRAPHBUILDER_NODE(GBUILDER, (int[] ) { 2 });

	ZRGRAPHBUILDER_EDGE(GBUILDER, gnodea, gnodeb, (char[] ) { 'b' });

	void *nodes[] = { gnodea, gnodeb };
	NB_NODES_REF = ZRCARRAY_NBOBJ(nodes);
	memcpy(NODES_REF, nodes, sizeof(void*) * NB_NODES_REF);
}

MU_TEST(test)
{
	ZRGRAPHTEST_BEGIN();

	ZRTEST_ASSERT_PTR_NE(NULL, NODES_REF[0]);
	ZRTEST_ASSERT_PTR_NE(NULL, NODES_REF[1]);

	ZRTEST_ASSERT_INT_EQ(1, ZRGRAPHNODE_GETNBCHILDS(GRAPH, NODES_REF[0]));
	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBCHILDS(GRAPH, NODES_REF[1]));
	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBPARENTS(GRAPH, NODES_REF[0]));
	ZRTEST_ASSERT_INT_EQ(1, ZRGRAPHNODE_GETNBPARENTS(GRAPH, NODES_REF[1]));

	ZRTEST_ASSERT_INT_EQ(1, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[0]));
	ZRTEST_ASSERT_INT_EQ(2, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[1]));

	ZRTEST_ASSERT_INT_EQ('b', *(char* )ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[0], 0, ZRGraphEdge_selectOUT).obj);
}

MU_TEST(getNodesMakeGB)
{
	ZRGRAPHTEST_BEGIN();
	size_t const nb = 1000;

	NB_NODES = nb;
	NB_EDGES = 0;

// Construct the nodes
	for (size_t i = 0; i < nb; i++)
		ZRGRAPHBUILDER_NODE(GBUILDER, &i);

	NB_NODES_REF = 0;
}

MU_TEST(getNodes)
{
	ZRGRAPHTEST_BEGIN();
	size_t const bufferNodeSize = 12;
	size_t const nbCpyLoop = NB_NODES / bufferNodeSize;
	size_t const cpyLoopRest = NB_NODES % bufferNodeSize;

	ZRTEST_ASSERT_INT_EQ(NB_NODES, GRAPH->nbNodes);

	ZRGraphNode *nodes[bufferNodeSize];
	size_t nbLoop = 0;
	size_t lastNbCpy;

	for (;;)
	{
		lastNbCpy = ZRGRAPH_GETNNODES(GRAPH, nodes, nbLoop * bufferNodeSize, bufferNodeSize);

		for (size_t i = 0; i < lastNbCpy; i++)
			ZRTEST_ASSERT_INT_RANGE(0, NB_NODES - 1, *(int* )ZRGRAPHNODE_GETOBJ(nodes[i]));

		if (lastNbCpy < bufferNodeSize)
			break;

		nbLoop++;
	}
	ZRTEST_ASSERT_INT_EQ(nbCpyLoop, nbLoop);

	if (cpyLoopRest)
		ZRTEST_ASSERT_INT_EQ(lastNbCpy, cpyLoopRest);
}

MU_TEST(cpyEdgesMakeGB)
{
	ZRGRAPHTEST_BEGIN();
	size_t const nb = 200;

	NB_NODES = nb + 1;
	NB_EDGES = nb;

	ZRGraphNode *refa;

	size_t v = 120;
	refa = ZRGRAPHBUILDER_NODE(GBUILDER, &v);

	for (size_t i = 0; i < nb; i++)
		ZRGRAPHBUILDER_EDGE(GBUILDER, refa, ZRGRAPHBUILDER_NODE(GBUILDER, &i), NULL);

	void *nodes[] = { refa };
	NB_NODES_REF = ZRCARRAY_NBOBJ(nodes);
	memcpy(NODES_REF, nodes, sizeof(void*) * NB_NODES_REF);
}

MU_TEST(cpyEdges)
{
	ZRGRAPHTEST_BEGIN();
	size_t const bufferSize = 13;
	size_t const nbCpyLoop = NB_EDGES / bufferSize;
	size_t const cpyLoopRest = NB_EDGES % bufferSize;
	ZRGraphEdge buffer[bufferSize];
	size_t nbLoop = 0;
	size_t lastNbCpy;

	for (;;)
	{
		lastNbCpy = ZRGRAPH_CPYNEDGES(GRAPH, buffer, nbLoop * bufferSize, bufferSize);

		// All must be 0 because of NULL
		for (size_t i = 0; i < lastNbCpy; i++)
		{
			ZRTEST_ASSERT_INT_EQ(0, *(char* )buffer[i].obj);
			ZRTEST_ASSERT_PTR_EQ(NODES_REF[0], buffer[i].a);
		}

		if (lastNbCpy < bufferSize)
			break;

		nbLoop++;
	}
	ZRTEST_ASSERT_INT_EQ(nbCpyLoop, nbLoop);

	if (cpyLoopRest)
		ZRTEST_ASSERT_INT_EQ(lastNbCpy, cpyLoopRest);
}

MU_TEST(nodeGetNNodesMakeGB)
{
	ZRGRAPHTEST_BEGIN();
	size_t const nb = 500;
	size_t const nbNodes = nb + 1;

	NB_NODES = nbNodes;
	NB_EDGES = nb;

	ZRGraphNode *refa, *refb;

	refa = ZRGRAPHBUILDER_NODE(GBUILDER, (int[] ) { 120 });
	refb = ZRGRAPHBUILDER_NODE(GBUILDER, (int[] ) { 200 });
	ZRGRAPHBUILDER_EDGE(GBUILDER, refa, refb, (char[] ) { 'x' });

	for (size_t i = 1; i < nb; i++)
		ZRGRAPHBUILDER_EDGE(GBUILDER, refa, ZRGRAPHBUILDER_NODE(GBUILDER, &i), NULL);

	void *nodes[] = { refa, refb };
	NB_NODES_REF = ZRCARRAY_NBOBJ(nodes);
	memcpy(NODES_REF, nodes, sizeof(void*) * NB_NODES_REF);
}

MU_TEST(nodeGetNNodes)
{
	ZRGRAPHTEST_BEGIN();
	size_t nbNodes;
	ZRGraphNode *buffer[NB_NODES * 2];

	ZRTEST_ASSERT_INT_EQ(NB_NODES - 1, ZRGRAPHNODE_GETNBCHILDS(GRAPH, NODES_REF[0]));
	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBCHILDS(GRAPH, NODES_REF[1]));
	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBPARENTS(GRAPH, NODES_REF[0]));
	ZRTEST_ASSERT_INT_EQ(1, ZRGRAPHNODE_GETNBPARENTS(GRAPH, NODES_REF[1]));

	nbNodes = ZRGRAPHNODE_GETNCHILDS(GRAPH, NODES_REF[0], buffer, 0, ZRCARRAY_NBOBJ(buffer));

	ZRTEST_ASSERT_INT_EQ(NB_NODES - 1, nbNodes);

	nbNodes = ZRGRAPHNODE_GETNPARENTS(GRAPH, NODES_REF[0], buffer, 0, ZRCARRAY_NBOBJ(buffer));
	ZRTEST_ASSERT_INT_EQ(0, nbNodes);
	nbNodes = ZRGRAPHNODE_GETNPARENTS(GRAPH, NODES_REF[1], buffer, 0, ZRCARRAY_NBOBJ(buffer));
	ZRTEST_ASSERT_INT_EQ(1, nbNodes);

	ZRTEST_ASSERT_INT_EQ(120, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[0]));
	ZRTEST_ASSERT_INT_EQ(200, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[1]));
}

MU_TEST(nodeCpyNEdgesMakeGB)
{
	ZRGRAPHTEST_BEGIN();
	size_t const nb = 500;

	NB_NODES = nb + 1;
	NB_EDGES = nb;

	ZRGraphNode *refa, *refb;

	refa = ZRGRAPHBUILDER_NODE(GBUILDER, (int[] ) { 120 });
	refb = ZRGRAPHBUILDER_NODE(GBUILDER, (int[] ) { 200 });
	ZRGRAPHBUILDER_EDGE(GBUILDER, refa, refb, (char[] ) { 'x' });

	for (size_t i = 1; i < nb; i++)
		ZRGRAPHBUILDER_EDGE(GBUILDER, refa, ZRGRAPHBUILDER_NODE(GBUILDER, &i), NULL);

	void *nodes[] = { refa, refb };
	NB_NODES_REF = ZRCARRAY_NBOBJ(nodes);
	memcpy(NODES_REF, nodes, sizeof(void*) * NB_NODES_REF);
}

MU_TEST(nodeCpyNEdges)
{
	ZRGRAPHTEST_BEGIN();
	ZRGraphEdge buffer[NB_EDGES * 2];
	size_t nbEdges;

	ZRTEST_ASSERT_INT_EQ(120, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[0]));
	ZRTEST_ASSERT_INT_EQ(200, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[1]));

	nbEdges = ZRGRAPHNODE_CPYNEDGES(GRAPH, NODES_REF[0], buffer, 0, ZRCARRAY_NBOBJ(buffer), ZRGraphEdge_selectOUT);
	ZRTEST_ASSERT_INT_EQ(NB_EDGES, nbEdges);
	nbEdges = ZRGRAPHNODE_CPYNEDGES(GRAPH, NODES_REF[0], buffer, 0, ZRCARRAY_NBOBJ(buffer), ZRGraphEdge_selectIN);
	ZRTEST_ASSERT_INT_EQ(0, nbEdges);

	nbEdges = ZRGRAPHNODE_CPYNEDGES(GRAPH, NODES_REF[1], buffer, 0, ZRCARRAY_NBOBJ(buffer), ZRGraphEdge_selectOUT);
	ZRTEST_ASSERT_INT_EQ(0, nbEdges);
	nbEdges = ZRGRAPHNODE_CPYNEDGES(GRAPH, NODES_REF[1], buffer, 0, ZRCARRAY_NBOBJ(buffer), ZRGraphEdge_selectIN);
	ZRTEST_ASSERT_INT_EQ(1, nbEdges);
}

MU_TEST(GB_addGraphMakeGB)
{
	/*
	 * 1   --'a'--> 2
	 * 100 --'A'--> 200
	 * 300 --'B'-/
	 */
	ZRGRAPHTEST_BEGIN();
	ZRGraphNode *refa, *refb;
	size_t i = 0;

	refa = ZRGRAPHBUILDER_NODE(GBUILDER, (int[] ) { 1 });
	refb = ZRGRAPHBUILDER_NODE(GBUILDER, (int[] ) { 2 });
	ZRGRAPHBUILDER_EDGE(GBUILDER, refa, refb, (char[] ) { 'a' });
	NODES_REF[i++] = refa;
	NODES_REF[i++] = refb;

	ZRGraphBuilder *builder2 = CONFIG[I].fmake();

	refa = ZRGRAPHBUILDER_NODE(builder2, (int[] ) { 100 });
	refb = ZRGRAPHBUILDER_NODE(builder2, (int[] ) { 200 });
	ZRGRAPHBUILDER_EDGE(builder2, refa, refb, (char[] ) { 'A' });
	NODES_REF[i++] = refa;
	NODES_REF[i++] = refb;

	refa = ZRGRAPHBUILDER_NODE(builder2, (int[] ) { 300 });
	ZRGRAPHBUILDER_EDGE(builder2, refa, refb, (char[] ) { 'B' });
	NODES_REF[i++] = refa;

	ZRGraphBuilder_cpyGraph(GBUILDER, ZRGB_GRAPH(builder2), &NODES_REF[i - 3], 3, ALLOCATOR);

	ZRGRAPH_DESTROY(ZRGB_GRAPH(builder2));

	NB_NODES = 5;
	NB_EDGES = 3;
	NB_NODES_REF = i;
}

MU_TEST(GB_addGraph)
{
	ZRGRAPHTEST_BEGIN();
	ZRTEST_ASSERT_INT_EQ(1, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[0]));
	ZRTEST_ASSERT_INT_EQ(2, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[1]));
	ZRTEST_ASSERT_INT_EQ(100, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[2]));
	ZRTEST_ASSERT_INT_EQ(200, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[3]));
	ZRTEST_ASSERT_INT_EQ(300, *(int* )ZRGRAPHNODE_GETOBJ(NODES_REF[4]));

	ZRTEST_ASSERT_INT_EQ(1, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[0], ZRGraphEdge_selectOUT));
	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[1], ZRGraphEdge_selectOUT));
	ZRTEST_ASSERT_INT_EQ(1, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[2], ZRGraphEdge_selectOUT));
	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[3], ZRGraphEdge_selectOUT));
	ZRTEST_ASSERT_INT_EQ(1, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[4], ZRGraphEdge_selectOUT));

	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[0], ZRGraphEdge_selectIN));
	ZRTEST_ASSERT_INT_EQ(1, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[1], ZRGraphEdge_selectIN));
	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[2], ZRGraphEdge_selectIN));
	ZRTEST_ASSERT_INT_EQ(2, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[3], ZRGraphEdge_selectIN));
	ZRTEST_ASSERT_INT_EQ(0, ZRGRAPHNODE_GETNBEDGES(GRAPH, NODES_REF[4], ZRGraphEdge_selectIN));

	ZRTEST_ASSERT_INT_EQ('a', *(char* )ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[0], 0, ZRGraphEdge_selectOUT).obj);
	ZRTEST_ASSERT_PTR_EQ(000, ________ ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[1], 0, ZRGraphEdge_selectOUT).obj);
	ZRTEST_ASSERT_INT_EQ('A', *(char* )ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[2], 0, ZRGraphEdge_selectOUT).obj);
	ZRTEST_ASSERT_PTR_EQ(000, ________ ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[3], 0, ZRGraphEdge_selectOUT).obj);
	ZRTEST_ASSERT_INT_EQ('B', *(char* )ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[4], 0, ZRGraphEdge_selectOUT).obj);

	ZRTEST_ASSERT_PTR_EQ(000, ________ ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[0], 0, ZRGraphEdge_selectIN).obj);
	ZRTEST_ASSERT_INT_EQ('a', *(char* )ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[1], 0, ZRGraphEdge_selectIN).obj);
	ZRTEST_ASSERT_PTR_EQ(000, ________ ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[2], 0, ZRGraphEdge_selectIN).obj);
	ZRTEST_ASSERT_INT_EQ('A', *(char* )ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[3], 0, ZRGraphEdge_selectIN).obj);
	ZRTEST_ASSERT_INT_EQ('B', *(char* )ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[3], 1, ZRGraphEdge_selectIN).obj);
	ZRTEST_ASSERT_PTR_EQ(000, ________ ZRGRAPHEDGE_CPY(GRAPH, NODES_REF[4], 0, ZRGraphEdge_selectIN).obj);
}

// ============================================================================
// TESTS
// ============================================================================

MU_TEST(GraphStructure)
{
	ZRGRAPHTEST_BEGIN();
	ZRTEST_ASSERT_INT_EQ(NB_NODES, GRAPH->nbNodes);
	ZRTEST_ASSERT_INT_EQ(NB_EDGES, GRAPH->nbEdges);
	ZRTEST_ASSERT_INT_EQ(sizeof(TYPE_NODE), GRAPH->nodeObjSize);
	ZRTEST_ASSERT_INT_EQ(__alignof(TYPE_NODE), GRAPH->nodeObjAlignment);
	ZRTEST_ASSERT_INT_EQ(sizeof(TYPE_EDGE), GRAPH->edgeObjSize);
	ZRTEST_ASSERT_INT_EQ(__alignof(TYPE_EDGE), GRAPH->edgeObjAlignment);
}
#define testGraph() ZRBLOCK( \
	MU_RUN_TEST(FTESTS[j].ftestGraph); \
	MU_RUN_TEST(GraphStructure); \
)

MU_TEST_SUITE( AllTests)
{

#define X(N) N ## MakeGB, N,
	struct
	{
		void (*fmakeGB)(void);
		void (*ftestGraph)(void);
	} FTESTS[] = { TEST_LIST() };
#undef X

	for (I = 0; I < ZRCARRAY_NBOBJ(CONFIG); I++)
	{
		for (size_t j = 0; j < ZRCARRAY_NBOBJ(FTESTS); j++)
		{
			GBUILDER = CONFIG[I].fmake();
			MU_RUN_TEST(FTESTS[j].fmakeGB);

#if TEST_BUILDER_AS_GRAPH
			if (NB_NODES_REF > 0)
				memcpy(NODES_REF_TMP, NODES_REF, sizeof(*NODES_REF) * NB_NODES_REF);

			// Builder Graph
			TEST_NEW_GRAPH = false;
			GRAPH = ZRGB_GRAPH(GBUILDER);
			testGraph();

			if (NB_NODES_REF > 0)
				memcpy(NODES_REF, NODES_REF_TMP, sizeof(*NODES_REF) * NB_NODES_REF);
#endif

			// New Graph
			TEST_NEW_GRAPH = true;
			GRAPH = ZRGraphBuilder_new(GBUILDER, NODES_REF, NB_NODES_REF);
			testGraph();

			ZRGraph_destroy(GRAPH);
			ZRGraph_destroy(ZRGB_GRAPH(GBUILDER));
		}
	}
}

int GraphTests(void)
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

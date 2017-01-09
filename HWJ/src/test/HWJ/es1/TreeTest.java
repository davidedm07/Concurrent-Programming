package test.HWJ.es1;

import static org.junit.Assert.*;

import org.junit.Test;

import source.HWJ.base.Node;
import source.HWJ.base.Tree;

public class TreeTest {

	@Test
	public void testBuildTree() {
		Node root = Tree.buildTree(7);
		int numNodes = Tree.numberOfNodes(root);
		assertEquals(7, numNodes);
	}

}

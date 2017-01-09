package test.HWJ.es3;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

import source.HWJ.base.Node;
import source.HWJ.base.Tree;
import source.HWJ.es3.ImplementedBinaryTreeAdderForkJoin;

public class ImplementedBinaryTreeAdderForkJoinTest {
	private ImplementedBinaryTreeAdderForkJoin adder;
	private int depth;
	private Node root;
	private int nodes;

	@Before
	public void setUp() throws Exception {
		this.adder = new ImplementedBinaryTreeAdderForkJoin();
	}

	@Test
	public void testLittleSizeTree() {
		this.depth = 3;
		this.nodes = (int)(Math.pow(2, this.depth)-1); 
		this.root = Tree.buildTreeDepth(this.depth);
		this.adder.setDepth(this.depth);
		long startTime = System.nanoTime();
		int sum = this.adder.computeOnerousSum(root);
		long endTime = System.nanoTime();
		long parallelTime = endTime - startTime;
		System.out.println("Execution Time Parallel  = " + parallelTime);
		startTime = System.nanoTime();
		sum = this.adder.computeOnerousSerialSum(root);
		endTime = System.nanoTime();
		long serialTime = endTime - startTime;
		System.out.println("Execution Time Serial  = " + serialTime);
		double speedUp = (double)serialTime/(double)parallelTime;
		System.out.println("SpeedUp = " + speedUp);
		assertEquals(this.nodes, sum);
	}
	
	
	@Test
	public void testMediumSizeTree() {
		this.depth = 15;
		this.nodes = (int)(Math.pow(2, this.depth)-1);
		this.root = Tree.buildTreeDepth(this.depth);
		this.adder.setDepth(this.depth);
		long startTime = System.nanoTime();
		int sum = this.adder.computeOnerousSum(root);
		long endTime = System.nanoTime();
		long parallelTime = endTime - startTime;
		System.out.println("Execution Time Parallel  = " + parallelTime);
		assertEquals(this.nodes, sum);
		startTime = System.nanoTime();
		sum = this.adder.computeOnerousSerialSum(root);
		endTime = System.nanoTime();
		long serialTime = endTime - startTime;
		System.out.println("Execution Time Serial  = " + serialTime);
		double speedUp = (double)serialTime/(double)parallelTime;
		System.out.println("SpeedUp = " + speedUp);
		assertEquals(this.nodes, sum);
	 	
	}
	
	
	
	@Test
	public void testBigSizeTree() {
		this.depth = 20;
		this.nodes = (int)(Math.pow(2, this.depth)-1);
		this.root = Tree.buildTreeDepth(this.depth);
		this.adder.setDepth(this.depth);
		long startTime = System.nanoTime();
		int sum = this.adder.computeOnerousSum(root);
		long endTime = System.nanoTime();
		long parallelTime = endTime - startTime;
		System.out.println("Execution Time Parallel  = " + parallelTime);
		startTime = System.nanoTime();
		sum = this.adder.computeOnerousSerialSum(root);
		endTime = System.nanoTime();
		long serialTime = endTime - startTime;
		System.out.println("Execution Time Serial  = " + serialTime);
		double speedUp = (double)serialTime/(double)parallelTime;
		System.out.println("SpeedUp = " + speedUp);
		assertEquals(this.nodes, sum);
	}

}

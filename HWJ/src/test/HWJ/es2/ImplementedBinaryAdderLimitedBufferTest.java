package test.HWJ.es2;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

import source.HWJ.base.Node;
import source.HWJ.base.Tree;
import source.HWJ.es2.ImplementedBinaryAdderLimitedBuffer;


public class ImplementedBinaryAdderLimitedBufferTest {

	private ImplementedBinaryAdderLimitedBuffer adder;
	private int depth;
	private int nodes;
	private Node root;

	@Before
	public void setUp() throws Exception {
		this.adder = new ImplementedBinaryAdderLimitedBuffer();
	}

	@Test
	public void testLittleSizeTree() {
		this.depth = 3;
		this.nodes = (int)(Math.pow(2, this.depth)-1); 
		this.root = Tree.buildTreeDepth(this.depth);
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
	public void testMediumSizeTree() {
		this.depth = 15;
		this.nodes = (int)(Math.pow(2, this.depth)-1);
		this.root = Tree.buildTreeDepth(this.depth);
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
		assertEquals(this.nodes, sum);
		System.out.println("Execution Time Serial  = " + serialTime);
		double speedUp = (double)serialTime/(double)parallelTime;
		System.out.println("SpeedUp = " + speedUp);
		
	 	
	}
	
	@Test
	public void testBigSizeTree() {
		this.depth = 20;
		this.nodes = (int)(Math.pow(2, this.depth)-1);
		this.root = Tree.buildTreeDepth(this.depth);;
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

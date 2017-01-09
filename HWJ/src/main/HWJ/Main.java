package main.HWJ;

import source.HWJ.base.Node;
import source.HWJ.base.Tree;
import source.HWJ.es1.ImplementedBinaryTreeAdder;
import source.HWJ.es2.ImplementedBinaryAdderLimitedBuffer;
import source.HWJ.es3.ImplementedBinaryTreeAdderForkJoin;

public class Main {

	public static void main(String[] args) {
		Node rootLittleTree = Tree.buildTreeDepth(3);
		Node rootMediumTree = Tree.buildTreeDepth(15);
		Node rootBigTree = Tree.buildTreeDepth(20);
		ImplementedBinaryTreeAdder treeAdder = new ImplementedBinaryTreeAdder();
		ImplementedBinaryAdderLimitedBuffer treeAdderLimitedBuffer = new ImplementedBinaryAdderLimitedBuffer();
		ImplementedBinaryTreeAdderForkJoin treeAdderForkJoin = new ImplementedBinaryTreeAdderForkJoin();
		System.out.println("Calculating Speed Up HWJ es.1");
		System.out.println("SpeedUp HWJ es.1 : Small Tree, depth = 3, number of nodes = 7");
		calculateSpeedUp(rootLittleTree,treeAdder);
		System.out.println("SpeedUp HWJ es.1 : Medium Tree, depth = 15, number of nodes = 32767");
		calculateSpeedUp(rootMediumTree,treeAdder);
		System.out.println("SpeedUp HWJ es.1 : Big Tree, depth = 20, number of nodes = 1048575");
		calculateSpeedUp(rootBigTree,treeAdder);
		System.out.println();
		System.out.println("Calculating Speed Up HWJ es.2");
		System.out.println("SpeedUp HWJ es.2 : Small Tree, depth = 3, number of nodes = 7");
		calculateSpeedUpLimitedBuffer(rootLittleTree,treeAdderLimitedBuffer);
		System.out.println("SpeedUp HWJ es.2 : Medium Tree, depth = 15, number of nodes = 32767");
		calculateSpeedUpLimitedBuffer(rootMediumTree,treeAdderLimitedBuffer);
		System.out.println("SpeedUp HWJ es.2 : Big Tree, depth = 20, number of nodes = 1048575");
		calculateSpeedUpLimitedBuffer(rootBigTree,treeAdderLimitedBuffer);
		System.out.println();
		System.out.println("Calculating Speed Up HWJ es.3");
		System.out.println("SpeedUp HWJ es.3 : Small Tree, depth = 3, number of nodes = 7");
		calculateSpeedUpForkJoin(rootBigTree,treeAdderForkJoin,3);
		System.out.println("SpeedUp HWJ es.3 : Medium Tree, depth = 15, number of nodes = 32767");
		calculateSpeedUpForkJoin(rootBigTree,treeAdderForkJoin,15);
		System.out.println("SpeedUp HWJ es.3 : Big Tree, depth = 20, number of nodes = 1048575");
		calculateSpeedUpForkJoin(rootBigTree,treeAdderForkJoin,20);
	
	}
	
	public static void calculateSpeedUp(Node root, ImplementedBinaryTreeAdder adder) {
		long startTime = System.nanoTime();
		int sum = adder.computeOnerousSum(root);
		long endTime = System.nanoTime();
		long parallelTime = endTime - startTime;
		//System.out.println("Execution Time Parallel  = " + parallelTime);
		startTime = System.nanoTime();
		sum = adder.computeOnerousSerialSum(root);
		endTime = System.nanoTime();
		long serialTime = endTime - startTime;
		//System.out.println("Execution Time Serial  = " + serialTime);
		double speedUp = (double)serialTime/(double)parallelTime;
		System.out.println("SpeedUp = " + speedUp);
	}
	
	public static void calculateSpeedUpLimitedBuffer(Node root, ImplementedBinaryAdderLimitedBuffer adder) {
		long startTime = System.nanoTime();
		int sum = adder.computeOnerousSum(root);
		long endTime = System.nanoTime();
		long parallelTime = endTime - startTime;
		//System.out.println("Execution Time Parallel  = " + parallelTime);
		startTime = System.nanoTime();
		sum = adder.computeOnerousSerialSum(root);
		endTime = System.nanoTime();
		long serialTime = endTime - startTime;
		//System.out.println("Execution Time Serial  = " + serialTime);
		double speedUp = (double)serialTime/(double)parallelTime;
		System.out.println("SpeedUp = " + speedUp);
	}
	
	public static void calculateSpeedUpForkJoin(Node root, ImplementedBinaryTreeAdderForkJoin adder,int depth) {
		adder.setDepth(depth);
		long startTime = System.nanoTime();
		int sum = adder.computeOnerousSum(root);
		long endTime = System.nanoTime();
		long parallelTime = endTime - startTime;
		//System.out.println("Execution Time Parallel  = " + parallelTime);
		startTime = System.nanoTime();
		sum = adder.computeOnerousSerialSum(root);
		endTime = System.nanoTime();
		long serialTime = endTime - startTime;
		//System.out.println("Execution Time Serial  = " + serialTime);
		double speedUp = (double)serialTime/(double)parallelTime;
		System.out.println("SpeedUp = " + speedUp);
	}

}

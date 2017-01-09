package source.HWJ.es1;

import java.util.concurrent.CompletionService;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorCompletionService;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import source.HWJ.base.BinaryTreeAdder;
import source.HWJ.base.Node;

public class ImplementedBinaryTreeAdder implements BinaryTreeAdder {

	private int numberOfNodesTree; 
	private int depth;
	private int numberOfCpu;

	public ImplementedBinaryTreeAdder() {
		super();
		this.numberOfCpu = Runtime.getRuntime().availableProcessors();
	}
	
	//balanced and complete binary trees
	public ImplementedBinaryTreeAdder(int depth) {
		this.depth = depth;
		this.numberOfNodesTree = (int)(Math.pow(2,this.depth)-1);
		this.numberOfCpu = Runtime.getRuntime().availableProcessors();
	}

	//generic trees 
	public ImplementedBinaryTreeAdder(int numberOfNodes, int depth) {
		this.numberOfNodesTree = numberOfNodes;
		this.depth = depth;
		this.numberOfCpu = Runtime.getRuntime().availableProcessors();
	}

	@Override
	public int computeOnerousSum(Node root) {
		ExecutorService executor = Executors.newFixedThreadPool(this.numberOfCpu);
		LinkedBlockingQueue<Node> listNodes = new LinkedBlockingQueue<Node>();
		listNodes.add(root);
		int sum = 0; // sum of the values of the tree's nodes
		CyclicBarrierRunnable actionWall = new CyclicBarrierRunnable(executor,listNodes);
		CyclicBarrier wall = new CyclicBarrier(this.numberOfCpu, actionWall);
		actionWall.setBarrier(wall);

		CompletionService<Integer> completionService = new ExecutorCompletionService<Integer>(executor);

		for (int i = 0; i < this.numberOfCpu; i++) 
			completionService.submit(new TreeTask(listNodes,wall));


		for (int i = 0; i < this.numberOfCpu; i++) {
			try {
				sum += completionService.take().get();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				System.out.println("Interrupted Exception");
				e.printStackTrace();
			} catch (ExecutionException e) {
				// TODO Auto-generated catch block
				System.out.println("Execution Execption");
				e.printStackTrace();
			}
		}
		executor.shutdown();
		try {
			executor.awaitTermination(1000, TimeUnit.MILLISECONDS);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			System.out.println("Interrupted Exception after trying to shutdown the executor");
			e.printStackTrace();
		}

		return sum;
	}

	public int computeOnerousSerialSum(Node root) {
		ExecutorService executor = Executors.newFixedThreadPool(1);
		LinkedBlockingQueue<Node> listNodes = new LinkedBlockingQueue<Node>();
		listNodes.add(root);
		int sum = 0; // sum of the values of the tree's nodes
		CyclicBarrierRunnable actionWall = new CyclicBarrierRunnable(executor,listNodes);
		CyclicBarrier wall = new CyclicBarrier(1, actionWall);
		actionWall.setBarrier(wall);
		CompletionService<Integer> completionService = new ExecutorCompletionService<Integer>(executor);

		completionService.submit(new TreeTask(listNodes,wall));


		try {
			sum += completionService.take().get();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			System.out.println("Interrupted Exception");
			e.printStackTrace();
		} catch (ExecutionException e) {
			// TODO Auto-generated catch block
			System.out.println("Execution Execption");
			e.printStackTrace();
		}

		executor.shutdown();
		try {
			executor.awaitTermination(1000, TimeUnit.MILLISECONDS);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			System.out.println("Interrupted Exception after trying to shutdown the executor");
			e.printStackTrace();
		}

		return sum;
	}


	public int getNumberOfNodesTree() {
		return numberOfNodesTree;
	}

	public void setNumberOfNodesTree(int numberOfNodesTree) {
		this.numberOfNodesTree = numberOfNodesTree;
	}
}

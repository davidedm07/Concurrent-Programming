package source.HWJ.es3;

import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.LinkedBlockingDeque;

import source.HWJ.base.BinaryTreeAdder;
import source.HWJ.base.Node;

public class ImplementedBinaryTreeAdderForkJoin implements BinaryTreeAdder {
	private int numberOfCpu;
	private int depth; 
	private long numberOfNodes;
	
	public ImplementedBinaryTreeAdderForkJoin() {
		this.numberOfCpu = Runtime.getRuntime().availableProcessors();
	}

	@Override
	public int computeOnerousSum(Node root) {
		LinkedBlockingDeque<Node> buffer  = new LinkedBlockingDeque<Node>();
		buffer.add(root);
		final ForkJoinPool pool = new ForkJoinPool(this.numberOfCpu);
		final ForkJoinTask task = new ForkJoinTask(root);
		task.setDepth(depth);
		task.setNumberOfNodes((long)(Math.pow(2, numberOfNodes)-1));
		return pool.invoke(task);

	}
	
	public int computeOnerousSerialSum(Node root) {
		LinkedBlockingDeque<Node> buffer  = new LinkedBlockingDeque<Node>();
		buffer.add(root);
		final ForkJoinPool pool = new ForkJoinPool(1);
		final ForkJoinTask task = new ForkJoinTask(root);
		task.setDepth(depth);
		return pool.invoke(task);

	}
	
	public int getDepth() {
		return depth;
	}

	public void setDepth(int depth) {
		this.depth = depth;
	}

	public long getNumberOfNodes() {
		return numberOfNodes;
	}

	public void setNumberOfNodes(long numberOfNodes) {
		this.numberOfNodes = numberOfNodes;
	}
}

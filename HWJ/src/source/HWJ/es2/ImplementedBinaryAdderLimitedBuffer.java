package source.HWJ.es2;

import java.util.concurrent.*;
import source.HWJ.base.BinaryTreeAdder;
import source.HWJ.base.Node;
import source.HWJ.es1.CyclicBarrierRunnable;
import source.HWJ.es1.TreeTask;


public class ImplementedBinaryAdderLimitedBuffer implements BinaryTreeAdder  {

	private int numberOfCpu;

	public ImplementedBinaryAdderLimitedBuffer() {
		this.numberOfCpu = Runtime.getRuntime().availableProcessors();
	}

	@Override
	public int computeOnerousSum(Node root) {
		ExecutorService executor = Executors.newFixedThreadPool(this.numberOfCpu);
		int sum = 0; // sum of the values of the tree's nodes
		ConcurrentLinkedQueue<LinkedBlockingDeque<Node>> buffers = new ConcurrentLinkedQueue<LinkedBlockingDeque<Node>>();
		CyclicBarrierRunnableLimitedBuffer actionWall = new CyclicBarrierRunnableLimitedBuffer(executor,buffers);
		CyclicBarrier wall = new CyclicBarrier(this.numberOfCpu, actionWall);
		actionWall.setBarrier(wall);
		CompletionService<Integer> completionService = new ExecutorCompletionService<Integer>(executor);
		LinkedBlockingDeque<Node> firstThreadlist = new LinkedBlockingDeque<Node>(10);
		firstThreadlist.add(root);
		buffers.add(firstThreadlist);

		completionService.submit(new TreeTaskLimitedBuffer(firstThreadlist,buffers,wall));

		for (int i = 1; i < this.numberOfCpu; i++) {
			LinkedBlockingDeque<Node> threadList =  new LinkedBlockingDeque<Node>(100);
			buffers.add(threadList);
			completionService.submit(new TreeTaskLimitedBuffer(threadList,buffers,wall));
		}

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
			System.out.println("Interrupted Exception after trying to shutodown the executor");
			e.printStackTrace();
		}

		return sum;
	}

	public int computeOnerousSerialSum(Node root) {
		ExecutorService executor = Executors.newFixedThreadPool(1);
		LinkedBlockingQueue<Node> listNodes = new LinkedBlockingQueue<Node>();
		ConcurrentLinkedQueue<LinkedBlockingDeque<Node>> buffers = new ConcurrentLinkedQueue<LinkedBlockingDeque<Node>>();
		listNodes.add(root);
		int sum = 0; // sum of the values of the tree's nodes
		CyclicBarrierRunnableLimitedBuffer actionWall = new CyclicBarrierRunnableLimitedBuffer(executor,buffers);
		CyclicBarrier wall = new CyclicBarrier(this.numberOfCpu, actionWall);
		actionWall.setBarrier(wall);
		CompletionService<Integer> completionService = new ExecutorCompletionService<Integer>(executor);

		LinkedBlockingDeque<Node> firstThreadlist = new LinkedBlockingDeque<Node>(10);
		firstThreadlist.add(root);
		buffers.add(firstThreadlist);
		
		for (int i = 1; i < this.numberOfCpu; i++) {
			LinkedBlockingDeque<Node> threadList =  new LinkedBlockingDeque<Node>(10);
			buffers.add(threadList);
		}
		completionService.submit(new TreeTaskLimitedBuffer(firstThreadlist,buffers,wall));
	
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
			System.out.println("Interrupted Exception after trying to shutodown the executor");
			e.printStackTrace();
		}

		return sum;

	}
}

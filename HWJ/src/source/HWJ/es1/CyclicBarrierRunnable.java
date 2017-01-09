package source.HWJ.es1;

import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;

import source.HWJ.base.Node;

public class CyclicBarrierRunnable implements Runnable {
	
	CyclicBarrier wall = null;
	private ExecutorService executor;
	private LinkedBlockingQueue<Node> listOfNodes;

	public CyclicBarrierRunnable(ExecutorService executor,LinkedBlockingQueue<Node> nodes) {
		this.executor = executor;
		this.listOfNodes = nodes;
	}
	
	@Override
	public void run() {
		if(this.listOfNodes.isEmpty()){
			this.executor.shutdown();
		}
		else{
			this.wall.reset();
		}
	}

	public void setBarrier(CyclicBarrier barrier){
		this.wall = barrier;
	}

}

package source.HWJ.es2;

import java.util.Iterator;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingDeque;

import source.HWJ.base.Node;

public class CyclicBarrierRunnableLimitedBuffer implements Runnable {

	CyclicBarrier wall = null;
	private ExecutorService executor;
	private ConcurrentLinkedQueue<LinkedBlockingDeque<Node>> buffers;

	public CyclicBarrierRunnableLimitedBuffer(ExecutorService executor,ConcurrentLinkedQueue<LinkedBlockingDeque<Node>> poolDeque) {
		this.executor = executor;
		this.buffers = poolDeque;
	}
	
	@Override
	public void run(){
		boolean test = false;
		Iterator <LinkedBlockingDeque<Node>> iterator = this.buffers.iterator();
		while(iterator.hasNext() && !test){
			LinkedBlockingDeque<Node> list = iterator.next();
			if(!list.isEmpty()){
				test = true;
			}
		}
		if(test)
			this.wall.reset();
		else
			this.executor.shutdown();
	}

	public void setBarrier(CyclicBarrier wall){
		this.wall = wall;
	}

}

package source.HWJ.es1;

import java.util.concurrent.*;

import source.HWJ.base.Node;
import source.HWJ.base.Processor;

public class TreeTask implements  Callable <Integer> {

	private LinkedBlockingQueue<Node> nodes;
	private CyclicBarrier wall;

	public TreeTask(LinkedBlockingQueue<Node> nodes, CyclicBarrier wall) {
		this.nodes = nodes;
		this.wall = wall;
	}

	@Override
	public Integer call() throws Exception {
		
		int value = 0;
		Node current =  this.nodes.poll();
		while(current != null) {
			if(this.wall.getNumberWaiting()>0)
				this.wall.reset();

			if (current.getDx()!= null) 
				this.nodes.add(current.getDx());

			if (current.getSx()!= null) 
				this.nodes.add(current.getSx());

			value += new Processor(1000).onerousFunction(current.getValue());

			current = this.nodes.poll();
		}
		return value;

	}

}

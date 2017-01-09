package source.HWJ.es2;

import source.HWJ.base.Node;
import source.HWJ.base.Processor;

import java.util.Iterator;
import java.util.concurrent.*;

public class TreeTaskLimitedBuffer implements Callable <Integer> {

	private LinkedBlockingDeque<Node> nodes;
	private ConcurrentLinkedQueue<LinkedBlockingDeque<Node>> buffers;
	private CyclicBarrier wall;

	public TreeTaskLimitedBuffer(LinkedBlockingDeque<Node> nodes,
			ConcurrentLinkedQueue<LinkedBlockingDeque<Node>> buffers,
			CyclicBarrier wall) {

		this.nodes = nodes;
		this.buffers = buffers;
		this.wall = wall;
	}

	@Override
	public Integer call() throws Exception {
		int value = 0;
		while (!checkAllBuffersEmpty()) {
			if (this.nodes.isEmpty()){
				Node stolenNode = stealNode();
				if (stolenNode!= null) {
					this.nodes.offer(stolenNode);
					
				}
			}
			if(this.wall.getNumberWaiting()>0)
				this.wall.reset();
				value += sum();

		}

		return value;
	}

	/**
	 * This method processes the nodes of the tree and
	 * fills the deque of the buffer until only 2 spots are free.
	 * After that it calls a recursive method to progress the sum in order to
	 * process and put only one node at a time in the buffer 
	 * @return the sum of the nodes of the tree
	 */
	public int sum() {
		int sum = 0;
		while (!this.nodes.isEmpty() && this.nodes.remainingCapacity()>=2) {
			Node current = this.nodes.pollLast();
			if(current == null)
				return sum;
			if (current.getDx()!= null) 
				this.nodes.add(current.getDx());

			if (current.getSx()!= null) 
				this.nodes.add(current.getSx());

			sum += new Processor(1000).onerousFunction(current.getValue());
		}

		sum += sumRecursive();

		return sum;

	}
	
	/**
	 * This method processes the sum of the nodes in a recursive way in order to
	 * not fill the dimension of the buffer
	 * @return the partial sum of the nodes of the tree
	 */
	public int sumRecursive() {
		Node current = this.nodes.pollLast();
		int sum = 0;
		if (current == null) 
			return sum;
		
		if (current.getSx()!=null) {

			if(this.wall.getNumberWaiting()>0)
				this.wall.reset();
			this.nodes.add(current.getSx());
			sum += sumRecursive();
		}

		if (current.getDx()!=null) {
			if(this.wall.getNumberWaiting()>0)
				this.wall.reset();
			this.nodes.add(current.getDx());
			sum += sumRecursive();
		}
		sum += new Processor(1000).onerousFunction(current.getValue());
		return sum;
	}


	/**
	 *  This method steals a node from another Thread's buffer in order to allow 
	 *  the current thread to work.
	 * @return a node 
	 */
	private Node stealNode() {
		boolean found = false;
		Node current = null;
		Iterator<LinkedBlockingDeque<Node>> iterator = this.buffers.iterator();
		while (iterator.hasNext() && !found) {
			LinkedBlockingDeque<Node> list = iterator.next();
				if (!list.isEmpty()) {
					current = list.poll();
					found = true;
				}
		}
		return current;
	}

	/**
	 * Check if all the buffers of the threads are empty
	 *  It's the termination condition of the algorithm
	 * @return a boolean, true if all the buffers are empty and false otherwise
	 */
	public synchronized boolean checkAllBuffersEmpty() {
		boolean allEmpty = true;
		Iterator<LinkedBlockingDeque<Node>> iterator = this.buffers.iterator();
		while (iterator.hasNext() && allEmpty) {
			LinkedBlockingDeque<Node> list = iterator.next();
			if (!list.isEmpty())
				allEmpty = false;
		}
		return allEmpty;
	}

}

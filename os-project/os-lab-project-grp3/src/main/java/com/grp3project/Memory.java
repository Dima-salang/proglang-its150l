package com.grp3project;

import java.util.ArrayList;
import java.util.Comparator;

public class Memory {
    private int size;
    private ArrayList<Process> memArray;
    private FreeList freeList;

    private int freeMemory;

    public Memory(int size) {
        this.size = size;
        this.memArray = new ArrayList<Process>();
        this.freeList = new FreeList(size);
        this.freeMemory = size;
    }

    public int getFreeMemory() {
        return freeMemory;
    }

    public void setFreeMemory(int freeMemory) {
        this.freeMemory = freeMemory;
    }

    public int getSize() {
        return size;
    }

    public ArrayList<Process> getMemArray() {
        return memArray;
    }

    public FreeList getFreeList() {
        return freeList;
    }

    // we add the process to the memory array, which is also the running processes
    // array
    public boolean addProcess(Process process) {
        if (freeMemory < process.getSize()) {
            System.out.println("Not enough memory");
            return false;
        }

        // we get the first fit free block from the free list
        FreeBlock freeBlock = freeList.getFirstFit(process.getSize());
        if (freeBlock == null) {
            System.out.println("No free block found");
            return false;
        }

        process.setStartAddress(freeBlock.getStartAddress());
        process.setEndAddress(freeBlock.getEndAddress());
        memArray.add(process);
        freeMemory -= process.getSize();
        return true;
    }

    // remove the process from the memory array and add free block to the free list
    public void removeProcess(Process process) {
        memArray.remove(process);
        freeList.addFreeBlock(process.getStartAddress(), process.getEndAddress());
        freeMemory += process.getSize();
    }

    // the compaction process for the memory
    public void compaction() {
        // sort processes by starting address to ensure correct order
        memArray.sort(Comparator.comparingInt(Process::getStartAddress));

        // we move all the processes to the start of the array
        int nextStart = 0;
        for (Process process : memArray) {
            process.setStartAddress(nextStart);
            process.setEndAddress(nextStart + process.getSize() - 1);
            nextStart = process.getEndAddress() + 1;
        }

        // we clear the free list and then add a new big free block after the running processes
        freeList.getFreeList().clear();

        freeList.addFreeBlock(nextStart, size - 1);

        // we update the free memory
        freeMemory = size - nextStart;
    }

    // we coalesce the free list
    public void coalesce() {
        freeList.coalesceFreeList();
    }
}

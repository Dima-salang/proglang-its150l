package com.grp3project;

import java.util.ArrayList;

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

    public void addProcess(Process process) {
        if (freeMemory < process.getSize()) {
            throw new RuntimeException("Not enough memory");
        }
        FreeBlock freeBlock = freeList.getFirstFit(process.getSize());
        if (freeBlock == null) {
            throw new RuntimeException("Not enough memory");
        }
        process.setStartAddress(freeBlock.getStartAddress());
        process.setEndAddress(freeBlock.getEndAddress());
        memArray.add(process);
        freeMemory -= process.getSize();
    }

    public void removeProcess(Process process) {
        memArray.remove(process);
        freeList.addFreeBlock(process.getStartAddress(), process.getEndAddress());
        freeMemory += process.getSize();
    }


    public void compaction() {
        int prevEndAddress = -1;
        for (Process process : memArray) {
            process.setStartAddress(prevEndAddress+1);
            process.setEndAddress(process.getStartAddress() + process.getSize());
            prevEndAddress = process.getEndAddress();
        }
        freeList.compaction(prevEndAddress);
    }
}

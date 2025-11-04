package com.grp3project;

public class FreeBlock {
    private int startAddress;
    private int endAddress;
    private int size;
    
    public FreeBlock(int startAddress, int endAddress) {
        this.startAddress = startAddress;
        this.endAddress = endAddress;
        this.size = endAddress - startAddress + 1;
    }

    public int getStartAddress() {
        return startAddress;
    }

    public int getEndAddress() {
        return endAddress;
    }

    public void setStartAddress(int startAddress) {
        this.startAddress = startAddress;
    }

    public void setEndAddress(int endAddress) {
        this.endAddress = endAddress;
    }

    public int getSize() {
        return size;
    }
}

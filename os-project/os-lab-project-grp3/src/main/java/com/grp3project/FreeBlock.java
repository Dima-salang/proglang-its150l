package com.grp3project;

import javafx.beans.property.SimpleIntegerProperty;

public class FreeBlock {
    private SimpleIntegerProperty startAddress;
    private SimpleIntegerProperty endAddress;
    
    public FreeBlock(int startAddress, int endAddress) {
        this.startAddress = new SimpleIntegerProperty(startAddress);
        this.endAddress = new SimpleIntegerProperty(endAddress);
    }

    public int getStartAddress() {
        return startAddress.get();
    }

    public int getEndAddress() {
        return endAddress.get();
    }

    public void setStartAddress(int startAddress) {
        this.startAddress.set(startAddress);
    }

    public void setEndAddress(int endAddress) {
        this.endAddress.set(endAddress);
    }

    public int getSize() {
        return endAddress.get() - startAddress.get() + 1;
    }
}

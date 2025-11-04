package com.grp3project;

import java.util.ArrayList;

public class FreeList {
    private int size;
    private ArrayList<FreeBlock> freeList;

    public FreeList(int size) {
        this.size = size;
        this.freeList = new ArrayList<FreeBlock>();
        this.freeList.add(new FreeBlock(0, size));
    }

    public int getSize() {
        return size;
    }

    public ArrayList<FreeBlock> getFreeList() {
        return freeList;
    }

    public void addFreeBlock(int startAddress, int endAddress) {
        freeList.add(new FreeBlock(startAddress, endAddress));
    }

    public void removeFreeBlock(int startAddress, int endAddress) {
        for (int i = 0; i < freeList.size(); i++) {
            if (freeList.get(i).getStartAddress() == startAddress && freeList.get(i).getEndAddress() == endAddress) {
                freeList.remove(i);
                return;
            }
        }
    }

    public FreeBlock getFirstFit(int size) {
        for (FreeBlock freeBlock : freeList) {
            int freeBlockSize = freeBlock.getEndAddress() - freeBlock.getStartAddress();
            if (freeBlockSize >= size) {
                // if the free block is the same size as the process size we just return the free block itself
                if (freeBlockSize == size) {
                    freeList.remove(freeBlock);
                    return freeBlock;
                }
                // only get the amount of size we need and divide the free block

                // delete the free block
                freeList.remove(freeBlock);

                // create the division
                int returnedStartAddress = freeBlock.getStartAddress();
                int returnedEndAddress = freeBlock.getStartAddress() + size;

                // new free block for the process
                addFreeBlock(returnedStartAddress, returnedEndAddress);

                // new extra free block after the new block since we divided
                addFreeBlock(returnedEndAddress+1, freeBlock.getEndAddress());
                return new FreeBlock(returnedStartAddress, returnedEndAddress);
            }
        }
        return null;
    }


    public void coalesceFreeList() {
        if (freeList.isEmpty()) {
            return;
        }


        int left = 0;
        int right = 1;

        while (right < freeList.size()) {
            if (freeList.get(left).getEndAddress() + 1 == freeList.get(right).getStartAddress()) {
                freeList.get(left).setEndAddress(freeList.get(right).getEndAddress());
                freeList.remove(right);
            } else {
                left++;
                right++;
            }
        }
    }

    public void compaction(int prevProcessEndAddress) {
        for (int i = 0; i < freeList.size(); i++) {
            FreeBlock freeBlock = freeList.get(i);
            freeBlock.setStartAddress(prevProcessEndAddress+1);
            freeBlock.setEndAddress(freeBlock.getStartAddress() + freeBlock.getEndAddress());
            prevProcessEndAddress = freeBlock.getEndAddress();
        }
    }
}

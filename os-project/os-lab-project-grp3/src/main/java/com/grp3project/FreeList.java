package com.grp3project;

import java.util.ArrayList;
import java.util.Iterator;

public class FreeList {
    private int size;
    private ArrayList<FreeBlock> freeList;

    public FreeList(int size) {
        this.size = size;
        this.freeList = new ArrayList<FreeBlock>();
        this.freeList.add(new FreeBlock(0, size - 1));
    }

    public int getSize() {
        return size;
    }

    public ArrayList<FreeBlock> getFreeList() {
        return freeList;
    }

    // if there is a removed process, we add another free block with that process
    // size
    public void addFreeBlock(int startAddress, int endAddress) {
        freeList.add(new FreeBlock(startAddress, endAddress));
    }

    // if there is an added process, we remove from the free list
    public void removeFreeBlock(int startAddress, int endAddress) {
        for (int i = 0; i < freeList.size(); i++) {
            if (freeList.get(i).getStartAddress() == startAddress && freeList.get(i).getEndAddress() == endAddress) {
                freeList.remove(i);
                return;
            }
        }
    }

    // we use the first fit algorithm since it is the simplest
    // and it is also faster as said in the book
    public FreeBlock getFirstFit(int processSize) {
        Iterator<FreeBlock> iterator = freeList.iterator();
        while (iterator.hasNext()) {
            FreeBlock block = iterator.next();
            int blockSize = block.getEndAddress() - block.getStartAddress() + 1;

            if (blockSize >= processSize) {
                int allocStart = block.getStartAddress();
                int allocEnd = allocStart + processSize - 1;

                block.setStartAddress(allocEnd + 1);

                if (block.getStartAddress() > block.getEndAddress()) {
                    iterator.remove();
                }

                return new FreeBlock(allocStart, allocEnd);
            }
        }
        return null;
    }


    // we coalesce the free list to merge together consecutive free blocks
    public void coalesceFreeList() {
        if (freeList.size() < 2) {
            return;
        }

        // sort free list by start address
        freeList.sort(java.util.Comparator.comparingInt(FreeBlock::getStartAddress));


        // we use a two-window approach for coalescing
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
}

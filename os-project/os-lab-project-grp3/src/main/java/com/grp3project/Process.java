package com.grp3project;

public class Process {
    private int pid;
    private int size;
    private int burstTime;
    private int startAddress;
    private int endAddress;

    public Process(int pid, int size, int burstTime) {
        this.pid = pid;
        this.size = size;
        this.burstTime = burstTime;
        this.startAddress = -1;
        this.endAddress = -1;
    }

    public int getPid() {
        return pid;
    }

    public int getSize() {
        return size;
    }

    public int getBurstTime() {
        return burstTime;
    }

    public void setBurstTime(int burstTime) {
        this.burstTime = burstTime;
    }

    public void setSize(int size) {
        this.size = size;
    }

    public void setPid(int pid) {
        this.pid = pid;
    }

    public int getStartAddress() {
        return startAddress;
    }

    public void setStartAddress(int startAddress) {
        this.startAddress = startAddress;
    }

    public int getEndAddress() {
        return endAddress;
    }

    public void setEndAddress(int endAddress) {
        this.endAddress = endAddress;
    }
}

package com.grp3project;

public class Process {
    private int pid;
    private int size;
    private int burstTime;
    private int arrivalTime;
    private int waitingTime;
    private int turnaroundTime;
    private int responseTime; 
    private int startAddress;
    private int endAddress;
    private int remainingTime;
    private boolean isFinished;

    public Process(int pid, int size, int burstTime, int arrivalTime) {
        this.pid = pid;
        this.size = size;
        this.burstTime = burstTime;
        this.arrivalTime = arrivalTime;
        this.startAddress = -1;
        this.endAddress = -1;
        this.remainingTime = burstTime;
        this.isFinished = false;
        this.waitingTime = 0;
        this.turnaroundTime = 0;
        this.responseTime = 0;
    }

    public void reset() {
        this.waitingTime = 0;
        this.turnaroundTime = 0;
        this.responseTime = 0;
        this.startAddress = -1;
        this.endAddress = -1;
        this.remainingTime = this.burstTime;
        this.isFinished = false;
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

    public int getArrivalTime() {
        return arrivalTime;
    }

    public int getWaitingTime() {
        return waitingTime;
    }

    public int getTurnaroundTime() {
        return turnaroundTime;
    }

    public int getResponseTime() {
        return responseTime;
    }

    public void setWaitingTime(int waitingTime) {
        this.waitingTime = waitingTime;
    }

    public void setTurnaroundTime(int turnaroundTime) {
        this.turnaroundTime = turnaroundTime;
    }

    public void setResponseTime(int responseTime) {
        this.responseTime = responseTime;
    }

    public void setArrivalTime(int arrivalTime) {
        this.arrivalTime = arrivalTime;
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

    public int getRemainingTime() {
        return remainingTime;
    }

    public void setRemainingTime(int remainingTime) {
        this.remainingTime = remainingTime;
    }

    public boolean isFinished() {
        return isFinished;
    }

    public void setFinished(boolean finished) {
        isFinished = finished;
    }
}

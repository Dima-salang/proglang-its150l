package com.grp3project;

import java.util.ArrayList;
import java.util.Iterator;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.util.Duration;

public class Simulator {
    private Memory memory;
    private ArrayList<Process> processList;
    private ArrayList<Process> readyQueue;
    private int time;
    private Timeline timeline;
    private int coalesceInterval;
    private int compactionInterval;

    public Simulator(int size, int coalesceInterval, int compactionInterval) {
        this.memory = new Memory(size);
        this.processList = new ArrayList<Process>();
        this.readyQueue = new ArrayList<Process>();
        this.coalesceInterval = coalesceInterval;
        this.compactionInterval = compactionInterval;
    }

    public void run() {
        timeline = new Timeline(new KeyFrame(Duration.seconds(1), event -> {
            checkFinishedProcesses(time);
            checkArrival(time);
            checkReadyQueue();
            if (checkCoalesce(time)) {
                memory.coalesce();
            }
            if (checkCompaction(time)) {
                memory.compaction();
            }
            System.out.println("Time: " + time);
            printRunningProcesses();
            printReadyQueue();
            printFreeList();
            updateRunningProcesses();
            time++;
        }));
        timeline.setCycleCount(Timeline.INDEFINITE);
        timeline.play();
    }

    public void checkArrival(int time) {
        for (Process process : processList) {
            if (process.getArrivalTime() == time && !readyQueue.contains(process) && !process.isFinished()) {
                readyQueue.add(process);
            }
        }
    }

    public void checkFinishedProcesses(int time) {
        Iterator<Process> iterator = memory.getMemArray().iterator();
        while (iterator.hasNext()) {
            Process process = iterator.next();
            if (process.getRemainingTime() <= 0) {
                process.setFinished(true);
                memory.removeProcess(process);
                iterator.remove();
            }
        }
    }

    public void checkReadyQueue() {
        Iterator<Process> iterator = readyQueue.iterator();
        while (iterator.hasNext()) {
            Process process = iterator.next();
            if (memory.addProcess(process)) {
                iterator.remove();
            }
        }
    }

    public void updateRunningProcesses() {
        Iterator<Process> iterator = memory.getMemArray().iterator();
        while (iterator.hasNext()) {
            Process process = iterator.next();
            process.setRemainingTime(process.getRemainingTime() - 1);
            iterator.remove();
        }
    }



    public boolean checkCoalesce(double time) {
        if (time % coalesceInterval == 0) {
            memory.coalesce();
            return true;
        }
        return false;
    }

    public boolean checkCompaction(double time) {
        if (time % compactionInterval == 0) {
            memory.compaction();
            return true;
        }
        return false;
    }

    private boolean processesFinished() {
        return processList.stream().allMatch(process -> process.isFinished());
    }


    private void printRunningProcesses() {
        System.out.println("Running processes:");
        for (Process process : memory.getMemArray()) {
            System.out.println(process.getPid());
        }
    }

    private void printReadyQueue() {
        System.out.println("Ready queue:");
        for (Process process : readyQueue) {
            System.out.println(process.getPid());
        }
    }

    private void printFreeList() {
        System.out.println("Free list:");
        for (FreeBlock block : memory.getFreeList().getFreeList()) {
            System.out.println(block.getStartAddress() + " - " + block.getEndAddress());
        }
    }

    public Memory getMemory() {
        return memory;
    }

    public ArrayList<Process> getProcessList() {
        return processList;
    }

    public int getCurrentTime() {
        return time;
    }

    public ArrayList<Process> getReadyQueue() {
        return readyQueue;
    }
}

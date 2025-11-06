package com.grp3project;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.*;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.cell.TextFieldTableCell;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.layout.Border;
import javafx.scene.layout.BorderStroke;
import javafx.scene.layout.BorderStrokeStyle;
import javafx.scene.layout.BorderWidths;
import javafx.scene.layout.CornerRadii;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Pane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.util.Duration;
import javafx.util.converter.IntegerStringConverter;

public class SimulatorController {

    private int seqArrivalTime = 0;

    @FXML private TitledPane settingsPane;
    @FXML private TitledPane processPane;

    @FXML private Button startButton;
    @FXML private Button stopButton;

    @FXML private TextField memorySize;
    @FXML private TextField coalesceInterval;
    @FXML private TextField compactionInterval;

    @FXML private TableView<Process> processTable;
    @FXML private TableColumn<Process, Integer> pidCol;
    @FXML private TableColumn<Process, Integer> sizeCol;
    @FXML private TableColumn<Process, Integer> burstCol;
    @FXML private TableColumn<Process, Integer> arrivalCol;

    @FXML private Label timeLabel;
    @FXML private ScrollPane memoryScrollPane;
    @FXML private HBox memoryDisplay;
    
    @FXML private TableView<FreeBlock> freeListTable;
    @FXML private TableColumn<FreeBlock, Integer> startAddressCol;
    @FXML private TableColumn<FreeBlock, Integer> endAddressCol;
    @FXML private TableColumn<FreeBlock, Integer> freeSizeCol;

    @FXML private TableView<Process> readyQueueTable;
    @FXML private TableColumn<Process, Integer> readyPidCol;
    @FXML private TableColumn<Process, Integer> readySizeCol;


    private Simulator simulator;
    private Timeline uiUpdater;
    private final ObservableList<Process> processList = FXCollections.observableArrayList();
    private final HashMap<Integer, Color> processColors = new HashMap<>();


    @FXML
    private void initialize() {
        processTable.setEditable(true);
        pidCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getPid()).asObject());
        sizeCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getSize()).asObject());
        burstCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getBurstTime()).asObject());
        arrivalCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getArrivalTime()).asObject());

        sizeCol.setCellFactory(TextFieldTableCell.forTableColumn(new IntegerStringConverter()));
        sizeCol.setOnEditCommit(e -> e.getRowValue().setSize(e.getNewValue()));
        burstCol.setCellFactory(TextFieldTableCell.forTableColumn(new IntegerStringConverter()));
        burstCol.setOnEditCommit(e -> e.getRowValue().setBurstTime(e.getNewValue()));
        arrivalCol.setCellFactory(TextFieldTableCell.forTableColumn(new IntegerStringConverter()));
        arrivalCol.setOnEditCommit(e -> e.getRowValue().setArrivalTime(e.getNewValue()));
        processTable.setItems(processList);

        startAddressCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getStartAddress()).asObject());
        endAddressCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getEndAddress()).asObject());
        freeSizeCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getSize()).asObject());

        readyPidCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getPid()).asObject());
        readySizeCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getSize()).asObject());
    }

    @FXML
    private void addProcess() {
        seqArrivalTime += 2;
        int nextPid = processList.isEmpty() ? 1 : processList.stream().mapToInt(Process::getPid).max().orElse(0) + 1;
        Process newProc = new Process(nextPid, 100, 5, seqArrivalTime);
        processList.add(newProc);

    }

    @FXML
    private void removeSelectedProcess() {
        Process selected = processTable.getSelectionModel().getSelectedItem();
        if (selected != null) processList.remove(selected);
        seqArrivalTime -= 2;
    }

    @FXML
    private void startSimulator() {
        if (processList.isEmpty()) {
            new Alert(AlertType.WARNING, "Please add at least one process!").showAndWait();
            return;
        }

        try {
            int memSize = Integer.parseInt(memorySize.getText());
            int coalesceInt = Integer.parseInt(coalesceInterval.getText());
            int compactInt = Integer.parseInt(compactionInterval.getText());

            // reset processes to initial state
            processList.forEach(Process::reset);

            processColors.clear();
            for (int i = 0; i < processList.size(); i++) {
                Process p = processList.get(i);
                // generate unique colors
                Color color = Color.hsb(i * (360.0 / processList.size()), 0.75, 0.9);
                processColors.put(p.getPid(), color);
            }

            simulator = new Simulator(memSize, coalesceInt, compactInt);
            simulator.getProcessList().addAll(processList);

            // listen for simulation to finish on its own
            simulator.runningProperty().addListener((obs, wasRunning, isNowRunning) -> {
                if (!isNowRunning && wasRunning) {
                    Platform.runLater(() -> {
                        if (uiUpdater != null) {
                            uiUpdater.stop();
                        }
                        showStatisticsWindow(simulator.getFinishedProcesses());
                        resetSimulationUI();
                    });
                }
            });

            simulator.run();

            uiUpdater = new Timeline(new KeyFrame(Duration.millis(500), e -> refreshUI()));
            uiUpdater.setCycleCount(Timeline.INDEFINITE);
            uiUpdater.play();

            setUIState(true); // set UI to "running" state

        } catch (NumberFormatException e) {
            new Alert(AlertType.ERROR, "Please fill all settings with valid numbers.").showAndWait();
        }
    }

    @FXML
    private void onStop() {
        if (simulator == null) return;
        simulator.stopSimulator();
    }

    private void resetSimulationUI() {
        setUIState(false);
        
        // clear all simulation-specific UI elements
        timeLabel.setText("Time: 0s");
        memoryDisplay.getChildren().clear();
        freeListTable.getItems().clear();
        readyQueueTable.getItems().clear();
        
        simulator = null;
        if (uiUpdater != null) {
            uiUpdater.stop();
            uiUpdater = null;
        }
    }

    private void setUIState(boolean isRunning) {
        // disable settings while running
        settingsPane.setDisable(isRunning);
        processPane.setDisable(false);
        
        // toggle run-control buttons
        startButton.setDisable(isRunning);
        stopButton.setDisable(!isRunning);
    }

    private void showStatisticsWindow(ArrayList<Process> finishedProcesses) {
        if (finishedProcesses == null || finishedProcesses.isEmpty()) {

            int totalProcesses = (simulator != null) ? simulator.getProcessList().size() : processList.size();
            if (totalProcesses > 0) {
                 new Alert(AlertType.INFORMATION, "Simulation stopped before any processes could finish.").show();
            } else {
                 new Alert(AlertType.INFORMATION, "Simulation stopped. No processes were run.").show();
            }
            return;
        }
        
        Alert alert = new Alert(AlertType.INFORMATION);
        alert.setTitle("Simulation Statistics");
        alert.setHeaderText("Simulation Finished!");

        TableView<Process> statsTable = new TableView<>();
        statsTable.setItems(FXCollections.observableArrayList(finishedProcesses));

        TableColumn<Process, String> pidCol = new TableColumn<>("PID");
        pidCol.setCellValueFactory(cell -> new SimpleStringProperty("P" + cell.getValue().getPid()));

        TableColumn<Process, Integer> turnaroundCol = new TableColumn<>("Turnaround Time");
        turnaroundCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getTurnaroundTime()).asObject());

        TableColumn<Process, Integer> waitingCol = new TableColumn<>("Waiting Time");
        waitingCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getWaitingTime()).asObject());

        TableColumn<Process, Integer> responseCol = new TableColumn<>("Response Time");
        responseCol.setCellValueFactory(cell -> new SimpleIntegerProperty(cell.getValue().getResponseTime()).asObject());
        
        statsTable.getColumns().addAll(pidCol, turnaroundCol, waitingCol, responseCol);
        statsTable.setPrefHeight(250);
        statsTable.setColumnResizePolicy(TableView.CONSTRAINED_RESIZE_POLICY);

        // calculate the averages
        double avgTurnaround = finishedProcesses.stream().mapToInt(Process::getTurnaroundTime).average().orElse(0);
        double avgWaiting = finishedProcesses.stream().mapToInt(Process::getWaitingTime).average().orElse(0);
        
        Label averages = new Label(String.format("Average Turnaround Time: %.2f\nAverage Waiting Time: %.2f", avgTurnaround, avgWaiting));
        averages.setPadding(new Insets(10));

        VBox content = new VBox(10, statsTable, averages);
        
        // put it in the dialog pane for showing the stats
        DialogPane dialogPane = alert.getDialogPane();
        dialogPane.setContent(content);
        dialogPane.setPrefWidth(500);

        alert.showAndWait();
    }

    private void refreshUI() {
        if (simulator == null) return;

        timeLabel.setText("Time: " + simulator.getCurrentTime() + "s");

        freeListTable.setItems(FXCollections.observableArrayList(simulator.getMemory().getFreeList().getFreeList()));
        readyQueueTable.setItems(FXCollections.observableArrayList(simulator.getReadyQueue()));
        

        // update the ui from the state of the controller
        updateMemoryVisuals();
    }

    private void updateMemoryVisuals() {
        if (simulator == null) {
            memoryDisplay.getChildren().clear();
            return;
        }
        
        memoryDisplay.getChildren().clear();

        int totalSize = simulator.getMemory().getSize();
        
        double visualWidth = memoryScrollPane.getWidth() - 4; 
        if (visualWidth <= 0) visualWidth = 800;

        List<MemoryBlock> blocks = new ArrayList<>();

        // add all running processes
        for (Process p : simulator.getMemory().getMemArray()) {
            blocks.add(new MemoryBlock(
                    p.getStartAddress(),
                    p.getSize(),
                    "P" + p.getPid(),
                    processColors.getOrDefault(p.getPid(), Color.ORANGE) // get the color of the process
            ));
        }

        // add all free blocks with lightgray color
        for (FreeBlock f : simulator.getMemory().getFreeList().getFreeList()) {
            blocks.add(new MemoryBlock(
                    f.getStartAddress(),
                    f.getSize(),
                    "Free",
                    Color.LIGHTGRAY
            ));
        }

        blocks.sort(Comparator.comparingInt(MemoryBlock::startAddress));

        for (MemoryBlock block : blocks) {
            Pane blockPane = makeBlock(block, totalSize, visualWidth);
            memoryDisplay.getChildren().add(blockPane);
        }
    }

    private Pane makeBlock(MemoryBlock block, int totalSize, double totalWidth) {
        double width = (double) block.size() / totalSize * totalWidth;
        
        VBox contentBox = new VBox(-2);
        contentBox.setAlignment(Pos.CENTER);

        Label pidLabel = new Label(block.label());
        pidLabel.setTextFill(Color.BLACK);
        pidLabel.setStyle("-fx-font-weight: bold;");

        Label sizeLabel = new Label(block.size() + "K");
        sizeLabel.setTextFill(Color.BLACK);
        sizeLabel.setStyle("-fx-font-size: 9px;");

        if (width > 35) {
            contentBox.getChildren().addAll(pidLabel, sizeLabel);
        } else if (width > 15) {
            contentBox.getChildren().add(pidLabel);
        }
        
        Pane blockPane = new Pane(contentBox);
        blockPane.setPrefWidth(width);
        blockPane.setMinWidth(width);
        blockPane.setMaxWidth(width);
        
        blockPane.setPrefHeight(50);
        blockPane.setBackground(new Background(new BackgroundFill(block.color(), new CornerRadii(3), null)));
        blockPane.setBorder(new Border(new BorderStroke(Color.BLACK, BorderStrokeStyle.SOLID, new CornerRadii(3), new BorderWidths(1))));

        contentBox.layoutXProperty().bind(blockPane.widthProperty().subtract(contentBox.widthProperty()).divide(2));
        contentBox.layoutYProperty().bind(blockPane.heightProperty().subtract(contentBox.heightProperty()).divide(2));

        Tooltip.install(blockPane, new Tooltip(
                String.format("%s\nSize: %dK\nAddress: %d - %d",
                        block.label().startsWith("P") ? "Process " + block.label() : "Free Block",
                        block.size(),
                        block.startAddress(),
                        block.startAddress() + block.size() - 1
                )
        ));

        return blockPane;
    }
}